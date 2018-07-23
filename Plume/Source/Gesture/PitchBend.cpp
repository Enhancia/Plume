/*
  ==============================================================================

    PitchBend.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "PitchBend.h"

PitchBend::PitchBend (String gestName, float leftLow, float leftHigh, float rightLow, float rightHigh)
    : Gesture (gestName, Gesture::pitchBend, Range<float> (-90.0f, 90.0f), 0.0f),
      lLow (leftLow), lHigh (leftHigh), hLow (rightLow), hHigh (rightHigh) 
{
}

PitchBend::~PitchBend()
{
}
    
//==============================================================================
void PitchBend::addGestureMidi (MidiBuffer& midiMessages)
{
    if (on == false) return; // does nothing if the gesture is inactive
    
    int pbVal = getMidiValue();
    
    if (send == true)
    {
        // Creates the pitchwheel message
        MidiMessage message = MidiMessage::pitchWheel (1, pbVal);
        midiMessages.addEvent(message, 1);
    }
}

int PitchBend::getMidiValue()
{
    if (value>= 0.0f && -value < 140.0f && hLow != hHigh)
    {
        send = true;
        return (Gesture::map (-value, hLow, hHigh, 8192, 16383));
    }
    
    else if (-value < 0.0f && -value > -140.0f && lLow != lHigh)
    {
        send = true;
        return (Gesture::map (-value, lLow, lHigh, 0, 8191));
    }
    
    send = false;
    return 8192;
}
   
void PitchBend::updateMappedParameters()
{
    //WIP
}

float PitchBend::getValueForMappedParameter (int paramId)
{
    return 0.0f; //WIP
}
    
//==============================================================================
void PitchBend::updateValue (const Array<float> rawData)
{
    value = rawData[5];
}

void PitchBend::addGestureParameters()
{
    //WIP
}