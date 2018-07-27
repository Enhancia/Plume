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
    
    if (send == true)
    {
        // Creates the pitchwheel message
        addEventAndMergePitchToBuffer (midiMessages, pbVal, 1);
    }
}

int PitchBend::getMidiValue()
{
    if (value>= 0.0f && value < 140.0f && !(rangeRight.isEmpty()))
    {
        send = true;
        return (Gesture::map (value, rangeRight.getStart(), rangeRight.getEnd(), 8192, 16383));
    }
    
    else if (value < 0.0f && value > -140.0f && !(rangeLeft.isEmpty()))
    {
        send = true;
        return (Gesture::map (value, rangeLeft.getStart(), rangeLeft.getEnd(), 0, 8191));
    }
    
    send = false;
    return 8192;
}
   
void PitchBend::updateMappedParameters()
{
    if (on == false) return; // does nothing if the gesture is inactive
    
    float paramVal;
    
    // Goes through the parameterArray to update each value
    for (auto* param : parameterArray)
    {
        paramVal = getValueForMappedParameter (param->range);
        
        if (send == true && paramVal != param->parameter.getValue())
        {
            param->parameter.setValueNotifyingHost (paramVal);
        }
    }
}

float PitchBend::getValueForMappedParameter (Range<float> paramRange)
{
    if (value>= 0.0f && value < 140.0f && !(rangeRight.isEmpty()))
    {
        send = true;
        return (Gesture::mapParameter (value, rangeRight.getStart(), rangeRight.getEnd(),
                                       Range<float> (paramRange.getStart() + paramRange.getLength()/2,
                                                     paramRange.getEnd())));
    }
    
    else if (value < 0.0f && value > -140.0f && !(rangeLeft.isEmpty()))
    {
        send = true;
        return (Gesture::mapParameter (value, rangeLeft.getStart(), rangeLeft.getEnd(),
                                       Range<float> (paramRange.getStart(),
                                                     paramRange.getStart() + paramRange.getLength()/2)));
    }
    
    send = false;
    return paramRange.getStart() + paramRange.getLength()/2;
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