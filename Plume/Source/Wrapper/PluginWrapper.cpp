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

PluginWrapper::PluginWrapper (PlumeProcessor& p)
    : owner (p)
{
    // Initializes the booleans
    hasWrappedInstance = false;
    hasOpenedEditor = false;
    
    // Creates the objects to wrap the plugin
    wrappedPluginDescriptions = new OwnedArray<PluginDescription>;
    formatManager = new AudioPluginFormatManager;
    formatManager->addFormat (new VSTPluginFormat());
}

PluginWrapper::~PluginWrapper()
{
    wrapperEditor = nullptr;
    wrappedInstance = nullptr;
    wrapperProcessor = nullptr;
    
    formatManager = nullptr;
    wrappedPluginDescriptions->clear();
    delete wrappedPluginDescriptions;
}

//==============================================================================
bool PluginWrapper::wrapPlugin (File pluginFile)
{   
    if (pluginFile.getFullPathName() == (File::getSpecialLocation (File::currentExecutableFile)).getFullPathName())
    {
        DBG ("Can't wrap yourself can you?");
        return false;
    }
    
    if (hasWrappedInstance)
    {
        unwrapPlugin();
    }
    
    //Scans the plugin directory for the wanted plugin and gets it's PluginDescription
    DBG ("\n[Scan and add File]");
    ScopedPointer<KnownPluginList> pluginList = new KnownPluginList();
    
    if (pluginList->scanAndAddFile (pluginFile.getFullPathName(),
                                    false,
                                    *wrappedPluginDescriptions,
                                    *(formatManager->getFormat (0))))
    {
        pluginList = nullptr;
    }
    else
    {
        DBG ("Error: The specified plugin doesn't exist or failed to load.\n\n");
        return false;
    }
    
    //Creates the plugin instance using the format manager
    String errorMsg;
    DBG ("\n[create Plugin Instance]");
    wrappedInstance = formatManager->createPluginInstance (*((*wrappedPluginDescriptions)[wrappedPluginDescriptions->size()-1]),
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
    if (hasWrappedInstance == false)
    {
        return;
    }
    
    if (hasOpenedEditor)
    {
        clearWrapperEditor();
    }
    
    hasWrappedInstance = false;
    
    wrapperProcessor.reset();
    wrappedInstance.reset();
}

bool PluginWrapper::rewrapPlugin(File pluginFile)
{
    unwrapPlugin();
    return wrapPlugin(pluginFile);
}

//==============================================================================
bool PluginWrapper::isWrapping()
{
    return hasWrappedInstance;
}

//==============================================================================
void PluginWrapper::createWrapperEditor()
{
    jassert(wrapperProcessor != nullptr);
    
    if (wrapperProcessor == nullptr)
    {
        return;
    }
    
    if (hasOpenedEditor == true)
    {
        wrapperEditor->toFront(false);
        return;
    }
    
    wrapperProcessor->hasOpenedEditor = true;
        
    if (wrapperEditor == nullptr)
    {
        wrapperEditor = new WrapperEditorWindow (*wrapperProcessor);
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