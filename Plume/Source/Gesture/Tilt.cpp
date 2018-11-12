/*
  ==============================================================================

    Tilt.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "Tilt.h"
using namespace plumeCommon;

Tilt::Tilt (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters, float lowValue, float highValue)
    : Gesture (gestName, Gesture::tilt, gestId, Range<float> (TILT_MIN, TILT_MAX), 0.0f),
    
      rangeLow  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::tilt_low]))),
      rangeHigh (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::tilt_high])))
{
	rangeLow.setValueNotifyingHost (rangeLow.convertTo0to1 (lowValue));
	rangeHigh.setValueNotifyingHost (rangeHigh.convertTo0to1 (highValue));
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
    
    if (midiMap)
    {
        addMidiModeSignalToBuffer (midiMessages, getMidiValue(), 0, 127, 1);
    }
    else
    {
        addEventAndMergeCCToBuffer (midiMessages, getMidiValue(), 1, 1);
    }
}

int Tilt::getMidiValue()
{
    return Gesture::normalizeMidi (rangeLow.convertFrom0to1 (rangeLow.getValue()), rangeHigh.convertFrom0to1 (rangeHigh.getValue()), value);
}

void Tilt::updateMappedParameters()
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

float Tilt::getValueForMappedParameter (Range<float> paramRange)
{
	return Gesture::mapParameter (value, rangeLow.convertFrom0to1 (rangeLow.getValue()), rangeHigh.convertFrom0to1 (rangeHigh.getValue()), paramRange);
}
    
//==============================================================================
void Tilt::updateValue (const Array<float> rawData)
{
    value = rawData[4];
}