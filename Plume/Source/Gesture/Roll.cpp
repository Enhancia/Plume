/*
  ==============================================================================

    Roll.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "Roll.h"
using namespace PLUME;

Roll::Roll (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
            float lowValue, float highValue, String description)
    : Gesture (gestName, Gesture::roll, gestId, NormalisableRange<float> (ROLL_MIN, ROLL_MAX, 0.1f),
               plumeParameters, description),
    
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

    addRightMidiSignalToBuffer (midiMessages, plumeBuffer, 1);
}

int Roll::getMidiValue()
{
    return Gesture::normalizeMidi (getGestureValue(),
                                   rangeLow.convertFrom0to1 (rangeLow.getValue()),
                                   rangeHigh.convertFrom0to1 (rangeHigh.getValue()),
                                   (midiType == Gesture::pitch),
                                   getMidiReverse());
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
    setGestureValue (rawData[PLUME::data::roll]);
}