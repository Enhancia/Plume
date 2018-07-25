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

class PluginWrapper : public AudioProcessorParameter::Listener
{
public:
    //==============================================================================
    PluginWrapper(PlumeProcessor&, GestureArray&);
    ~PluginWrapper();
    
    //==============================================================================
    bool wrapPlugin (File pluginFile);
    void unwrapPlugin();
    bool rewrapPlugin(File pluginFile);
    
    //==============================================================================
    bool isWrapping();
    
    //==============================================================================
    void createWrapperEditor();
    void clearWrapperEditor();
    
    //==============================================================================
    String getWrappedPluginName();
    WrapperProcessor& getWrapperProcessor();
    PlumeProcessor& getOwner();
    
    //==============================================================================
    void fillInPluginDescription (PluginDescription& pd);
    
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