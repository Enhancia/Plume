/*
  ==============================================================================

    Tilt.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "Tilt.h"

Tilt::Tilt (String gestName, float lowValue, float highValue)
    : Gesture (gestName, Gesture::tilt, Range<float> (-90.0f, 90.0f), 0.0f),
      range (Range<float> (lowValue, highValue))
{
}

Tilt::~Tilt()
{
}
    
//==============================================================================
void Tilt::addGestureMidi (MidiBuffer& midiMessages)
{
    // Checks if Gesture is on and if value is within the right range
    if (on == false || value >= 120.0f || value <= -120.0f)
    {
        return;
    }
    
    MidiMessage message = MidiMessage::controllerEvent (1, 1, getMidiValue());
    midiMessages.addEvent(message, 1);
}

int Tilt::getMidiValue()
{
    return Gesture::normalizeMidi (range.getStart(), range.getEnd(), value);
}

void Tilt::updateMappedParameters()
{
}

float Tilt::getValueForMappedParameter (int paramId)
{
	return 0.0f;
}
    
//==============================================================================
void Tilt::updateValue (const Array<float> rawData)
{
    value = rawData[4];
}

void Tilt::addGestureParameters()
{
}