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
    void addGestureMidi(MidiBuffer& midiMessages) override;
    int getMidiValue () override;
    
    void updateMappedParameters() override;
    float getValueForMappedParameter(Range<float> paramRange) override;
    
    //==============================================================================
    void updateValue (const Array<float> rawData) override;
    
    //==============================================================================
    bool getSend(); /**< \brief Getter for the send boolean value */
    
    //==============================================================================
    // Attributes that will be referenced to the Tuner component.
    // Might want to replace them with audio processor parameters in the future.
    
    RangedAudioParameter& gain; /**< threshold used to trigger the effect*/
	RangedAudioParameter& threshold; /**< Sensibility of the vibrato. From 0.0f (no effect) to 500.0f (maximum effect)*/
    
private:
    float intensity = 0.0f; /**< Value that will be checked to trigger the vibrato if higher than the treshold */
    
    // Booleans that represent the state of the vibrato
    bool send = false; /**< \brief Boolean to know if the gesture should send midi */
    bool vibLast = false; /**< \brief Boolean to know if the gesture sent midi in the last buffer*/
};