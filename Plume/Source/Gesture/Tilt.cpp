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
               plumeParameters, param::valuesIds[param::tilt_value], description),
      tiltDisplayRange (PLUME::UI::TILT_DISPLAY_MIN, PLUME::UI::TILT_DISPLAY_MAX, 1.0f),
      rangeLow  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_0]))),
      rangeHigh (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_1])))
{
    rangeLow.beginChangeGesture();
    rangeLow.setValueNotifyingHost (tiltDisplayRange.convertTo0to1 (lowValue));
    rangeLow.endChangeGesture();
    
    rangeHigh.beginChangeGesture();
    rangeHigh.setValueNotifyingHost (tiltDisplayRange.convertTo0to1 (highValue));
    rangeHigh.endChangeGesture();
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
                                   tiltDisplayRange.convertFrom0to1 (rangeLow.getValue()),
                                   tiltDisplayRange.convertFrom0to1 (rangeHigh.getValue()),
                                   (midiType == Gesture::pitch),
                                   getMidiReverse());
}

void Tilt::updateMappedParameters()
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

float Tilt::computeMappedParameterValue (Range<float> paramRange, bool reversed = false)
{
	  return Gesture::mapParameter (getGestureValue(),
                                  tiltDisplayRange.convertFrom0to1 (rangeLow.getValue()),
                                  tiltDisplayRange.convertFrom0to1 (rangeHigh.getValue()),
                                  paramRange, reversed);
}
    
//==============================================================================
void Tilt::updateValue (const Array<float> rawData)
{
    setGestureValue (-rawData[PLUME::data::tilt]);
}