/*
  ==============================================================================

    PitchBend.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "PitchBend.h"

#define rangeLeftStart  pitchBendDisplayRange.convertFrom0to1 (rangeLeftLow)
#define rangeLeftEnd    pitchBendDisplayRange.convertFrom0to1 (rangeLeftHigh)
#define rangeRightStart pitchBendDisplayRange.convertFrom0to1 (rangeRightLow)
#define rangeRightEnd   pitchBendDisplayRange.convertFrom0to1 (rangeRightHigh)

using namespace PLUME;

PitchBend::PitchBend (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
                      float leftLow, float leftHigh, float rightLow, float rightHigh, String description)
                      
    : Gesture (gestName, Gesture::pitchBend, gestId,
               NormalisableRange<float> (PLUME::gesture::PITCHBEND_MIN, PLUME::gesture::PITCHBEND_MAX, 0.1f),
               plumeParameters, param::valuesIds[param::roll_value], description),
      pitchBendDisplayRange (PLUME::UI::PITCHBEND_DISPLAY_MIN, PLUME::UI::PITCHBEND_DISPLAY_MAX, 1.0f)
{
    midiType = Gesture::pitch;
    rangeLeftLow   = pitchBendDisplayRange.convertTo0to1 (leftLow);
	rangeLeftHigh  = pitchBendDisplayRange.convertTo0to1 (leftHigh);
	rangeRightLow  = pitchBendDisplayRange.convertTo0to1 (rightLow);
	rangeRightHigh = pitchBendDisplayRange.convertTo0to1 (rightHigh);
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
        if (rangeRightLow == rangeRightHigh) currentMidi = midiRangeMiddle;
        
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
        if (rangeLeftLow == rangeLeftHigh) currentMidi = 0;
        
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
}

bool PitchBend::shouldSend()
{
    const float val = currentMidi;

    return ((val >= rangeRightStart && val < 140.0f) || // Right side
            (val <= rangeLeftEnd && val > -140.0f) || // Left side
            (val > rangeLeftEnd && val < rangeRightStart && pbLast == true)); // Back to center
}
   
bool PitchBend::shouldUpdateParameters()
{
    if (!isActive()) return false; // does nothing if the gesture is inactive
    
    updateSendLogic();

    return (send);
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
        if (rangeLeftLow == rangeLeftHigh) return reversed ? paramRange.getEnd() : paramRange.getStart();
        
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