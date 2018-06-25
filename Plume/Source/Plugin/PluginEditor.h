/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Plugin/PluginProcessor.h"

//==============================================================================
/**
*/

/**
 *  \class PlumeEditor PluginEditor.h
 *
 *  \brief Plume's editor component.
 *
 *  Holds plumes GUI. This component will create the child components that will handle the 3
 *  interface blocks: wrapper, presets, and gestures.
 */
class PlumeEditor  : public AudioProcessorEditor
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

private:
    PlumeProcessor& processor; /** < Reference to Plume's processor object */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeEditor)
};
