/*
  ==============================================================================

    Tilt.h
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Gesture/Gesture.h"

class Tilt : public Gesture
{
public:
    Tilt (String gestName);
    ~Tilt();
    
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