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
    : Gesture (gestName, Gesture::roll, gestId, NormalisableRange<float> (PLUME::gesture::ROLL_MIN, PLUME::gesture::ROLL_MAX, 0.1f),
               plumeParameters, param::valuesIds[param::roll_value], description),
      rollDisplayRange (PLUME::UI::ROLL_DISPLAY_MIN, PLUME::UI::ROLL_DISPLAY_MAX, 1.0f),
      rangeLow  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_0]))),
      rangeHigh (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_1])))
{
    rangeLow.beginChangeGesture();
    rangeLow.setValueNotifyingHost (rollDisplayRange.convertTo0to1 (lowValue));
    rangeLow.endChangeGesture();
    
    rangeHigh.beginChangeGesture();
    rangeHigh.setValueNotifyingHost (rollDisplayRange.convertTo0to1 (highValue));
    rangeHigh.endChangeGesture();
}

Roll::~Roll()
{
}
    
//==============================================================================
void Roll::addGestureMidi (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    // Checks if Gesture is on and if value is within the right range
    if (!isActive() || getGestureValue() >= 100.0f || getGestureValue() <= -100.0f)
    {
        return;
    }

    addRightMidiSignalToBuffer (midiMessages, plumeBuffer, 1);
}

void Roll::updateMidiValue()
{
    currentMidi = Gesture::normalizeMidi (getGestureValue(),
                                   rollDisplayRange.convertFrom0to1 (rangeLow.getValue()),
                                   rollDisplayRange.convertFrom0to1 (rangeHigh.getValue()),
                                   (midiType == Gesture::pitch),
                                   getMidiReverse());
}

void Roll::updateMappedParameters()
{
    // Checks if Gesture is on and if value is within the right range
    if (!isActive() || getGestureValue() >= 100.0f || getGestureValue() <= -100.0f)
    {
        return;
    }
    
    // Goes through the parameterArray to update each value
    for (auto* param : parameterArray)
    {   
        param->parameter.setValueNotifyingHost (computeMappedParameterValue (param->range, param->reversed));
    }
}

float Roll::computeMappedParameterValue (Range<float> paramRange, bool reversed = false)
{
	return Gesture::mapParameter (getGestureValue(), rollDisplayRange.convertFrom0to1 (rangeLow.getValue()), rollDisplayRange.convertFrom0to1 (rangeHigh.getValue()), paramRange, reversed);
}
    
//==============================================================================
void Roll::updateValue (const Array<float> rawData)
{
    setGestureValue (rawData[PLUME::data::roll]);
}