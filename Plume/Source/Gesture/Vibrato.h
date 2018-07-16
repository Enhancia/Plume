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

class Vibrato : public Gesture
{
public:
    Vibrato (String gestName);
    ~Vibrato();
    
    //==============================================================================
    void addGestureMidi(MidiBuffer& midiMessages) override;
    int getMidiValue () override;
    
    void updateMappedParameter() override;
    float getValueForMappedParameter() override;
    
    //==============================================================================
    void updateValue (const Array<float> rawData) override;
    void addGestureParameters() override;
    
private:
    float threshold = 40.0f;
    float gain = 400.0f;
    
    float intensity = 0.0f;
    float value = 0.0f;
    
    bool send = false;
    bool vibLast = false;
};