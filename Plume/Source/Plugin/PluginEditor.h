/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"

#include "Ui/Wrapper/WrapperComponent.h"
#include "Ui/Presets/PresetComponent.h"
#include "Ui/Gesture/GesturePanel.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

//#define TRACE_IN  Logger::writeToLog ("[FNC] Entering function: " + __FUNCTION__);
//#define TRACE_OUT Logger::writeToLog ("[FNC] Entering function: " + __FUNCTION__);
//==============================================================================
/**
*/

class PresetComponent;

/**
 *  \class PlumeEditor PluginEditor.h
 *
 *  \brief Plume's editor component. Holds the ui of the plugin.
 *
 *  Holds plumes GUI. This component will create the child components that will handle the 3
 *  interface blocks: wrapper, presets, and gestures.
 */
class PlumeEditor  : public AudioProcessorEditor,
                     public ActionListener
{
public:
    /**
     * \brief Constructor.
     *
     * Draws the background, calls the subcomponents and sets their bounds accordingly.
     *
     * \param p Reference to Plume's processor object.
     */
    PlumeEditor (PlumeProcessor& p);
    
    /**
     * \brief Destructor.
     */
    ~PlumeEditor();

    //==============================================================================
    /**
     * \brief JUCE Components' paint method.
     */
    void paint (Graphics& g) override;
    /**
     * \brief JUCE Components' resized method.
     */
    void resized() override;
    
    //==============================================================================
    /**
     * \brief Callback to a change message sent by the processor.
     *
     * This method is called by the processor when it needs the interface to be fully updated.
     * It calls the method updateFullInterface. sets the right current wrapped plugin, preset and gestures.
     */
    void actionListenerCallback(const String &message) override;
    
    //==============================================================================
    PlumeProcessor& getProcessor();
    
    //==============================================================================
    /**
     * \brief Method that sets a full interface update.
     *
     * This method is called after a change message is sent by the processor.
     * It sets the right current wrapped plugin, preset and gestures.
     */
    void updateFullInterface();
    
    void setInterfaceUpdates (bool shouldUpdate);
    
private:
    //==============================================================================
    PlumeProcessor& processor; /**< \brief Reference to Plume's processor object */
    ScopedPointer<WrapperComponent> wrapperComp; /**< \brief Object allowing to choose a plugin to wrap and use with neova */
    ScopedPointer<PresetComponent> presetComp; /**< \brief Object allowing to save or load presets, in the xml format */
    ScopedPointer<GesturePanel> gesturePanel; /**< \brief Object that handles the different gesture gui objects */
    
    //==============================================================================
    PlumeLookAndFeel plumeLookAndFeel;
    ScopedPointer<ResizableCornerComponent> resizableCorner;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeEditor)
};
