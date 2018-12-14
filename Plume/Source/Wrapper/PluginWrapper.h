/*
  ==============================================================================

    PluginWrapper.h
    Created: 29 Jun 2018 3:03:40pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

#include "Gesture/GestureArray.h"
#include "Wrapper/WrapperEditor.h"
/**
 *  \class PluginWrapper PluginWrapper.h
 *
 *  \brief Class that manages the plugin wrapping function of plume.
 *
 *  The class holds pointers to a wrapperProcessor, an AudioPluginInstance and a WrapperEditorWindow.
 *  It can be used to wrap a plugin, unwrap it, create or destroy its editor.
 */
class PlumeProcessor;

class PluginWrapper : public AudioProcessorParameter::Listener,
                      public ChangeBroadcaster
{
public:
    enum Formats
    {
        VST =0,
      #if JUCE_MAC
        AU,
      #endif
      #if JUCE_PLUGINHOST_VST3
        VST3,
      #endif
        numFormats
    };
   
    //==============================================================================
    PluginWrapper(PlumeProcessor&, GestureArray&);
    ~PluginWrapper();
    
    //==============================================================================
    bool wrapPlugin (String pluginFileOrId);
    void unwrapPlugin();
    bool rewrapPlugin (String pluginFileOrId);
    
    AudioPluginFormat* getPluginFormat (File pluginFile);
    
    //==============================================================================
    bool isWrapping();
    
    //==============================================================================
    void createWrapperEditor (int x = 0, int y = 0);
    void clearWrapperEditor();
    void wrapperEditorToFront (bool shouldAlsoGiveFocus);
    
    //==============================================================================
    WrapperProcessor& getWrapperProcessor();
    PlumeProcessor& getOwner();
    bool hasOpenedWrapperEditor();
    
    String getWrappedPluginName();
    String getWrappedPluginInfoString();
    
    //==============================================================================
    void fillInPluginDescription (PluginDescription& pd);
    void addParametersToGestureFromXml (XmlElement& gesture, int gestureNum);
    
    //==============================================================================
    void parameterValueChanged (int parameterIndex, float newValue) override;
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override {};
    
private:       
    //==============================================================================
    bool hasWrappedInstance;
    bool hasOpenedEditor;
    
    //==============================================================================
    ScopedPointer<WrapperProcessor> wrapperProcessor;
    ScopedPointer<AudioPluginInstance> wrappedInstance;
    ScopedPointer<WrapperEditorWindow> wrapperEditor;

    //==============================================================================
    OwnedArray<PluginDescription>* wrappedPluginDescriptions;
    ScopedPointer<AudioPluginFormatManager> formatManager;
 
    PlumeProcessor& owner;
    GestureArray& gestArray;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWrapper)
};