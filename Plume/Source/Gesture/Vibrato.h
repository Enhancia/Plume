/*
  ==============================================================================

    Vibrato.h
    Created: 9 Jul 2018 3:04:42pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Gesture/Gesture.h"

/**
 *  \class Vibrato Vibrato.h
 *
 *  \brief Vibrato gesture class.
 *
 *  Gesture class for to create the vibrato effect. Has it's own variables and overrides the base Gesture class methods
 *  in accordance to the effect.
 */
class Vibrato : public Gesture
{
public:
    Vibrato (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters, float val = 400.0f, float thresh = 40.0f);
    ~Vibrato();
    
    //==============================================================================
    void addGestureMidi(MidiBuffer& midiMessages, MidiBuffer& plumeBuffer) override;
    int getMidiValue () override;
    
    void updateMappedParameters() override;
    float getValueForMappedParameter (Range<float> paramRange, bool reversed) override;
    
    //==============================================================================
    void setIntensityValue (float newVal);
    void updateValue (const Array<float> rawData) override;
    
    //==============================================================================
    bool getSend(); /**< \brief Getter for the send boolean value */
    
    //==============================================================================
    RangedAudioParameter& gain; /**< Sensibility of the vibrato. From 0.0f (no effect) to 500.0f (maximum effect)*/
	RangedAudioParameter& threshold; /**< threshold used to trigger the effect*/
	RangedAudioParameter& intensity; /**< Current intensity of the vibrato. The effect is triggered if this is above the threshold parameter*/
    
private:
    //float intensity = 0.0f; /**< Value that will be checked to trigger the vibrato if higher than the treshold */
    
    // Booleans that represent the state of the vibrato
    bool send = false; /**< \brief Boolean to know if the gesture should send midi */
    bool vibLast = false; /**< \brief Boolean to know if the gesture sent midi in the last buffer*/

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Vibrato)
};