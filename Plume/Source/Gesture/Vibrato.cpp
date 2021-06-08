/*
  ==============================================================================

    Vibrato.cpp
    Created: 9 Jul 2018 3:04:42pm
    Author:  Alex

  ==============================================================================
*/

#include "Vibrato.h"
using namespace PLUME;

Vibrato::Vibrato (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
                  float val, float thresh, String description)
    : Gesture (gestName, Gesture::vibrato, gestId,
               NormalisableRange<float> (-PLUME::gesture::VIBRATO_RANGE_MAX, PLUME::gesture::VIBRATO_RANGE_MAX, 0.1f),
               plumeParameters, param::valuesIds[param::vibrato_value], description),
      
      gainDisplayRange      (0.0f, PLUME::UI::VIBRATO_DISPLAY_MAX, 1.0f),
      thresholdDisplayRange (0.0f, PLUME::UI::VIBRATO_THRESH_DISPLAY_MAX, 1.0f),
      intensityRange (0.0f, PLUME::gesture::VIBRATO_INTENSITY_MAX, 1.0f),
      gain      (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_0]))),
      threshold (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_1]))),
      intensity (*(plumeParameters.getParameter (param::valuesIds[param::vibrato_intensity]))),
      intensityRef (plumeParameters.getRawParameterValue (PLUME::param::valuesIds[PLUME::param::vibrato_intensity]))
{
    midiType = Gesture::pitch;

    gain.beginChangeGesture();
    gain.setValueNotifyingHost (gainDisplayRange.convertTo0to1 (val));
    gain.endChangeGesture();
    
    threshold.beginChangeGesture();
    threshold.setValueNotifyingHost (thresholdDisplayRange.convertTo0to1 (thresh));
    threshold.endChangeGesture();
}

Vibrato::~Vibrato()
{
}
    
//==============================================================================
void Vibrato::addGestureMidi (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    if (!isActive()) return; // does nothing if the gesture is inactive
    
    int vibVal = getMidiValue();
    
	if (vibVal == lastMidi) return; // Does nothing if the midi value did not change

    if (send == true)
    {
        addRightMidiSignalToBuffer (midiMessages, plumeBuffer, 1);
    }
}

int Vibrato::getMidiValue()
{
    bool vibTrig = (intensityRange.convertFrom0to1 (intensity.getValue()) > thresholdDisplayRange.convertFrom0to1 (threshold.getValue()));
    float gainVal = gainDisplayRange.convertFrom0to1 (gain.getValue());
    
    // Vibrato should be triggered
    if (vibTrig && gainVal != 0.0f)
    {
        DBG ("Tresh : " << thresholdDisplayRange.convertFrom0to1 (threshold.getValue()));
        vibLast = true;
        send = true;

        const float normalizedValue = (getGestureValue()/(2*9.80665f)*gainVal/200.0f*0.5f + 0.5f);
        return Gesture::normalizeMidi (normalizedValue, 0.0f, 1.0f, useDefaultMidi);
    }
    
    // Vibrato back to neutral
    else if (vibTrig != vibLast/* && vibTrig == false*/)
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
    if (!isActive()) return; // does nothing if the gesture is inactive
    
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
    bool vibTrig = (intensityRange.convertFrom0to1 (intensity.getValue()) > thresholdDisplayRange.convertFrom0to1 (threshold.getValue()));
    float gainVal = gainDisplayRange.convertFrom0to1 (gain.getValue());
    
    if (vibTrig && gainVal != 0.0f)
    {
        vibLast = true;
        send = true;
        const float normalizedValue = (getGestureValue()/(2*9.80665f)*gainVal/200.0f*0.5f + 0.5f);
        return (Gesture::mapParameter (normalizedValue, 0.0f, 1.0f, paramRange, reversed));
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
    intensity.setValueNotifyingHost (intensityRange.convertTo0to1 (newVal));
    intensity.endChangeGesture();
}

std::atomic<float>& Vibrato::getIntensityReference()
{
    return *intensityRef;
}

//==============================================================================
void Vibrato::updateValue (const Array<float> rawData)
{
    if (isActive()) setIntensityValue (rawData[PLUME::data::acceleration]);

    const int roundedVarianceNew = roundToInt (range.convertTo0to1 (rawData[PLUME::data::variance]) * 100);
    const int roundedVarianceLast = roundToInt (value.getValue() * 100);

    DBG ("Vibrato values : " << roundedVarianceLast << " | " << roundedVarianceNew);
    
    if (roundedVarianceNew != roundedVarianceLast)
    {
        setGestureValue (rawData[PLUME::data::variance]);
    }
}
