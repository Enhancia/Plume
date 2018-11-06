/*
  ==============================================================================

    PitchBend.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "PitchBend.h"

PitchBend::PitchBend (String gestName, float leftLow, float leftHigh, float rightLow, float rightHigh)
    : Gesture (gestName, Gesture::pitchBend, Range<float> (-90.0f, 90.0f), 0.0f),
      rangeLeft (Range<float> (leftLow, leftHigh)), rangeRight (Range<float> (rightLow, rightHigh))
{
}

PitchBend::~PitchBend()
{
}
    
//==============================================================================
void PitchBend::addGestureMidi (MidiBuffer& midiMessages)
{
    if (on == false) return; // does nothing if the gesture is inactive or mapped
    
    int pbVal = getMidiValue();
    
    if (send == true || midiMap == true)
    {
        // Creates the pitchwheel message
        if (midiMap)
        {
            addMidiModeSignalToBuffer (midiMessages, pbVal, 0, 127, 1);
        }
        else
        {
            addEventAndMergePitchToBuffer (midiMessages, pbVal, 1/*, pitchReference*/);
        }
    }
}

int PitchBend::getMidiValue()
{
    // Right side
    if (value>= rangeRight.getStart() && value < 140.0f)
    {
        send = true;
        pbLast = true;
        
        // if the range is empty just returns the max value
        if (rangeRight.isEmpty()) return midiMap ? 127 : 16383;
        
        // Normal case, maps to an interval from neutral to max pitch
        if (midiMap) return Gesture::map (value, rangeRight.getStart(), rangeRight.getEnd(), 64, 127);
        else         return Gesture::map (value, rangeRight.getStart(), rangeRight.getEnd(), 8192, 16383);
    }
    
    // Left side
    else if (value < rangeLeft.getEnd() && value > -140.0f)
    {
        send = true;
        pbLast = true;
        
        // if the range is empty just returns the min value
        if (rangeRight.isEmpty()) return 0;
        
        // Normal case, maps to an interval from min pitch to neutral
        if (midiMap) return Gesture::map (value, rangeLeft.getStart(), rangeLeft.getEnd(), 0, 64);
        else         return Gesture::map (value, rangeLeft.getStart(), rangeLeft.getEnd(), 0, 8191);
    }
    
    // If back to central zone
    else if (value > rangeLeft.getEnd() && value < rangeRight.getStart() && pbLast == true)
    {
        send = true;
        pbLast = false;
        if (midiMap) return 64;
        else         return 8192;
    }
    
    send = false;
    if (midiMap) return 64;
    else         return 8192;
}
   
void PitchBend::updateMappedParameters()
{
    if (on == false) return; // does nothing if the gesture is inactive
    
    float paramVal = -1.0f;
    
    // Goes through the parameterArray to update each value
    for (auto* param : parameterArray)
    {
		if (paramVal == -1.0f)
		{
		    paramVal = getValueForMappedParameter(param->range);
		}
		    
        if (send == true && paramVal != param->parameter.getValue() && paramVal != -1.0f)
        {
            param->parameter.setValueNotifyingHost (paramVal);
        }
    }
}

float PitchBend::getValueForMappedParameter (Range<float> paramRange)
{
    // Right side
    if (value>= rangeRight.getStart() && value < 140.0f)
    {
        send = true;
		pbLast = true;
		
		// if the range is empty just returns the max value
        if (rangeRight.isEmpty()) return paramRange.getEnd();
        
        // Normal case, maps to an interval from neutral to max
        return (Gesture::mapParameter (value, rangeRight.getStart(), rangeRight.getEnd(),
                                       Range<float> (paramRange.getStart() + paramRange.getLength()/2,
                                                     paramRange.getEnd())));
    }
    
    // Left side
    else if (value < rangeLeft.getEnd() && value > -140.0f)
    {
        send = true;
        pbLast = true;
        
        // if the range is empty just returns the min value
        if (rangeRight.isEmpty()) return paramRange.getStart();
        
        // Normal case, maps to an interval from min to neutral
        return (Gesture::mapParameter (value, rangeLeft.getStart(), rangeLeft.getEnd(),
                                       Range<float> (paramRange.getStart(),
                                                     paramRange.getStart() + paramRange.getLength()/2)));
    }
    
    // If back to central zone
    else if (value > rangeLeft.getEnd() && value < rangeRight.getStart() && pbLast == true)
    {
        send = true;
        pbLast = false;
        
        return paramRange.getStart() + paramRange.getLength()/2;
    }
    
    send = false;
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
    value = -rawData[5];
}

void PitchBend::addGestureParameters()
{
    //WIP
}