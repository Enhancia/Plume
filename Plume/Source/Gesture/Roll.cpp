/*
  ==============================================================================

    Roll.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "Roll.h"

Roll::Roll (String gestName)  : Gesture (gestName, Gesture::tilt, Range<float> (-90.0f, 90.0f), 0.0f)
{
}

~Roll::Roll()
{
}
    
//==============================================================================
void Roll::addGestureMidi (MidiBuffer& midiMessages)
{
}

int Roll::getMidiValue()
{
}
   
void Roll::updateMappedParameters()
{
}

float Roll::getValueForMappedParameter (int paramId)
{
}
    
//==============================================================================
void Roll::updateValue (const Array<float> rawData)
{
}

void Roll::addGestureParameters()
{
}