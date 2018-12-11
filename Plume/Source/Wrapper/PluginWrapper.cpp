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
    wrappedPluginDescriptions = new OwnedArray<PluginDescription>;
    formatManager = new AudioPluginFormatManager;
    formatManager->addFormat (new VSTPluginFormat());
  #if JUCE_MAC
    formatManager->addFormat (new AudioUnitPluginFormat());
  #endif
  #if JUCE_PLUGINHOST_VST3
    formatManager->addFormat (new VST3PluginFormat());
  #endif
}

PluginWrapper::~PluginWrapper()
{
    TRACE_IN;
    wrapperEditor = nullptr;
	wrapperProcessor = nullptr;
    wrappedInstance = nullptr;
    
    formatManager = nullptr;
    wrappedPluginDescriptions->clear();
    delete wrappedPluginDescriptions;
}

//==============================================================================
bool PluginWrapper::wrapPlugin (String pluginFileOrId)
{   
    TRACE_IN;
    
    bool isFile = File (pluginFileOrId).exists();
    
    if ((isFile && File (pluginFileOrId) == File::getSpecialLocation (File::currentExecutableFile)) ||
        pluginFileOrId.contains ("Plume."))
    {
        DBG ("Can't wrap yourself can you?");
        return false;
    }
    
    //Scans the plugin directory for the wanted plugin and gets its PluginDescription
    DBG ("\n[Scan and add File]");
    ScopedPointer<KnownPluginList> pluginList = new KnownPluginList();
    
    if (isFile)
    {
        File pluginFile (pluginFileOrId);
        
        // Scanning method for a File
        PluginDirectoryScanner pScanner (*pluginList, *getPluginFormat (pluginFile),
                                         FileSearchPath (pluginFile.getParentDirectory().getFullPathName()),
                                         false, File(), true);
        String name;
    
        pScanner.setFilesOrIdentifiersToScan (StringArray (pluginFileOrId));
        pScanner.scanNextFile (false, name);
    
        if (auto desc = pluginList->getType (0))
        {
            // Only loads the plugin if it is an instrument (ie it creates sound).
            if (desc->isInstrument)  wrappedPluginDescriptions->add (desc);
        
            else
            {
                DBG ("Error: The specified plugin ( " << name << " ) isn't an instrument.\n\n");
                return false;
            }
        }
        else
        {
            DBG ("Error: The specified plugin ( " << name << " | " << pluginFileOrId << " ) doesn't exist or failed to load.\n\n");
            return false;
        }
    }
    
    // If not a file, then most likely an AU identifier:
    // the search takes place in the common AU folders with the AU format.
    else
    {
      #if !JUCE_MAC
        DBG ("Error: The specified plugin ( " << pluginFileOrId << " ) isn't a in a legal file path.\n\n");
        return false;
      #else
        PluginDirectoryScanner pScanner (*pluginList, formatManager->getFormat (Formats::AU),
		                                FileSearchPath ("Macintosh HD:/Library/Audio/Plug-Ins/Components/;
                                                        ~/Library/Audio/Plug-Ins/Components/"),,
                                        false, File(), true);
        String name;
    
        pScanner.setFilesOrIdentifiersToScan (StringArray (pluginFileOrId));
        pScanner.scanNextFile (false, name);
    
        if (auto desc = pluginList->getType (0))
        {
            // Only loads the plugin if it is an instrument (ie it creates sound).
            if (desc->isInstrument)  wrappedPluginDescriptions->add (desc);
        
            else
            {
                DBG ("Error: The specified plugin ( " << name << " ) isn't an instrument.\n\n");
                return false;
            }
        }
        else
        {
            DBG ("Error: The specified plugin ( " << name << " | " << pluginFileOrId << " ) doesn't exist or failed to load.\n\n");
            return false;
        }
      #endif
    }
    

	if (hasWrappedInstance)
	{
		unwrapPlugin();
	}

    //Creates the plugin instance using the format manager
    String errorMsg;
    DBG ("\n[create Plugin Instance]");
    wrappedInstance = formatManager->createPluginInstance (*(wrappedPluginDescriptions->getLast()),
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

	wrappedPluginDescriptions->clear (false);
	pluginList = nullptr;
	
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

bool PluginWrapper::rewrapPlugin(String pluginFileOrId)
{
    unwrapPlugin();
    return wrapPlugin(pluginFileOrId);
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
//==============================================================================
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