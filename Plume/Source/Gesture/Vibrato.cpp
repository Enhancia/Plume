/*
  ==============================================================================

    Vibrato.cpp
    Created: 9 Jul 2018 3:04:42pm
    Author:  Alex

  ==============================================================================
*/

#include "Gesture/Vibrato.h"
using namespace plumeCommon;

Vibrato::Vibrato (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters, float val)
    : Gesture (gestName, Gesture::vibrato, gestId, Range<float> (0, VIBRATO_RANGE_MAX), 0.0f),
    
      gain  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::vibrato_range]))),
      threshold (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::vibrato_thresh])))
{
    gain.setValueNotifyingHost (gain.convertTo0to1 (val));
}

Vibrato::~Vibrato()
{
}
    
//==============================================================================
void Vibrato::addGestureMidi (MidiBuffer& midiMessages)
{
    if (on == false) return; // does nothing if the gesture is inactive
    
    int vibVal = getMidiValue();
    
    if (send == true || midiMap == true)
    {
        // Creates the control change message
        if (midiMap)
        {
            addMidiModeSignalToBuffer (midiMessages, vibVal, 0, 127, 1);
        }
        // Creates the pitchwheel message
        else
        {
            addEventAndMergePitchToBuffer (midiMessages, vibVal, 1/*, pitchReference*/);
        }
    }
}

int Vibrato::getMidiValue()
{
    bool vibTrig = (intensity > threshold.convertFrom0to1 (threshold.getValue()));
    float gainVal = gain.convertFrom0to1 (gain.getValue());
    
    // Vibrato should be triggered
    if (vibTrig && gainVal != 0.0f)
    {
        vibLast = true;
        send = true;
        
        if (midiMap) return Gesture::normalizeMidi (-(500.0f - gainVal), (500.01f - gainVal), value);
        else         return Gesture::map (value, -(500.0f - gainVal), (500.01f - gainVal), 0, 16383);
    }
    
    // Vibrato back to neutral
    else if (vibTrig != vibLast && vibTrig == false)
    {
        vibLast = false;
        send = true;
        
        if (midiMap) return 64;
        else         return 8192;
    }
    
    // No vibrato
    send = false;
    if (midiMap) return 64;
    else         return 8192;
}

void Vibrato::updateMappedParameters()
{
    if (on == false) return; // does nothing if the gesture is inactive
    
    bool vibLastTemp = vibLast;
    
    // Goes through the parameterArray to update each value
    for (auto* param : parameterArray)
    {
		vibLast = vibLastTemp;
		float paramVal = getValueForMappedParameter(param->range);
        
        if (send == true)
        {
            param->parameter.setValueNotifyingHost (paramVal);
        }
    }
}

float Vibrato::getValueForMappedParameter (Range<float> paramRange)
{
    bool vibTrig = (intensity > threshold.convertFrom0to1 (threshold.getValue()));
    float gainVal = gain.convertFrom0to1 (gain.getValue());
    
    if (vibTrig && gainVal != 0.0f)
    {
        vibLast = true;
        send = true;
        return (Gesture::mapParameter (value, -(500.0f - gainVal), (500.01f - gainVal), paramRange));
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
void Vibrato::updateValue (const Array<float> rawData)
{
    intensity = rawData[1];
    value = rawData[0];
}