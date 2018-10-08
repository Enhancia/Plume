/*
  ==============================================================================

    Roll.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "Roll.h"

Roll::Roll (String gestName, float lowValue, float highValue)
    : Gesture (gestName, Gesture::roll, Range<float> (-90.0f, 90.0f), 0.0f),
      range (Range<float> (lowValue, highValue))
{
}

Roll::~Roll()
{
}
    
//==============================================================================
void Roll::addGestureMidi (MidiBuffer& midiMessages)
{
    // Checks if Gesture is on and if value is within the right range
    if (on == false || value >= 120.0f || value <= -120.0f)
    {
        return;
    }
    
    if (midiMap)
    {
        addEventAndMergeCCToBuffer (midiMessages, getMidiValue(), cc, 1);
    }
    else
    {
        addEventAndMergeCCToBuffer (midiMessages, getMidiValue(), 1, 1);
    }
}

int Roll::getMidiValue()
{
    return Gesture::normalizeMidi (range.getStart(), range.getEnd(), value);
}

void Roll::updateMappedParameters()
{
    // Checks if Gesture is on and if value is within the right range
    if (on == false || value >= 120.0f || value <= -120.0f)
    {
        return;
    }
    
    // Goes through the parameterArray to update each value
    for (auto* param : parameterArray)
    {   
        
        param->parameter.setValueNotifyingHost (getValueForMappedParameter (param->range));
    }
}

float Roll::getValueForMappedParameter (Range<float> paramRange)
{
	return Gesture::mapParameter (value, range.getStart(), range.getEnd(), paramRange);
}
    
//==============================================================================
void Roll::updateValue (const Array<float> rawData)
{
    value = rawData[4];
}

void Roll::addGestureParameters()
{
}