/*
  ==============================================================================

    PluginWrapper.cpp
    Created: 29 Jun 2018 3:03:40pm
    Author:  Alex

  ==============================================================================
*/

#include "../Plugin/PluginProcessor.h"

#if ! (JUCE_PLUGINHOST_VST || JUCE_PLUGINHOST_VST3 || JUCE_PLUGINHOST_AU)
 #error "If you're building the wrapper, you probably want to enable VST and/or AU support"
#endif

PluginWrapper::PluginWrapper (PlumeProcessor& p, GestureArray& gArr, ValueTree pluginDirs)
    : owner (p), gestArray (gArr), customDirectories (pluginDirs)
{
    // Initializes the booleans
    hasWrappedInstance = false;
    hasOpenedEditor = false;
    
    // Creates the objects to wrap the plugin
    formatManager.reset (new AudioPluginFormatManager());
    formatManager->addFormat (new VSTPluginFormat());
  #if JUCE_MAC
    formatManager->addFormat (new AudioUnitPluginFormat());
  #endif
  #if JUCE_PLUGINHOST_VST3
    formatManager->addFormat (new VST3PluginFormat());
  #endif
    
    pluginList.reset (new KnownPluginList());
    scanHandler.reset (new ScanHandler (*pluginList));
    loadPluginListFromFile();
}

PluginWrapper::~PluginWrapper()
{
    removeAllChangeListeners();
    
  #if JUCE_MAC
    unwrapPluginDelayed (1, false);
  #else
    unwrapPlugin (false);
  #endif

    pluginList->clear();
    pluginList = nullptr;
    formatManager = nullptr;
}

//==============================================================================
bool PluginWrapper::wrapPlugin (PluginDescription& description)
{
    ScopedLock plLock (pluginListLock);

    auto descToWrap = getDescriptionToWrap (description);
    
    if (descToWrap == nullptr)
    {
        PLUME::log::writeToLog ("Failed to find the plugin to wrap..  "
                                "Specified description for wrapping : " + description.name
                                + " (Id : " + String (description.uniqueId) + " | Format : "
                                            + description.pluginFormatName + ")",
                                PLUME::log::pluginWrapping, PLUME::log::error);

        getOwner().sendActionMessage (PLUME::commands::missingPlugin);
        return false;
    }
    
    if (descToWrap->name == "Plume" || descToWrap->name == "Plume Tests")
    {
        PLUME::log::writeToLog ("Plume tried to wrap itself..", PLUME::log::pluginWrapping, PLUME::log::error);
        return false;
    }
    
    if (!(descToWrap)->isInstrument)
    {
        PLUME::log::writeToLog ("Attempted to wrap a non-instrument plugin : " + descToWrap->name, PLUME::log::pluginWrapping, PLUME::log::error);
        return false;
    }
    
    // TO DELETE when implementing AU
  #if JUCE_MAC
    if (descToWrap->pluginFormatName.compare (AudioUnitPluginFormat::getFormatName()) == 0)
    {
        return false;
    }
  #endif

    jassert (!hasWrappedInstance);
    if (hasWrappedInstance)
    {
        unwrapPlugin();
    }

    PLUME::log::writeToLog ("Attempting to load plugin : " + descToWrap->name, PLUME::log::pluginWrapping);
	
    String errorMsg;
    wrappedInstance = formatManager->createPluginInstance (*descToWrap,
                                                           owner.getSampleRate(),
														   owner.getBlockSize(),
                                                           errorMsg);
                                                             
    if (wrappedInstance == nullptr)
    {
        PLUME::log::writeToLog ("Failed to load plugin.. Error message : " + errorMsg,
                                PLUME::log::pluginWrapping, PLUME::log::error);
        return false;
    }
    
    //Creates the wrapped processor object using the instance
    
    wrapperProcessor.reset (new WrapperProcessor (*wrappedInstance, *this));
    wrapperProcessor->prepareToPlay (owner.getSampleRate(), owner.getBlockSize());
    hasWrappedInstance = true;
	
    //wrapperProcessor->setBusesLayout(owner.getBusesLayout());
    //wrappedInstance->setBusesLayout(owner.getBusesLayout());
    
    return true;
}

bool PluginWrapper::wrapPlugin (int pluginMenuId)
{
    PluginDescription descToWrap = pluginList->getTypes() [KnownPluginList::getIndexChosenByMenu (pluginList->getTypes(),
                                                                                                  pluginMenuId)];
    
    return wrapPlugin (descToWrap);
}

void PluginWrapper::unwrapPlugin (bool clearGestureParameters)
{
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
    if (clearGestureParameters) owner.getGestureArray().clearAllParameters();
    wrapperProcessor.reset();
    wrappedInstance.reset();
}

void PluginWrapper::unwrapPluginDelayed (const unsigned int delay, bool clearGestureParameters)
{
    if (hasWrappedInstance == false)
    {
        return;
    }
    
    if (hasOpenedEditor)
    {
        clearWrapperEditor();
    }
    
    hasWrappedInstance = false;

    if (clearGestureParameters) owner.getGestureArray().clearAllParameters();
    wrapEd.reset();
    wrapperProcessor.reset();

    wrappedInstanceToDelete = std::move (wrappedInstance);

    auto instanceDeletionLambda = [this] ()
    {
        wrappedInstanceToDelete.reset();
    };

    Timer::callAfterDelay (delay, instanceDeletionLambda);
}

/*
bool PluginWrapper::rewrapPluginDelayed (int pluginId)
{
    if (hasWrappedInstance == false)
    {
        return wrapPlugin (pluginId);
    }
    
    if (hasOpenedEditor)
    {
        clearWrapperEditor();
    }
    
    hasWrappedInstance = false;

    auto processorDeletionLambda = [this] ()
    {
        DBG ("Deleting Wrapper Processor and Instance, delayed.");
        wrapEd.reset();
        owner.getGestureArray().clearAllParameters();
        wrapperProcessor.reset();
        wrappedInstance.reset();
    };

    Timer::callAfterDelay (100, processorDeletionLambda);

    return wrapPlugin(description);
}*/

bool PluginWrapper::rewrapPlugin (PluginDescription& description)
{
    unwrapPluginDelayed();
    return wrapPlugin(description);
}

bool PluginWrapper::rewrapPlugin(int pluginId)
{
    unwrapPluginDelayed();
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
        wrapperEditor.reset (new WrapperEditorWindow (*wrapperProcessor, componentWhichWindowToAttachTo));
		wrapperEditor->toFront (true);
        return;
    }
    
    wrapperEditor.reset (new WrapperEditorWindow (*wrapperProcessor, componentWhichWindowToAttachTo));
}

WrapperEditorWindow* PluginWrapper::getWrapperEditorWindow()
{
    if (hasWrappedInstance)
    {
        return wrapperEditor.get();
    }

	return nullptr;
}

void PluginWrapper::clearWrapperEditor()
{
    if (hasWrappedInstance && hasOpenedEditor)
    {
        wrapperEditor->setVisible (false);

        wrapperEditor.reset (nullptr);
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

ScanHandler& PluginWrapper::getScanner()
{
    return *scanHandler;
}

void PluginWrapper::handleScanFinished()
{
    //loadPluginListFromFile();
    removeNonInstrumentsFromList();
    savePluginListToFile();
}

//==============================================================================
Array<File> PluginWrapper::createFileList()
{
    Array<File> directories;

    auto addFileToDirectories = [] (Array<File>& dirs, File fileToAdd)
    {
        if (fileToAdd.exists()) dirs.add (fileToAdd);
    };
    
    // Adds OS specific paths if they exist
  #if JUCE_WINDOWS

    if (useDefaultPaths)
    {
        // C:\Program Files\VSTPlugins 
        addFileToDirectories (directories, File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("VSTPlugins/"));
        
        // C:\Program Files\Steinberg\VSTPlugins
        addFileToDirectories (directories, File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("Steinberg/")
                                                                        .getChildFile ("VSTPlugins/"));
        
        // C:\Program Files\Common Files\VST2\                                                              
        addFileToDirectories (directories, File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("Common Files/")
                                                                        .getChildFile ("VST2/"));
        
        // C:\Program Files\Common Files\Steinberg\VST2                                                              
        addFileToDirectories (directories, File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("Common Files/")
                                                                        .getChildFile ("Steinberg/")
                                                                        .getChildFile ("VST2/"));
    }

  #elif JUCE_MAC

    if (useAudioUnits)
    {
        // Macintosh HD:/Library/Audio/Plug-Ins/Components/
        addFileToDirectories (directories, File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Audio/")
                                                                           .getChildFile ("Plug-Ins/")
                                                                           .getChildFile ("Components/"));
        
        // ~/Library/Audio/Plug-Ins/Components/                                                               
        addFileToDirectories (directories, File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Audio/")
                                                                         .getChildFile ("Plug-Ins/")
                                                                         .getChildFile ("Components/"));
    }

    if (useDefaultPaths)
    {
        // Macintosh HD:/Library/Audio/Plug-Ins/VST/                                                               
        addFileToDirectories (directories, File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Audio/")
                                                                           .getChildFile ("Plug-Ins/")
                                                                           .getChildFile ("VST/"));
        
        // ~/Library/Audio/Plug-Ins/VST/                                                            
        addFileToDirectories (directories, File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Audio/")
                                                                         .getChildFile ("Plug-Ins/")
                                                                         .getChildFile ("VST/"));
    } 

  #endif
    
    if (useCustomPaths && customDirectories.getNumChildren() != 0)
    {
		for (int i = 0; i < customDirectories.getNumChildren(); i++)
		{
			directories.add (getCustomDirectory (i));
		}
    }

	return directories;
}

void PluginWrapper::setDefaultPathUsage (bool shouldUseDefaultPaths)
{
    useDefaultPaths = shouldUseDefaultPaths;
    savePluginListToFile();
}

void PluginWrapper::setCustomPathUsage (bool shouldUseCustomPath)
{
    useCustomPaths = shouldUseCustomPath;
    savePluginListToFile();
}

void PluginWrapper::setAuUsage (bool                    
                                #if JUCE_MAC
                                    shouldUseAudioUnits
                                #endif
                                )
{
  #if JUCE_MAC
    // TO UNCOMMENT when implementing AU
    useAudioUnits = shouldUseAudioUnits;
    scanHandler->setPluginFormats (true, true, shouldUseAudioUnits);
    savePluginListToFile();
  #endif
}
bool PluginWrapper::usesDefaultPaths()
{
    return useDefaultPaths;
}
bool PluginWrapper::usesCustomPaths()
{
    return useCustomPaths;
}
bool PluginWrapper::usesAudioUnits()
{
    #if JUCE_MAC
    return useAudioUnits;
    #else
    return false;
    #endif
}

void PluginWrapper::addCustomDirectory (File newDir)
{
    // TODO virer cette ligne et decommenter code quand implémentation plusieurs path
    customDirectories.getOrCreateChildWithName (PLUME::treeId::directory, nullptr)
                     .setProperty (PLUME::treeId::value,
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

void PluginWrapper::startScanProcess (bool dontRescanIfAlreadyInList, bool resetBlackList)
{
    PLUME::log::writeToLog ("Starting plugin scan.", PLUME::log::pluginScan);

    getOrCreateDeadsManPedalFile();
    
    scanHandler->startScanProcess (!dontRescanIfAlreadyInList,
                                   createFileList());
}

void PluginWrapper::addPluginsToMenu (PopupMenu& menu, KnownPluginList::SortMethod sort)
{
    ScopedLock plLock (pluginListLock);
    
    if (pluginList->getNumTypes() == 0)
    {
        getOwner().sendActionMessage (PLUME::commands::scanRequired);
    }
    
    else pluginList->addToMenu (menu, sort);
}

std::unique_ptr<PluginDescription> PluginWrapper::getDescriptionToWrap (const PluginDescription& description)
{   
    ScopedLock plLock (pluginListLock);

    for (auto& desc : pluginList->getTypes())
    {
		if ((desc.uniqueId == description.uniqueId || desc.deprecatedUid == description.deprecatedUid)&&
			desc.pluginFormatName == description.pluginFormatName &&
			(desc.name == description.name || desc.descriptiveName == description.descriptiveName))
        {
            return std::make_unique<PluginDescription> (desc);
        }
    }
    
    return nullptr;
}

void PluginWrapper::savePluginListToFile()
{
    ScopedLock plLock (pluginListLock);

    // Create file if it doesn't exist yet
    if (!PLUME::file::pluginList.exists())
    {
        getOrCreatePluginListFile();
    }
    
    auto listXml = std::make_unique<XmlElement> ("PLUME_PLUGINLIST_CONFIG");
    
    // Writes plugin list data into the file
    listXml->addChildElement (new XmlElement (*pluginList->createXml()));
    
    XmlElement* userDirs = listXml->createNewChildElement ("USER_DIRECTORIES");
    userDirs->addChildElement (new XmlElement (*customDirectories.createXml()));

    XmlElement* searchSettings = listXml->createNewChildElement ("SETTINGS");
    searchSettings->setAttribute ("use_custom", useCustomPaths  ? 1 : 0);
    searchSettings->setAttribute ("use_system", useDefaultPaths ? 1 : 0);
  #if JUCE_MAC
    searchSettings->setAttribute ("use_au",     useAudioUnits   ? 1 : 0);
  #endif

    listXml->writeToFile (PLUME::file::pluginList, StringRef());
    listXml->deleteAllChildElements();
}

void PluginWrapper::removeNonInstrumentsFromList()
{
    ScopedLock plLock (pluginListLock);

    for (auto& type : pluginList->getTypes())
    {
        if (!type.isInstrument) pluginList->removeType (type);
    }
}

void PluginWrapper::resetDeadsManPedalFile()
{
    // Attempts to find file
    if (PLUME::file::deadMansPedal.exists() && PLUME::file::deadMansPedal.loadFileAsString().isNotEmpty())
    {
        PLUME::file::deadMansPedal.replaceWithText (String());
    }
}

void PluginWrapper::loadPluginListFromFile()
{
    ScopedLock plLock (pluginListLock);
    
    if (!PLUME::file::pluginList.exists())
    {
        DBG ("Couldn't load plugin list, the file doesn't exist..");
        return;
    }
    
    // Recreates plugin List
	std::unique_ptr<XmlElement> listXml = XmlDocument::parse (PLUME::file::pluginList);
	
	if (listXml == nullptr)
	{
		DBG("Couldn't parse plugin list file..");
		return;
	}

	if (listXml->getChildByName ("USER_DIRECTORIES")) // checks the file for the right XML
    {
	    // Recreates custom directories list
	    customDirectories.copyPropertiesAndChildrenFrom (ValueTree::fromXml (*listXml->getChildByName ("USER_DIRECTORIES")
	                                                                                ->getChildByName (PLUME::treeId::pluginDirs)),
	                                                                        nullptr);
    }

    if (auto* settings = listXml->getChildByName ("SETTINGS"))
    {
        useDefaultPaths = settings->getBoolAttribute ("use_system", true);
        useCustomPaths = settings->getBoolAttribute ("use_custom", true);
        #if JUCE_MAC
        useAudioUnits = settings->getBoolAttribute ("use_au", useAudioUnits);
        #endif
    }
    
    if (listXml->getChildByName ("KNOWNPLUGINS") != nullptr)
    {
        pluginList->recreateFromXml (*listXml->getChildByName("KNOWNPLUGINS"));
    }
    
	listXml->deleteAllChildElements();
}

File PluginWrapper::getOrCreatePluginListFile()
{
    ScopedLock plLock (pluginListLock);

    if (PLUME::file::pluginList.exists() || PLUME::file::pluginList.create().wasOk())
    {
        return PLUME::file::pluginList;
    }

    return File();
}

File PluginWrapper::getOrCreateDeadsManPedalFile()
{
    // Attempts to find file
    if (PLUME::file::deadMansPedal.exists() || PLUME::file::deadMansPedal.create().wasOk())
    {
        return PLUME::file::deadMansPedal;
    }

    return File();
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
