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
      rollDisplayRange (PLUME::UI::ROLL_DISPLAY_MIN, PLUME::UI::ROLL_DISPLAY_MAX, 1.0f)
{
    rangeLow = rollDisplayRange.convertTo0to1 (lowValue);    
    rangeHigh = rollDisplayRange.convertTo0to1 (highValue);
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
                                   rollDisplayRange.convertFrom0to1 (rangeLow),
                                   rollDisplayRange.convertFrom0to1 (rangeHigh),
                                   (midiType == Gesture::pitch),
                                   getMidiReverse());
}

bool Roll::shouldUpdateParameters()
{
    // Checks if Gesture is on and if value is within the right range
    return (getGestureValue() < 100.0f && getGestureValue() > -100.0f);
}

float Roll::computeMappedParameterValue (Range<float> paramRange, bool reversed = false)
{
    return Gesture::mapParameter (getGestureValue(), rollDisplayRange.convertFrom0to1 (rangeLow), rollDisplayRange.convertFrom0to1 (rangeHigh), paramRange, reversed);
}
    
//==============================================================================
void Roll::updateValue (const Array<float> rawData)
{
    setGestureValue (rawData[PLUME::data::roll]);
}