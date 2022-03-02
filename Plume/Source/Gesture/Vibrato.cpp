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

    if (send == true)
    {
        addRightMidiSignalToBuffer (midiMessages, plumeBuffer, 1);
    }
}

void Vibrato::updateMidiValue()
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
        currentMidi = Gesture::normalizeMidi (normalizedValue, 0.0f, 1.0f, (midiType == Gesture::pitch), getMidiReverse());
    }

    // Vibrato back to neutral
    else if (vibTrig != vibLast/* && vibTrig == false*/)
    {
        vibLast = false;
        send = true;
        
        if (!(midiType == Gesture::pitch)) currentMidi = 64;
        else                currentMidi = 8192;
    }

    // No vibrato
    else
    {
        send = false;
        if (!(midiType == Gesture::pitch)) currentMidi = 64;
        else                currentMidi = 8192;
    }
}

bool Vibrato::shouldUpdateParameters()
{
    if (!isActive()) return false; // does nothing if the gesture is inactive
    
    updateSendLogic();

    return (send);
}

void Vibrato::updateSendLogic()
{
    bool vibTrig = (intensityRange.convertFrom0to1 (intensity.getValue()) > thresholdDisplayRange.convertFrom0to1 (threshold.getValue()));
    float gainVal = gainDisplayRange.convertFrom0to1 (gain.getValue());
    
    if (vibTrig && gainVal != 0.0f)
    {
        vibLast = true;
        send = true;
    }
    else if (vibTrig != vibLast && vibTrig == false)
    {
        vibLast = false;
        send = true;
    }

    else
        send = false;
}

float Vibrato::computeMappedParameterValue (Range<float> paramRange, bool reversed = false)
{
    bool vibTrig = (intensityRange.convertFrom0to1 (intensity.getValue()) > thresholdDisplayRange.convertFrom0to1 (threshold.getValue()));
    float gainVal = gainDisplayRange.convertFrom0to1 (gain.getValue());
    
    if (vibTrig && gainVal != 0.0f)
    {
        const float normalizedValue = (getGestureValue()/(2*9.80665f)*gainVal/200.0f*0.5f + 0.5f);
        return (Gesture::mapParameter (normalizedValue, 0.0f, 1.0f, paramRange, reversed));
    }
    else if (vibTrig != vibLast && vibTrig == false)
    {
        return paramRange.getStart() + paramRange.getLength()/2;
    }
    
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
    if (isActive())
    {
        const int roundedNew = roundToInt (intensityRange.convertTo0to1 (newVal) * 100);
        const int roundedLast = roundToInt (lastIntensity * 100);

        if (roundedNew != roundedLast)
        {
            if (!wasBeingChangedIntensity)
            {
                intensity.beginChangeGesture();
                wasBeingChangedIntensity = true;
            }

            intensity.setValueNotifyingHost (intensityRange.convertTo0to1 (newVal));
            lastIntensity = intensityRange.convertTo0to1 (newVal);
        }
        else
        {
            if (wasBeingChangedIntensity)
            {
                intensity.endChangeGesture();
                wasBeingChangedIntensity = false;
            }
        }
    }
}

std::atomic<float>& Vibrato::getIntensityReference()
{
    return *intensityRef;
}

//==============================================================================
void Vibrato::updateValue (const Array<float> rawData)
{
    setIntensityValue (rawData[PLUME::data::acceleration]);
    setGestureValue (rawData[PLUME::data::variance]);
}
