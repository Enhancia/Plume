/*
  ==============================================================================

    PitchBend.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "PitchBend.h"

#define rangeLeftStart rangeLeftLow.convertFrom0to1 (rangeLeftLow.getValue())
#define rangeLeftEnd rangeLeftHigh.convertFrom0to1 (rangeLeftHigh.getValue())
#define rangeRightStart rangeRightLow.convertFrom0to1 (rangeRightLow.getValue())
#define rangeRightEnd rangeRightHigh.convertFrom0to1 (rangeRightHigh.getValue())

using namespace PLUME;

PitchBend::PitchBend (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
                      float leftLow, float leftHigh, float rightLow, float rightHigh)
                      
    : Gesture (gestName, Gesture::pitchBend, gestId, NormalisableRange<float> (PITCHBEND_MIN, PITCHBEND_MAX, 0.1f), plumeParameters),
    
      rangeLeftLow   (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::bend_leftLow]))),
      rangeLeftHigh  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::bend_leftHigh]))),
      rangeRightLow  (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::bend_rightLow]))),
      rangeRightHigh (*(plumeParameters.getParameter (String (gestId) + param::paramIds[param::bend_rightHigh])))
{
    rangeLeftLow.beginChangeGesture();
    rangeLeftLow.setValueNotifyingHost   (rangeLeftLow.convertTo0to1 (leftLow));
    rangeLeftLow.endChangeGesture();
    
    rangeLeftHigh.beginChangeGesture();
	rangeLeftHigh.setValueNotifyingHost  (rangeLeftHigh.convertTo0to1 (leftHigh));
    rangeLeftHigh.endChangeGesture();
	
	rangeRightLow.beginChangeGesture();
	rangeRightLow.setValueNotifyingHost  (rangeRightLow.convertTo0to1 (rightLow));
    rangeRightLow.endChangeGesture();
    
    rangeRightHigh.beginChangeGesture();
	rangeRightHigh.setValueNotifyingHost (rangeRightHigh.convertTo0to1 (rightHigh));
    rangeRightHigh.endChangeGesture();
}

PitchBend::~PitchBend()
{
}
    
//==============================================================================
void PitchBend::addGestureMidi (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    if (on.getValue() == 0.0f) return; // does nothing if the gesture is inactive or mapped
    
    int pbVal = getMidiValue();

	if (pbVal == lastMidi) return; // Does nothing if the midi value did not change

    if (send == true || isMidiMapped())
    {
        // Creates the pitchwheel message
        if (isMidiMapped())
        {
            addMidiModeSignalToBuffer (midiMessages, plumeBuffer, pbVal, 0, 127, 1);
        }
        else
        {
            addEventAndMergePitchToBuffer (midiMessages, plumeBuffer, pbVal, 1);
        }

		lastMidi = pbVal;
    }
}

int PitchBend::getMidiValue()
{
    // Right side
    if (getGestureValue() >= rangeRightStart && getGestureValue() < 140.0f)
    {
        send = true;
        pbLast = true;
        
        // if the range is empty just returns the max value
        if (rangeRightLow.getValue() == rangeRightHigh.getValue()) return isMidiMapped() ? 127 : 16383;
        
        // Normal case, maps to an interval from neutral to max pitch
        if (isMidiMapped()) return Gesture::map (getGestureValue(), rangeRightStart, rangeRightEnd, 64, 127);
        else                return Gesture::map (getGestureValue(), rangeRightStart, rangeRightEnd, 8192, 16383);
    }
    
    // Left side
    else if (getGestureValue() < rangeLeftEnd && getGestureValue() > -140.0f)
    {
        send = true;
        pbLast = true;
        
        // if the range is empty just returns the min value
        if (rangeLeftLow.getValue() == rangeLeftHigh.getValue()) return 0;
        
        // Normal case, maps to an interval from min pitch to neutral
        if (isMidiMapped()) return Gesture::map (getGestureValue(), rangeLeftStart, rangeLeftEnd, 0, 64);
        else         return Gesture::map (getGestureValue(), rangeLeftStart, rangeLeftEnd, 0, 8191);
    }
    
    // If back to central zone
    else if (getGestureValue() > rangeLeftEnd && getGestureValue() < rangeRightStart && pbLast == true)
    {
        send = true;
        pbLast = false;
        if (isMidiMapped()) return 64;
        else         return 8192;
    }
    
    send = false;
    if (isMidiMapped()) return 64;
    else         return 8192;
}
   
void PitchBend::updateMappedParameters()
{
    if (on.getValue() == 0.0f) return; // does nothing if the gesture is inactive
    
    bool pbLastTemp = pbLast;
    
    // Goes through the parameterArray to update each value
    for (auto* param : parameterArray)
    {
        pbLast = pbLastTemp;
		    
        if (getValueForMappedParameter(param->range, param->reversed) != param->parameter.getValue() && send == true)
        {
            param->parameter.setValueNotifyingHost (getValueForMappedParameter(param->range, param->reversed));
        }
    }
}

float PitchBend::getValueForMappedParameter (Range<float> paramRange, bool reversed = false)
{
    // Right side
    if (getGestureValue() >= rangeRightStart && getGestureValue() < 140.0f)
    {
        send = true;
		pbLast = true;
		
		// if the range is empty just returns the max value
        if (rangeRightLow.getValue() == rangeRightHigh.getValue()) return reversed ? paramRange.getStart() : paramRange.getEnd();
        
        // Normal case, maps to an interval from neutral to max
        if (!reversed) return (Gesture::mapParameter (getGestureValue(), rangeRightStart, rangeRightEnd,
                                                      Range<float> (paramRange.getStart() + paramRange.getLength()/2,
                                                                    paramRange.getEnd()),
                                                      false));
        // reversed
        else           return (Gesture::mapParameter (getGestureValue(), rangeRightStart, rangeRightEnd,
                                                      Range<float> (paramRange.getStart(),
                                                                    paramRange.getStart() + paramRange.getLength()/2),
                                                      true));
    }
    
    // Left side
    else if (getGestureValue() < rangeLeftEnd && getGestureValue() > -140.0f)
    {
        send = true;
        pbLast = true;
        
        // if the range is empty just returns the min value
        if (rangeLeftLow.getValue() == rangeLeftHigh.getValue()) return reversed ? paramRange.getEnd() : paramRange.getStart();
        
        // Normal case, maps to an interval from min to neutral
        if (!reversed) return (Gesture::mapParameter (getGestureValue(), rangeLeftStart, rangeLeftEnd,
                                                      Range<float> (paramRange.getStart(),
                                                                    paramRange.getStart() + paramRange.getLength()/2),
                                                      false));
        // reversed
        else           return (Gesture::mapParameter (getGestureValue(), rangeLeftStart, rangeLeftEnd,
                                                      Range<float> (paramRange.getStart() + paramRange.getLength()/2,
                                                                    paramRange.getEnd()),
                                                      true));
    }
    
    // If back to central zone
    else if (getGestureValue() > rangeLeftEnd && getGestureValue() < rangeRightStart && pbLast == true)
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
    setGestureValue (-rawData[5]);
}