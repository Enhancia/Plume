/*
  ==============================================================================

    Tilt.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "Tilt.h"
using namespace PLUME;

Tilt::Tilt (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
            float lowValue, float highValue, String description)
    : Gesture (gestName, Gesture::tilt, gestId, NormalisableRange<float> (PLUME::gesture::TILT_MIN, PLUME::gesture::TILT_MAX, 0.1f),
               plumeParameters, "", description),
      tiltDisplayRange (NormalisableRange<float> (PLUME::UI::TILT_DISPLAY_MIN, PLUME::UI::TILT_DISPLAY_MAX, 1.0f))
{
    rangeLow = tiltDisplayRange.convertTo0to1 (lowValue);
    rangeHigh = tiltDisplayRange.convertTo0to1 (highValue);
}

Tilt::~Tilt()
{
}
    
//==============================================================================
void Tilt::addGestureMidi (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    // Checks if Gesture is on and if value is within the right range
    if (isActive() == 0.0f || getGestureValue() >= 100.0f || getGestureValue() <= -100.0f)
    {
        return;
    }

    addRightMidiSignalToBuffer (midiMessages, plumeBuffer, 1);
}

void Tilt::updateMidiValue()
{
    currentMidi = Gesture::normalizeMidi (getGestureValue(),
                                   tiltDisplayRange.convertFrom0to1 (rangeLow),
                                   tiltDisplayRange.convertFrom0to1 (rangeHigh),
                                   (midiType == Gesture::pitch),
                                   getMidiReverse());
}

bool Tilt::shouldUpdateParameters()
{
    // Checks if Gesture is on and if value is within the right range
    return (getGestureValue() < 100.0f && getGestureValue() > -100.0f);
}

float Tilt::computeMappedParameterValue (Range<float> paramRange, bool reversed = false)
{
	return Gesture::mapParameter (getGestureValue(),
                                  tiltDisplayRange.convertFrom0to1 (rangeLow),
                                  tiltDisplayRange.convertFrom0to1 (rangeHigh),
                                  paramRange, reversed);
}

//==============================================================================
void Tilt::updateValue (const Array<float> rawData)
{
    setGestureValue (-rawData[PLUME::data::tilt]);
}