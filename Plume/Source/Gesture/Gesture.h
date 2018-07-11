/*
  ==============================================================================

    Gesture.h
    Created: 2 Jul 2018 3:15:48pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class Gesture
{
public:
    //==============================================================================
    enum GestureType
    {
        vibrato =0,
        pitchBend,
        tilt,
        wave,
        roll,
        numGestures
    };
    
    //==============================================================================
    Gesture(String gestName, int gestType)	: name (gestName), type (gestType)
    {
        on = false;
        mapped = false;
    }
    
    ~Gesture() {}

    //==============================================================================
    virtual void addGestureMidi(MidiBuffer& MidiMessages) =0;
    virtual int getMidiValue () =0;
    
    virtual void updateMappedParameter() =0;
    virtual float getValueForMappedParameter() =0;
    
    //==============================================================================
    virtual void updateValue (float* rawData) =0;
    virtual void addGestureParameters() =0;
    
    //==============================================================================
    void setActive (bool shouldBeOn)
    {
        on = shouldBeOn;
    }
    
    void setMapped (bool shouldBeMapped)
    {
        mapped = shouldBeMapped;
    }
    
    bool isMapped ()
    {
        return mapped;
    }
    
    //==============================================================================
    const String name;
    const int type;
    
protected:
    //==============================================================================
    static int normalizeMidi (float minVal, float maxVal, float value)
    {
        int norm;
        float a, b;
  
        a = 127.0f / (maxVal - minVal);
        b = -a * minVal;
  
        norm = (a*value+b);
        if (norm < 0) norm = 0;
        if (norm > 127) norm = 127;
  
        return (int) norm;
    }
    
    static int map (float value, float minVal, float maxVal, int minNew, int maxNew)
    {
        if (minVal == maxVal) return 0;
    
        if (value < minVal) value = minVal;
        if (value > maxVal) value = maxVal;
    
        return (minNew + int ((maxNew - minNew)*(value - minVal)/(maxVal-minVal)));
    }
    
    //==============================================================================
    bool on;
    bool mapped;
    
private:
};