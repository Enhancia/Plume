/*
  ==============================================================================

    PitchBend.h
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Gesture/Gesture.h"

/**
 *  \class PitchBend PitchBend.h
 *
 *  \brief PitchBend gesture class.
 *
 *  Gesture class for to create the pitch bend effect. Has it's own variables and overrides the base Gesture class methods
 *  in accordance to the effect.
 */
class PitchBend : public Gesture
{
public:
    PitchBend (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
               float leftLow = -50.0f, float leftHigh = -20.0f, float rightLow = 30.0f, float rightHigh = 60.0f);
    ~PitchBend();
    
    //==============================================================================
    void addGestureMidi(MidiBuffer& midiMessages, MidiBuffer& plumeBuffer) override;
    int getMidiValue () override;
    
    void updateMappedParameters() override;
    float getValueForMappedParameter (Range<float> paramRange, bool reversed) override;
    
    //==============================================================================
    void updateValue (const Array<float> rawData) override;
    
    //==============================================================================
    bool getSend(); /**< \brief Getter for the send boolean value */
    
    //==============================================================================
    RangedAudioParameter& rangeLeftLow; /**< \brief Bend's low range value. The down pitch effect will happen between this and rangeLeftHigh. */
	RangedAudioParameter& rangeLeftHigh; /**< \brief Bend's high range value. The down pitch effect will happen between rangeLeftLow and this. */
	RangedAudioParameter& rangeRightLow; /**< \brief Bend's low range value. The up pitch effect will happen between this and rangeRightHigh. */
	RangedAudioParameter& rangeRightHigh; /**< \brief Bend's high range value. The up pitch effect will happen between rangeRightLow and this. */
    
private:
    bool send = false; /**< \brief Boolean used to know if the gesture should send midi */
    bool pbLast = false; /**< \brief Boolean to know if the gesture sent midi in the last buffer*/
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchBend)
};