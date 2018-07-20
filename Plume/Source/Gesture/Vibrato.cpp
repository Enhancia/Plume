/*
  ==============================================================================

    Vibrato.cpp
    Created: 9 Jul 2018 3:04:42pm
    Author:  Alex

  ==============================================================================
*/

#include "Vibrato.h"

Vibrato::Vibrato (String gestName)  : Gesture (gestName, Gesture::vibrato, Range<float> (-200.0f, 200.0f), 0.0f)
{
}
Vibrato::~Vibrato()
{
}
    
//==============================================================================
void Vibrato::addGestureMidi(MidiBuffer& midiMessages)
{
    if (on == false) return; // does nothing if the gesture is inactive
    
    int vibVal = getMidiValue();
    
    if (send == true)
    {
        // Creates the pitchwheel message
        MidiMessage message = MidiMessage::pitchWheel (1, vibVal);
        midiMessages.addEvent(message, 1);
    }
}

int Vibrato::getMidiValue ()
{
    bool vibTrig = (intensity > 40.0f);
    
    if (vibTrig && gain != 0.0f)
    {
        vibLast = true;
        send = true;
        return (Gesture::map (value, -(500.0f - gain), (500.01f - gain), 0, 16367));
    }
    else if (vibTrig != vibLast && vibTrig == false)
    {
        vibLast = false;
        send = true;
        return 8192;
    }
    
    send = false;
    return 8192;
}

void Vibrato::updateMappedParameters()
{
    // WIP
}

float Vibrato::getValueForMappedParameter(int paramId)
{
    return 0.0f; // WIP
}

//==============================================================================
void Vibrato::updateValue (const Array<float> rawData)
{
    intensity = rawData[1];
    value = rawData[0];
    
    //DBG ("values: 1 = " << intensity << " | 2 = " << value << " \n");
}

void Vibrato::addGestureParameters()
{
    // WIP
}