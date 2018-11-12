/*
  ==============================================================================

    Roll.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "Roll.h"
using namespace plumeCommon;

Roll::Roll (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters, float lowValue, float highValue)
    : Gesture (gestName, Gesture::roll, gestId, Range<float> (ROLL_MIN, ROLL_MAX), 0.0f),
    
      rangeLow  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::roll_low]))),
      rangeHigh (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::roll_high])))
{
	rangeLow.setValueNotifyingHost (rangeLow.convertTo0to1 (lowValue));
	rangeHigh.setValueNotifyingHost (rangeHigh.convertTo0to1 (highValue));
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
        addMidiModeSignalToBuffer (midiMessages, getMidiValue(), 0, 127, 1);
    }
    else
    {
        //addEventAndMergeCCToBuffer (midiMessages, getMidiValue(), 1, 1);
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
    value = -rawData[5];
}