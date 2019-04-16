/*
  ==============================================================================

    Roll.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "Roll.h"
using namespace PLUME;

Roll::Roll (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters, float lowValue, float highValue)
    : Gesture (gestName, Gesture::roll, gestId, NormalisableRange<float> (ROLL_MIN, ROLL_MAX, 0.1f), plumeParameters),
    
      rangeLow  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::roll_low]))),
      rangeHigh (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::roll_high])))
{
    rangeLow.beginChangeGesture();
	rangeLow.setValueNotifyingHost (rangeLow.convertTo0to1 (lowValue));
    rangeLow.endChangeGesture();
    
    rangeHigh.beginChangeGesture();
	rangeHigh.setValueNotifyingHost (rangeHigh.convertTo0to1 (highValue));
    rangeHigh.endChangeGesture();
}

Roll::~Roll()
{
}
    
//==============================================================================
void Roll::addGestureMidi (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    // Checks if Gesture is on and if value is within the right range
    if (on.getValue() == 0.0f || getGestureValue() >= 120.0f || getGestureValue() <= -120.0f)
    {
        return;
    }
    
	int midiVal = getMidiValue();
	if (midiVal == lastMidi) return; // Does nothing if the midi value did not change

    if (!useDefaultMidi)
    {
        addMidiModeSignalToBuffer (midiMessages, plumeBuffer, getMidiValue(), 0, 127, 1);
    }
    else
    {
        //addEventAndMergeCCToBuffer (midiMessages, plumeBuffer, getMidiValue(), 1, 1);
    }

	lastMidi = midiVal;
}

int Roll::getMidiValue()
{
    return Gesture::normalizeMidi (rangeLow.convertFrom0to1 (rangeLow.getValue()), rangeHigh.convertFrom0to1 (rangeHigh.getValue()), getGestureValue());
}

void Roll::updateMappedParameters()
{
    // Checks if Gesture is on and if value is within the right range
    if (on.getValue() == 0.0f || getGestureValue() >= 120.0f || getGestureValue() <= -120.0f)
    {
        return;
    }
    
    // Goes through the parameterArray to update each value
    for (auto* param : parameterArray)
    {   
        param->parameter.setValueNotifyingHost (getValueForMappedParameter (param->range, param->reversed));
    }
}

float Roll::getValueForMappedParameter (Range<float> paramRange, bool reversed = false)
{
	return Gesture::mapParameter (getGestureValue(), rangeLow.convertFrom0to1 (rangeLow.getValue()), rangeHigh.convertFrom0to1 (rangeHigh.getValue()), paramRange, reversed);
}
    
//==============================================================================
void Roll::updateValue (const Array<float> rawData)
{
    setGestureValue (-rawData[5]);
}