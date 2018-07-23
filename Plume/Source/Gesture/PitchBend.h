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
    float getValueForMappedParameter(int paramId) override;
    
    //==============================================================================
    void updateValue (const Array<float> rawData) override;
    void addGestureParameters() override;
    
    //==============================================================================
    // Attributes that will be referenced to the Tuner component.
    // Might want to replace them with audio processor parameters in the future.
    
    float lLow; /**< \brief Low value of the left range */
    float lHigh; /**< \brief High value of the left range */
    float hLow; /**< \brief Low value of the right range */
    float hHigh; /**< \brief High value of the right range */
    
private:
    bool send = false; /**< \brief Boolean used to know if the gesture should send midi */
};