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
    PitchBend (String gestName, float leftLow = -50.0f, float leftHigh = -20.0f, float rightLow = 30.0f, float rightHigh = 60.0f);
    ~PitchBend();
    
    //==============================================================================
    void addGestureMidi(MidiBuffer& midiMessages) override;
    int getMidiValue () override;
    
    void updateMappedParameters() override;
    float getValueForMappedParameter(Range<float> paramRange) override;
    
    //==============================================================================
    void updateValue (const Array<float> rawData) override;
    void addGestureParameters() override;
    
    //==============================================================================
    bool getSend(); /**< \brief Getter for the send boolean value */
    
    //==============================================================================
    // Attributes that will be referenced to the Tuner component.
    // Might want to replace them with audio processor parameters in the future.
    
    Range<float> rangeLeft;  /**< \brief Left range of the gesture */
    Range<float> rangeRight; /**< \brief Right range of the gesture */
    
private:
    bool send = false; /**< \brief Boolean used to know if the gesture should send midi */
    bool pbLast = false; /**< \brief Boolean to know if the gesture sent midi in the last buffer*/
};