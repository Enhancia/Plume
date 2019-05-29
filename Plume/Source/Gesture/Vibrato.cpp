/*
  ==============================================================================

    Vibrato.cpp
    Created: 9 Jul 2018 3:04:42pm
    Author:  Alex

  ==============================================================================
*/

#include "Gesture/Vibrato.h"
using namespace PLUME;

Vibrato::Vibrato (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
                  float val, float thresh, String description)
    : Gesture (gestName, Gesture::vibrato, gestId,
               NormalisableRange<float> (-VIBRATO_RANGE_MAX, VIBRATO_RANGE_MAX, 0.1f),
               plumeParameters, description),
    
      gain      (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::vibrato_range]))),
      intensity (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::vibrato_intensity]))),
      threshold (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::vibrato_thresh])))
{
    gain.beginChangeGesture();
    gain.setValueNotifyingHost (gain.convertTo0to1 (val));
    gain.endChangeGesture();
    
    threshold.beginChangeGesture();
    threshold.setValueNotifyingHost (threshold.convertTo0to1 (thresh));
    threshold.endChangeGesture();
}

Vibrato::~Vibrato()
{
}
    
//==============================================================================
void Vibrato::addGestureMidi (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    if (on.getValue() == 0.0f) return; // does nothing if the gesture is inactive
    
    int vibVal = getMidiValue();
    
	if (vibVal == lastMidi) return; // Does nothing if the midi value did not change

    if (send == true)
    {
        // Creates the control change message
        if (!useDefaultMidi)
        {
            addMidiModeSignalToBuffer (midiMessages, plumeBuffer, vibVal, 0, 127, 1);
        }
        // Creates the pitchwheel message
        else
        {
            addEventAndMergePitchToBuffer (midiMessages, plumeBuffer, vibVal, 1/*, pitchReference*/);
        }

		lastMidi = vibVal;
    }
}

int Vibrato::getMidiValue()
{
    bool vibTrig = (intensity.convertFrom0to1 (intensity.getValue()) > threshold.convertFrom0to1 (threshold.getValue()));
    float gainVal = gain.convertFrom0to1 (gain.getValue());
    
    // Vibrato should be triggered
    if (vibTrig && gainVal != 0.0f)
    {
        vibLast = true;
        send = true;
        
        if (!useDefaultMidi) return Gesture::normalizeMidi (-(500.0f - gainVal), (500.01f - gainVal), getGestureValue());
        else                return Gesture::map (getGestureValue(), -(500.0f - gainVal), (500.01f - gainVal), 0, 16383);
    }
    
    // Vibrato back to neutral
    else if (vibTrig != vibLast && vibTrig == false)
    {
        vibLast = false;
        send = true;
        
        if (!useDefaultMidi) return 64;
        else                return 8192;
    }
    
    // No vibrato
    send = false;
    if (!useDefaultMidi) return 64;
    else                return 8192;
}

void Vibrato::updateMappedParameters()
{
    if (on.getValue() == 0.0f) return; // does nothing if the gesture is inactive
    
    bool vibLastTemp = vibLast;
    
    // Goes through the parameterArray to update each value
    for (auto* param : parameterArray)
    {
		vibLast = vibLastTemp;
		float paramVal = getValueForMappedParameter(param->range, param->reversed);
        
        if (send == true)
        {
            param->parameter.setValueNotifyingHost (paramVal);
        }
    }
}

float Vibrato::getValueForMappedParameter (Range<float> paramRange, bool reversed = false)
{
    bool vibTrig = (intensity.convertFrom0to1 (intensity.getValue()) > threshold.convertFrom0to1 (threshold.getValue()));
    float gainVal = gain.convertFrom0to1 (gain.getValue());
    
    if (vibTrig && gainVal != 0.0f)
    {
        vibLast = true;
        send = true;
        return (Gesture::mapParameter (getGestureValue(), -(500.0f - gainVal), (500.01f - gainVal), paramRange, reversed));
    }
    else if (vibTrig != vibLast && vibTrig == false)
    {
        vibLast = false;
        send = true;
        return paramRange.getStart() + paramRange.getLength()/2;
    }
    
    send = false;
    return paramRange.getStart() + paramRange.getLength()/2;
}

//==============================================================================
bool Vibrato::getSend()
{
    return send;
}

//==============================================================================
void Vibrato::setIntensityValue (float newVal)
{
    intensity.beginChangeGesture();
    intensity.setValueNotifyingHost (intensity.convertTo0to1 (newVal));
    intensity.endChangeGesture();
}

//==============================================================================
void Vibrato::updateValue (const Array<float> rawData)
{
    if (isActive()) setIntensityValue (rawData[1]);
    setGestureValue (rawData[0]);
}