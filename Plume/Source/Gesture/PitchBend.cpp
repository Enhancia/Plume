/*
  ==============================================================================

    PitchBend.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "PitchBend.h"

#define rangeLeftStart  pitchBendDisplayRange.convertFrom0to1 (rangeLeftLow.getValue())
#define rangeLeftEnd    pitchBendDisplayRange.convertFrom0to1 (rangeLeftHigh.getValue())
#define rangeRightStart pitchBendDisplayRange.convertFrom0to1 (rangeRightLow.getValue())
#define rangeRightEnd   pitchBendDisplayRange.convertFrom0to1 (rangeRightHigh.getValue())

using namespace PLUME;

PitchBend::PitchBend (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
                      float leftLow, float leftHigh, float rightLow, float rightHigh, String description)
                      
    : Gesture (gestName, Gesture::pitchBend, gestId,
               NormalisableRange<float> (PLUME::gesture::PITCHBEND_MIN, PLUME::gesture::PITCHBEND_MAX, 0.1f),
               plumeParameters, param::valuesIds[param::roll_value], description),

      pitchBendDisplayRange (PLUME::UI::PITCHBEND_DISPLAY_MIN, PLUME::UI::PITCHBEND_DISPLAY_MAX, 1.0f),
      rangeLeftLow   (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_0]))),
      rangeLeftHigh  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_1]))),
      rangeRightLow  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_2]))),
      rangeRightHigh (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::gesture_param_3])))
{
    midiType = Gesture::pitch;
    
    rangeLeftLow.beginChangeGesture();
    rangeLeftLow.setValueNotifyingHost   (pitchBendDisplayRange.convertTo0to1 (leftLow));
    rangeLeftLow.endChangeGesture();
    
    rangeLeftHigh.beginChangeGesture();
	rangeLeftHigh.setValueNotifyingHost  (pitchBendDisplayRange.convertTo0to1 (leftHigh));
    rangeLeftHigh.endChangeGesture();
	
	rangeRightLow.beginChangeGesture();
	rangeRightLow.setValueNotifyingHost  (pitchBendDisplayRange.convertTo0to1 (rightLow));
    rangeRightLow.endChangeGesture();
    
    rangeRightHigh.beginChangeGesture();
	rangeRightHigh.setValueNotifyingHost (pitchBendDisplayRange.convertTo0to1 (rightHigh));
    rangeRightHigh.endChangeGesture();
}

PitchBend::~PitchBend()
{
}
    
//==============================================================================
void PitchBend::addGestureMidi(MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    if (!isActive()) return; // does nothing if the gesture is inactive or mapped

    if (send)
    {
        // Creates the pitchwheel message
        addRightMidiSignalToBuffer(midiMessages, plumeBuffer, 1);

        //if (lastMidi == (midiType == Gesture::pitch) ? 8192 : 64) pbLast = false;
    }
}

void PitchBend::updateMidiValue()
{
    /*if (!(getGestureValue() >= rangeLeftEnd && getGestureValue() <= rangeRightStart && pbLast == true))
    {
        DBG ("COMPUTE PITCH BEND !! \nGesture value : " << getGestureValue() <<
            "\nSend bool     : " << (send ? "Y" : "N") <<
            "\nbLast bool     : " << (pbLast ? "Y" : "N"));
    }*/
    const int midiRangeHigh   = (midiType == Gesture::pitch) ? 16383 : 127;
    const int midiRangeMiddle = (midiType == Gesture::pitch) ? 8192 : 64;

    // Right side
    if (getGestureValue() >= rangeRightStart && getGestureValue() < 140.0f)
    {
        /*
        DBG ("PITCH BEND case RIGHT = \nValue " << getGestureValue() <<
            " | Send " << (send ? "Y" : "N") <<
            " | Last " << (pbLast ? "Y" : "N"));*/

        send = true;
        pbLast = true;
        
        // if the range is empty just returns the max value
        if (rangeRightLow.getValue() == rangeRightHigh.getValue()) currentMidi = midiRangeMiddle;
        
        // Normal case, maps to an interval from neutral to max pitch
        if (!getMidiReverse()) currentMidi = Gesture::map (getGestureValue(), rangeRightStart, rangeRightEnd, midiRangeMiddle, midiRangeHigh);
        else   currentMidi = midiRangeHigh - Gesture::map (getGestureValue(), rangeRightStart, rangeRightEnd, midiRangeMiddle, midiRangeHigh);
    }
    
    // Left side
    else if (getGestureValue() <= rangeLeftEnd && getGestureValue() > -140.0f)
    {
        /*
        DBG ("PITCH BEND case LEFT = \nValue " << getGestureValue() <<
            " | Send " << (send ? "Y" : "N") <<
            " | Last " << (pbLast ? "Y" : "N"));*/

        send = true;
        pbLast = true;
        
        // if the range is empty just returns the min value
        if (rangeLeftLow.getValue() == rangeLeftHigh.getValue()) currentMidi = 0;
        
        // Normal case, maps to an interval from min pitch to neutral
        if (!getMidiReverse()) currentMidi = Gesture::map (getGestureValue(), rangeLeftStart, rangeLeftEnd, 0, midiRangeMiddle);
        else   currentMidi = midiRangeHigh - Gesture::map (getGestureValue(), rangeLeftStart, rangeLeftEnd, 0, midiRangeMiddle);
    }
    
    // If back to central zone
    else if (getGestureValue() > rangeLeftEnd && getGestureValue() < rangeRightStart && pbLast == true)
    {
        /*
        DBG ("PITCH BEND case CENTER = \nValue " << getGestureValue() <<
            " | Send " << (send ? "Y" : "N") <<
            " | Last " << (pbLast ? "Y" : "N"));*/

        send = true;
        pbLast = false;
        currentMidi = midiRangeMiddle;
    }
    else
    {
        send = false;
        currentMidi = midiRangeMiddle;
    }

    DBG ("Updated pitch bend :\nValue : " << currentMidi <<
        " | Last Value : " << lastMidi <<
        "\nSend " << (send ? "Y" : "N") <<
        " | Last bool " << (pbLast ? "Y" : "N"));
}

bool PitchBend::shouldSend()
{
    const float val = currentMidi;

    return ((val >= rangeRightStart && val < 140.0f) || // Right side
            (val <= rangeLeftEnd && val > -140.0f) || // Left side
            (val > rangeLeftEnd && val < rangeRightStart && pbLast == true)); // Back to center
}
   
void PitchBend::updateMappedParameters()
{
    if (!isActive()) return; // does nothing if the gesture is inactive
    
    updateSendLogic();

    if (send)
    {
        // Goes through the parameterArray to update each value
        for (auto* param : parameterArray)
        {
            const float newParamValue = computeMappedParameterValue (param->range, param->reversed);

            if (newParamValue != param->parameter.getValue())
            {
                param->parameter.setValueNotifyingHost (newParamValue);
            }
        }
    }
}

void PitchBend::updateSendLogic()
{
    const float gestureValue = getGestureValue();

    // Right side
    if (gestureValue >= rangeRightStart && gestureValue < 140.0f)
    {
        send = true;
		pbLast = true;
    }
    
    // Left side
    else if (gestureValue < rangeLeftEnd && gestureValue > -140.0f)
    {
        send = true;
        pbLast = true;
    }
    
    // If back to central zone
    else if (gestureValue > rangeLeftEnd && gestureValue < rangeRightStart && pbLast == true)
    {
        send = true;
        pbLast = false;
    }
    
    else
        send = false;
}

float PitchBend::computeMappedParameterValue (Range<float> paramRange, bool reversed = false)
{
    const float gestureValue = getGestureValue();
    
    // Right side
    if (gestureValue >= rangeRightStart && gestureValue < 140.0f)
    {
        // Normal case, maps to an interval from neutral to max
        if (!reversed) return (Gesture::mapParameter (gestureValue, rangeRightStart, rangeRightEnd,
                                                      Range<float> (paramRange.getStart() + paramRange.getLength()/2,
                                                                    paramRange.getEnd()),
                                                      false));
        // reversed
        else           return (Gesture::mapParameter (gestureValue, rangeRightStart, rangeRightEnd,
                                                      Range<float> (paramRange.getStart(),
                                                                    paramRange.getStart() + paramRange.getLength()/2),
                                                      true));
    }
    
    // Left side
    else if (gestureValue < rangeLeftEnd && gestureValue > -140.0f)
    {
        // if the range is empty just returns the min value
        if (rangeLeftLow.getValue() == rangeLeftHigh.getValue()) return reversed ? paramRange.getEnd() : paramRange.getStart();
        
        // Normal case, maps to an interval from min to neutral
        if (!reversed) return (Gesture::mapParameter (gestureValue, rangeLeftStart, rangeLeftEnd,
                                                      Range<float> (paramRange.getStart(),
                                                                    paramRange.getStart() + paramRange.getLength()/2),
                                                      false));
        // reversed
        else           return (Gesture::mapParameter (gestureValue, rangeLeftStart, rangeLeftEnd,
                                                      Range<float> (paramRange.getStart() + paramRange.getLength()/2,
                                                                    paramRange.getEnd()),
                                                      true));
    }

    return paramRange.getStart() + paramRange.getLength()/2;
}

//==============================================================================
bool PitchBend::getSend()
{
    return send;
}

//==============================================================================
void PitchBend::updateValue (const Array<float> rawData)
{
    setGestureValue (rawData[PLUME::data::roll]);
}

void PitchBend::setActive (bool shouldBeOn)
{
    Gesture::setActive (shouldBeOn);

    if (shouldBeOn) pbLast = true;
}