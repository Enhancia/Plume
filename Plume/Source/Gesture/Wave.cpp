/*
  ==============================================================================

    Wave.cpp
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#include "Wave.h"
using namespace PLUME;

Wave::Wave (String gestName)  : Gesture (gestName, Gesture::wave, gestId, Range<float> (-90.0f, 90.0f), 0.0f)
{
}

~Wave::Wave()
{
}
    
//==============================================================================
void Wave::addGestureMidi (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
}

void Wave::updateMidiValue()
{
}
   
void Wave::updateMappedParameters()
{
}

float Wave::computeMappedParameterValue (Range<float> paramRange)
{
}
    
//==============================================================================
void Wave::updateValue (const Array<float> rawData)
{
}

void Wave::addGestureParameters()
{
}