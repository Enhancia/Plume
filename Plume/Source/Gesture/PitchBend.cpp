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
      rangeLeft (Range<float> (leftLow, leftHigh)), rangeRight (Range<float> (rightLow, rightHigh)) 
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
    if (value>= 0.0f && value < 140.0f && !(rangeRight.isEmpty()))
    {
        send = true;
        return (Gesture::map (value, rangeRight.getStart(), rangeRight.getEnd(), 8192, 16383));
    }
    
    else if (value < 0.0f && value > -140.0f && !(rangeLeft.isEmpty()))
    {
        send = true;
        return (Gesture::map (value, rangeLeft.getStart(), rangeLeft.getEnd(), 0, 8191));
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
    value = -rawData[5];
}

void PitchBend::addGestureParameters()
{
    //WIP
}