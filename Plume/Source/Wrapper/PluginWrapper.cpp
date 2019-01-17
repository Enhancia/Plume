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


#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))

PluginWrapper::PluginWrapper (PlumeProcessor& p, GestureArray& gArr)
    : owner (p), gestArray (gArr)
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
    clearWrapperEditor();
	wrapperProcessor = nullptr;
    wrappedInstance = nullptr;
    
    pluginList->clear();
    pluginList = nullptr;
    
    formatManager = nullptr;
    
    customDirectories.clear();
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
        DBG ("Can't wrap yourself my dude..");
        
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
void PluginWrapper::createWrapperEditor (int x, int y)
{
    TRACE_IN;
    
    jassert (wrapperProcessor != nullptr);
    
    if (wrapperProcessor == nullptr)
    {
        return;
    }
    
    if (hasOpenedEditor == true)
    {
        wrapperEditor->toFront (false);
        return;
    }
    
    hasOpenedEditor = true;
        
    if (wrapperEditor == nullptr)
    {
        wrapperEditor = new WrapperEditorWindow (*wrapperProcessor);
		wrapperEditor->toFront (false);
        return;
    }
    
    wrapperEditor.reset (new WrapperEditorWindow (*wrapperProcessor));
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

void PluginWrapper::wrapperEditorToFront (bool shouldAlsoGiveFocus)
{
    if (hasOpenedEditor)
    {
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

//==============================================================================
OwnedArray<File> PluginWrapper::createFileList()
{
    OwnedArray<File> directories;
    
    if (useDefaultPaths)
    {
        directories.add (new File (File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getFullPathName()));
        
      #if JUCE_WINDOWS
        // adds PF/Steinberg if it exists
      #elif JUCE_MAC
        // adds User and Local /Library/Audio/plug-Ins/
      #endif
    }
    
    if (!customDirectories.isEmpty())
    {
        directories.addArray (customDirectories);
    }

	return directories;
}

void PluginWrapper::addCustomDirectory (File newDir)
{
    if (newDir.exists() && newDir.isDirectory())
    {
        // Checks if it's not already in the array
        for (auto* file : customDirectories)
        {
            if (*file == newDir)
            {
                return;
            }
        }
        
        // Creates a copy of the file to prevent any memory leak
        customDirectories.add (new File (newDir));
    }
}

void PluginWrapper::scanAllPluginsInDirectories (bool dontRescanIfAlreadyInList, bool ignoreBlackList)
{
    if (formatManager->getNumFormats() == 0 ||
        (!useDefaultPaths && customDirectories.isEmpty())) return;
    
    if (!dontRescanIfAlreadyInList)
    {
        pluginList->clear();
    }
    
    // Sets all the files to search
    FileSearchPath fsp;
    for (auto* file : createFileList())
    {
        fsp.add (File (*file)); // Creates a copy of the file to prevent leakage / nullptr bad access
    }
        
    for (int i=0; i<formatManager->getNumFormats(); i++)
    {
        PluginDirectoryScanner dirScanner (*pluginList, *formatManager->getFormat (i), fsp, false, File(), false);
        
        while (dirScanner.scanNextFile (dontRescanIfAlreadyInList, pluginBeingScanned)) // Rescans until scanNextFile returns false
        {
            scanProgress = dirScanner.getProgress();
            DBG ("Scanning : " << pluginBeingScanned << " | Progress : " << scanProgress);
        }
    }
    
    savePluginListToFile();
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
    // TODO check ou mettre ça (application data??)
    //scannedPlugins = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Audio/")
  #endif
  
    scannedPlugins = scannedPlugins.getChildFile ("PluginList");
  
    if (!scannedPlugins.exists())
    {
        scannedPlugins.create();
    }
  
    // Writes plugin list data into the file
    ScopedPointer<XmlElement> listXml = pluginList->createXml();
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
    // TODO check ou mettre ça (application data??)
    //scannedPlugins = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Audio/")
  #endif
  
    scannedPlugins = scannedPlugins.getChildFile ("PluginList");
  
    if (!scannedPlugins.exists())
    {
        DBG ("Couldn't load pluginlist, the file doesn't exist..");
        return;
    }
    
    pluginList->recreateFromXml (*XmlDocument::parse (scannedPlugins));
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
void PluginWrapper::parameterValueChanged (int parameterIndex, float newValue)
{
    if (gestArray.mapModeOn)
    {
	    gestArray.addParameterToMapModeGesture (wrapperProcessor->getWrappedParameter (parameterIndex));
    }
}