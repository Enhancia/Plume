/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Ui/Wrapper/WrapperComponent.h"
#include "Ui/Presets/PresetComponent.h"

//==============================================================================
/**
*/

/**
 *  \class PlumeEditor PluginEditor.h
 *
 *  \brief Plume's editor component. Holds the ui of the plugin.
 *
 *  Holds plumes GUI. This component will create the child components that will handle the 3
 *  interface blocks: wrapper, presets, and gestures.
 */
class PlumeEditor  : public AudioProcessorEditor,
                     public ChangeListener
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
    void changeListenerCallback(ChangeBroadcaster* source) override;
    
    //==============================================================================
    /**
     * \brief Method that sets a full interface update.
     *
     * This method is called after a change message is sent by the processor.
     * It sets the right current wrapped plugin, preset and gestures.
     */
    void updateFullInterface();
    
private:
    PlumeProcessor& processor; /** < Reference to Plume's processor object */
    ScopedPointer<WrapperComponent> wrapperComp; /** < Object allowing to choose a plugin to wrap and use with neova */
    ScopedPointer<PresetComponent> presetComp; /** < Object allowing to save or load presets, in the xml format */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeEditor)
};
