/*
  ==============================================================================

    Roll.h
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Gesture/Gesture.h"

/**
 *  \class Roll Roll.h
 *
 *  \brief Roll gesture class.
 *
 *  Gesture class for to create the roll effect. Has it's own variables and overrides the base Gesture class methods
 *  in accordance to the effect.
 */
class Roll : public Gesture
{
public:
    Roll (String gestName);
    ~Roll();
    
    //==============================================================================
    void addGestureMidi(MidiBuffer& midiMessages) override;
    int getMidiValue () override;
    
    void updateMappedParameters() override;
    float getValueForMappedParameter(int paramId) override;
    
    //==============================================================================
    void updateValue (const Array<float> rawData) override;
    void addGestureParameters() override;
    
private:
};