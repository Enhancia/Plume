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
    void addGestureMidi(MidiBuffer& MidiMessages) override;
    int getMidiValue () override;
    
    void updateMappedParameter() override;
    float getValueForMappedParameter() override;
    
    //==============================================================================
    void updateValue (const float* rawData) override;
    void addGestureParameters() override;
    
private:
};