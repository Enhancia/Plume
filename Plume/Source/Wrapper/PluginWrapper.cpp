/*
  ==============================================================================

    PluginWrapper.cpp
    Created: 29 Jun 2018 3:03:40pm
    Author:  Alex

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"

#if ! (JUCE_PLUGINHOST_VST || JUCE_PLUGINHOST_VST3 || JUCE_PLUGINHOST_AU)
 #error "If you're building the wrapper, you probably want to enable VST and/or AU support"
#endif

PluginWrapper::PluginWrapper (PlumeProcessor& p, GestureArray& gArr, ValueTree pluginDirs)
    : owner (p), gestArray (gArr), customDirectories (pluginDirs)
{
    TRACE_IN;
    // Initializes the booleans
    hasWrappedInstance = false;
    hasOpenedEditor = false;
    
    // Creates the objects to wrap the plugin
    formatManager = new AudioPluginFormatManager();
    formatManager->addFormat (new VSTPluginFormat());
  #if JUCE_MAC
    formatManager->addFormat (new AudioUnitPluginFormat());
  #endif
  #if JUCE_PLUGINHOST_VST3
    formatManager->addFormat (new VST3PluginFormat());
  #endif
    
    pluginList = new KnownPluginList();
    loadPluginListFromFile();
}

PluginWrapper::~PluginWrapper()
{
    TRACE_IN;
    removeAllChangeListeners();
    
    clearWrapperEditor();
	wrapperProcessor = nullptr;
    wrappedInstance = nullptr;
    
    pluginList->clear();
    pluginList = nullptr;
    
    formatManager = nullptr;
}

//==============================================================================
bool PluginWrapper::wrapPlugin (PluginDescription& description)
{
    auto descToWrap = getDescriptionToWrap (description);
    
    if (descToWrap == nullptr)
    {
        DBG ("Error: Couldn't find the plugin for the specified description..\n" <<
			 "Specified description = Name : " << description.name << " | Id : " <<
			 description.uid << " | Format : " << description.pluginFormatName << "\n");
        return false;
    }
    
    if (descToWrap->name == "Plume")
    {
        DBG ("Can't wrap yourself my dude..");
        
        return false;
    }
    
    if (!(descToWrap)->isInstrument)
    {
        DBG ("Specified plugin is not an instrument!!");
        
        return false;
    }
        
    if (hasWrappedInstance)
	{
		unwrapPlugin();
	}
	
    String errorMsg;
    DBG ("\n[create Plugin Instance]");
    wrappedInstance = formatManager->createPluginInstance (*descToWrap,
                                                           owner.getSampleRate(),
														   owner.getBlockSize(),
                                                           errorMsg);
                                                             
    if (wrappedInstance == nullptr)
    {
        DBG ("Error: Failed to create an instance of the plugin, error message:\n\n" << errorMsg);
        return false;
    }
    
    //Creates the wrapped processor object using the instance
    wrappedInstance->enableAllBuses();
    
    wrapperProcessor = new WrapperProcessor (*wrappedInstance, *this);
    wrapperProcessor->prepareToPlay (owner.getSampleRate(), owner.getBlockSize());
    hasWrappedInstance = true;
	
    return true;
}

bool PluginWrapper::wrapPlugin (int pluginMenuId)
{
    int pluginId = pluginList->getIndexChosenByMenu (pluginMenuId);
    
    if (pluginList->getType (pluginId) == nullptr)
    {
        DBG ("Error: Couldn't find the plugin at the specified Id..\nSize of the plugin list: " << pluginList->getNumTypes()
                                                                                                << " | Specified id : "
                                                                                                << pluginId
                                                                                                << "\n");
        return false;
    }
    
    if (pluginList->getType (pluginId)->name == "Plume")
    {
        DBG ("Can't wrap yourself..");
        
        return false;
    }
    
    if (!(pluginList->getType (pluginId)->isInstrument))
    {
        DBG ("Specified plugin is not an instrument..");
        
        return false;
    }
        
    if (hasWrappedInstance)
	{
		unwrapPlugin();
	}
	
    String errorMsg;
    DBG ("\n[create Plugin Instance]");
    wrappedInstance = formatManager->createPluginInstance (*pluginList->getType (pluginId),
                                                           owner.getSampleRate(),
														   owner.getBlockSize(),
                                                           errorMsg);
                                                             
    if (wrappedInstance == nullptr)
    {
        DBG ("Error: Failed to create an instance of the plugin, error message:\n\n" << errorMsg);
        return false;
    }
    
    //Creates the wrapped processor object using the instance
    wrappedInstance->enableAllBuses();
    
    wrapperProcessor = new WrapperProcessor (*wrappedInstance, *this);
    wrapperProcessor->prepareToPlay (owner.getSampleRate(), owner.getBlockSize());
    hasWrappedInstance = true;
	
    return true;
}

void PluginWrapper::unwrapPlugin()
{
    TRACE_IN;
    if (hasWrappedInstance == false)
    {
        return;
    }
    
    if (hasOpenedEditor)
    {
        clearWrapperEditor();
    }
    
    hasWrappedInstance = false;
    
	wrapEd.reset();
    owner.getGestureArray().clearAllParameters();
    wrapperProcessor.reset();
    wrappedInstance.reset();
}

bool PluginWrapper::rewrapPlugin (PluginDescription& description)
{
    unwrapPlugin();
    return wrapPlugin(description);
}

bool PluginWrapper::rewrapPlugin(int pluginId)
{
    unwrapPlugin();
    return wrapPlugin(pluginId);
}

AudioPluginFormat* PluginWrapper::getPluginFormat (File pluginFile)
{
   String ext = pluginFile.getFileExtension();

 #if (JUCE_WINDOWS || JUCE_MAC) && JUCE_PLUGINHOST_VST
  #if JUCE_WINDOWS
   if (ext.compare (".dll") == 0)
  #elif JUCE_MAC
   if (ext.compare (".vst") == 0)
  #endif
   {
       return formatManager->getFormat (Formats::VST);
   }
 #endif

 #if (JUCE_WINDOWS || JUCE_MAC) && JUCE_PLUGINHOST_VST3
   if (ext.compare (".vst3") == 0)
   {
       return formatManager->getFormat (Formats::VST3);
   }
 #endif

 #if JUCE_MAC && JUCE_PLUGINHOST_AU
   if (ext.compare (".component") == 0)
   {
       return formatManager->getFormat (Formats::AU);
   }
 #endif

   return nullptr;
}

bool PluginWrapper::isWrapping()
{
    return hasWrappedInstance;
}

//==============================================================================
void PluginWrapper::createWrapperEditor (const Component* componentWhichWindowToAttachTo)
{
    TRACE_IN;
    
    jassert (wrapperProcessor != nullptr);
    
    if (wrapperProcessor == nullptr)
    {
        return;
    }
    
    if (hasOpenedEditor == true)
    {
        wrapperEditor->toFront (true);
        return;
    }
    
    hasOpenedEditor = true;
        
    if (wrapperEditor == nullptr)
    {
        wrapperEditor = new WrapperEditorWindow (*wrapperProcessor, componentWhichWindowToAttachTo);
		wrapperEditor->toFront (true);
        return;
    }
    
    wrapperEditor.reset (new WrapperEditorWindow (*wrapperProcessor, componentWhichWindowToAttachTo));
}

WrapperEditorWindow* PluginWrapper::getWrapperEditorWindow()
{
    if (hasWrappedInstance)
    {
        return wrapperEditor;
    }

	return nullptr;
}

void PluginWrapper::clearWrapperEditor()
{
    if (hasWrappedInstance && hasOpenedEditor)
    {
        wrapperEditor.reset();
        hasOpenedEditor = false;
        
        // Message for the components needing an update upon closing the wrapped editor (ie mapper components)
        sendChangeMessage();
    }
}

void PluginWrapper::minimiseWrapperEditor (bool shouldBeMinimised)
{
    if (hasWrappedInstance && hasOpenedEditor)
    {
      #if JUCE_WINDOWS
        if (shouldBeMinimised)
        {
            wrapperEditor->getPeer()->setMinimised (true);
        }
        else
        {
            wrapperEditor->getPeer()->setMinimised (false);
            wrapperEditorToFront (true);
        }

      #elif JUCE_MAC
        
        if (shouldBeMinimised)
        {
            wrapperEditor->getPeer()->setVisible (false);
        }
        else
        {
            wrapperEditor->getPeer()->setVisible (true);
            wrapperEditor->toFront (true);
        }
      #endif
    }
}

void PluginWrapper::wrapperEditorToFront (bool shouldAlsoGiveFocus)
{
    if (hasWrappedInstance && hasOpenedEditor)
    {
        if (wrapperEditor->getPeer()->isMinimised()) wrapperEditor->getPeer()->setMinimised (false);
        if (!wrapperEditor->isVisible())             wrapperEditor->getPeer()->setVisible (true);
        wrapperEditor->toFront (shouldAlsoGiveFocus);
    }
}


//==============================================================================
String PluginWrapper::getWrappedPluginName()
{
    if (hasWrappedInstance)
    {
        return wrappedInstance->getPluginDescription().name;
    }
    
    return "No plugin";
}

String PluginWrapper::getWrappedPluginInfoString()
{
    if (hasWrappedInstance)
    {
        auto desc = wrappedInstance->getPluginDescription();
        
        return (desc.name + " (" + desc.version + ") - " + desc.manufacturerName);
    }
    
    return " - ";
}

WrapperProcessor& PluginWrapper::getWrapperProcessor()
{
    return *wrapperProcessor;
}

PlumeProcessor& PluginWrapper::getOwner()
{
    return owner;
}

bool PluginWrapper::hasOpenedWrapperEditor()
{
    return hasOpenedEditor;
}


KnownPluginList& PluginWrapper::getList()
{
    return *pluginList;
}

//==============================================================================
Array<File*> PluginWrapper::createFileList()
{
    Array<File*> directories;
    
    // Adds OS specific paths if they exist
  #if JUCE_WINDOWS

    if (useDefaultPaths)
    {
        File f;
        // C:\Program Files\VSTPlugins 
        f = File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("VSTPlugins/");
        
        if (f.exists()) directories.add (new File (f));
        
        // C:\Program Files\Steinberg\VSTPlugins
        f = File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("Steinberg/")
                                                                        .getChildFile ("VSTPlugins/");
        if (f.exists()) directories.add (new File (f));
        
        // C:\Program Files\Common Files\VST2\                                                              
        f = File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("Common Files/")
                                                                        .getChildFile ("VST2/");
        if (f.exists()) directories.add (new File (f));
        
        // C:\Program Files\Common Files\Steinberg\VST2                                                              
        f = File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("Common Files/")
                                                                        .getChildFile ("Steinberg/")
                                                                        .getChildFile ("VST2/");
        if (f.exists()) directories.add (new File (f));
    }

  #elif JUCE_MAC

    if (useAudioUnits)
    {
        File f;
        // Macintosh HD:/Library/Audio/Plug-Ins/Components/
        f = File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Audio/")
                                                                           .getChildFile ("Plug-Ins/")
                                                                           .getChildFile ("Components/");
        if (f.exists()) directories.add (new File (f));
        
        // ~/Library/Audio/Plug-Ins/Components/                                                               
        f = File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Audio/")
                                                                           .getChildFile ("Plug-Ins/")
                                                                           .getChildFile ("Components/");
        if (f.exists()) directories.add (new File (f));
    }

    if (useDefaultPaths)
    {
        File f;
        // Macintosh HD:/Library/Audio/Plug-Ins/VST/                                                               
        f = File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Audio/")
                                                                           .getChildFile ("Plug-Ins/")
                                                                           .getChildFile ("VST/");
        if (f.exists()) directories.add (new File (f));
        
        // ~/Library/Audio/Plug-Ins/VST/                                                            
        f = File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Audio/")
                                                                           .getChildFile ("Plug-Ins/")
                                                                           .getChildFile ("VST/");
        if (f.exists()) directories.add (new File (f));    
    } 

  #endif
    
    if (useCustomPaths && customDirectories.getNumChildren() != 0)
    {
		for (int i = 0; i < customDirectories.getNumChildren(); i++)
		{
			directories.add (new File (getCustomDirectory (i)));
		}
    }

	return directories;
}

void PluginWrapper::setDefaultPathUsage (bool shouldUseDefaultPaths)
{
    useDefaultPaths = shouldUseDefaultPaths;
}

void PluginWrapper::setCustomPathUsage (bool shouldUseCustomPath)
{
    useCustomPaths = shouldUseCustomPath;
}

void PluginWrapper::setAuUsage (bool                    
                                #if JUCE_MAC
                                    shouldUseAudioUnits
                                #endif
                                )
{
  #if JUCE_MAC
    useAudioUnits = shouldUseAudioUnits;
  #endif 
}

void PluginWrapper::addCustomDirectory (File newDir)
{
    // TODO virer cette ligne et decommenter code quand implémentation plusieurs path
    customDirectories.getChild (0).setProperty (PLUME::treeId::value,
                                                newDir.getFullPathName(),
                                                nullptr);
    
    /*
    if (newDir.exists() && newDir.isDirectory())
    {
        // Checks if it's not already in the array
        for (int i = 0; i < customDirectories.getNumChildren(); i++)
        {
            if (newDir.getFullPathName() == getCustomDirectory (i))
            {
                return;
            }
        }
        
        // Adds the file to the end of the list
        customDirectories.appendChild (ValueTree (PLUME::treeId::directory).setProperty (PLUME::treeId::value,
                                                                                         var (newDir.getFullPathName()),
                                                                                         nullptr),
                                        nullptr);
    }
    */
    
    savePluginListToFile();
}

String PluginWrapper::getCustomDirectory (int numDir)
{
    if (numDir < 0 || numDir > customDirectories.getNumChildren()) return "";
    
    else return customDirectories.getChild (numDir)
                                 .getProperty (PLUME::treeId::value)
                                 .toString();
}

void PluginWrapper::clearCustomDirectories()
{
    //TODO 2eme ligne quand implementation plusieurs paths
    customDirectories.getChild (0).setProperty (PLUME::treeId::value, "", nullptr);
    
    //customDirectories.removeAllChildren (nullptr);
}

void PluginWrapper::scanAllPluginsInDirectories (bool dontRescanIfAlreadyInList, bool ignoreBlackList)
{
    if (formatManager->getNumFormats() == 0 ||
        (!useDefaultPaths && customDirectories.getNumChildren() == 0)) return;
    /*
    if (!dontRescanIfAlreadyInList)
    {
        pluginList->clear();
    }
    */
    
    pluginList->clear();
    
    // Sets all the files to search
    FileSearchPath fsp;
    for (auto* file : createFileList())
    {
        fsp.add (File (*file)); // Creates a copy of the file to prevent leakage / nullptr bad access
		delete file;
    }
        
    for (int i=0; i<formatManager->getNumFormats(); i++)
    {
        PluginDirectoryScanner dirScanner (*pluginList, *formatManager->getFormat (i), fsp, true, File(), true);
        scanProgress = 0.0f;
        
        // Rescans until scanNextFile returns false
        while (dirScanner.scanNextFile (dontRescanIfAlreadyInList, pluginBeingScanned))
        {
            scanProgress = dirScanner.getProgress();
            pluginBeingScanned = pluginBeingScanned.fromLastOccurrenceOf ("\\", false, false);
            DBG ("Scanning : " << pluginBeingScanned << " | Progress : " << scanProgress);
        }
        
        scanProgress = dirScanner.getProgress();
    }
    
    savePluginListToFile();
}

PluginDirectoryScanner* PluginWrapper::getDirectoryScannerForFormat (int formatToScan)
{
    if (formatManager->getNumFormats() == 0 ||
        (!useDefaultPaths && customDirectories.getNumChildren() == 0)) return nullptr;
        
    // Sets all the files to search
    FileSearchPath fsp;
    for (auto* file : createFileList())
    {
        fsp.add (File (*file)); // Creates a copy of the file to prevent leakage / nullptr bad access
		delete file;
    }
        
    return new PluginDirectoryScanner (*pluginList, *formatManager->getFormat (formatToScan),
                                                    fsp, true, File(), true);
}

void PluginWrapper::addPluginsToMenu (PopupMenu& menu, KnownPluginList::SortMethod sort)
{
    pluginList->addToMenu (menu, sort);
}

PluginDescription* PluginWrapper::getDescriptionToWrap (const PluginDescription& description)
{   
    for (auto desc = pluginList->begin(); desc != pluginList->end(); desc++)
    {
        if ((*desc)->uid == description.uid &&
			(*desc)->pluginFormatName == description.pluginFormatName &&
			(*desc)->name == description.name)
        {
            return *desc;
        }
    }
    
    return nullptr;
}

void PluginWrapper::savePluginListToFile()
{
    // Create file if it doesn't exist yet
    File scannedPlugins;
    
  #if JUCE_WINDOWS
    scannedPlugins = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/")
                                                                                  .getChildFile ("Plume/");
  #elif JUCE_MAC
    scannedPlugins = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Application Support/")
                                                                                  .getChildFile ("Plume/");
  #endif
  
    scannedPlugins = scannedPlugins.getChildFile ("plumepl.cfg");
  
    if (!scannedPlugins.exists())
    {
        scannedPlugins.create();
    }
    
    ScopedPointer<XmlElement> listXml = new XmlElement ("PLUME_PLUGINLIST_CONFIG");
    
    // Writes plugin list data into the file
    listXml->addChildElement (pluginList->createXml());
    
    XmlElement* userDirs = listXml->createNewChildElement ("USER_DIRECTORIES");
    userDirs->addChildElement (customDirectories.createXml());
    
    listXml->writeToFile (scannedPlugins, StringRef());
    listXml->deleteAllChildElements();
}
    
void PluginWrapper::loadPluginListFromFile()
{
    // Attempts to find file
	File scannedPlugins;

  #if JUCE_WINDOWS
    scannedPlugins = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/")
                                                                                  .getChildFile ("Plume/");
  #elif JUCE_MAC
    scannedPlugins = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Application Support/")
                                                                                  .getChildFile ("Plume/");
  #endif
  
    scannedPlugins = scannedPlugins.getChildFile ("plumepl.cfg");
  
    if (!scannedPlugins.exists())
    {
        DBG ("Couldn't load plugin list, the file doesn't exist..");
        return;
    }
    
    // Recreates plugin List
	ScopedPointer<XmlElement> listXml = XmlDocument::parse(scannedPlugins);
	
	if (listXml->getChildByName ("USER_DIRECTORIES")) // checks the file for the right XML
    {
	    // Recreates custom directories list
	    customDirectories.copyPropertiesAndChildrenFrom (ValueTree::fromXml (*listXml->getChildByName ("USER_DIRECTORIES")
	                                                                                ->getChildByName (PLUME::treeId::pluginDirs)),
	                                                                        nullptr);
    }
    
    if (listXml->getChildByName ("KNOWNPLUGINS") != nullptr)
    {
        pluginList->recreateFromXml (*listXml->getChildByName("KNOWNPLUGINS"));
    }
    
	listXml->deleteAllChildElements();
}

//==============================================================================
void PluginWrapper::fillInPluginDescription (PluginDescription& pd)
{
    if (hasWrappedInstance)
    {
        wrappedInstance->fillInPluginDescription (pd);
    }
}


void PluginWrapper::addParametersToGestureFromXml (XmlElement& gesture, int gestureNum)
{
    if (hasWrappedInstance)
    {
        forEachXmlChildElement (gesture, paramXml)
        {
            gestArray.addAndSetParameter (wrapperProcessor->getWrappedParameter (paramXml->getIntAttribute("id")),
                                          gestureNum,
                                          float (paramXml->getDoubleAttribute("start", 0.0f)),
                                          float (paramXml->getDoubleAttribute("end", 1.0f)),
                                          paramXml->getBoolAttribute ("reversed", false));
        }
    }
}

//==============================================================================
void PluginWrapper::parameterValueChanged (int parameterIndex, float)
{
    if (gestArray.mapModeOn)
    {
	    gestArray.addParameterToMapModeGesture (wrapperProcessor->getWrappedParameter (parameterIndex));
    }
}
