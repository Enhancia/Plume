/*
  ==============================================================================

    Tuner.cpp
    Created: 30 Nov 2018 4:16:50pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Tuner/Tuner.h"

#define CURSOR_SIZE 4

Tuner::Tuner (const String unit, Colour colour)
	: valueUnit(unit), tunerColour (colour)
{
	TRACE_IN;
	sliderPlacement = Range<int>((getWidth() * 3/4) / 8, (getWidth() *3/4) * 7/8);
}

Tuner::~Tuner()
{
	TRACE_IN;
}


	//==============================================================================
void Tuner::paint(Graphics&)
{
}

void Tuner::resized()
{
	sliderPlacement.setStart ((getWidth()* 3/4)/8);
	sliderPlacement.setEnd ((getWidth()*3/4)*7/8);
}

	//==============================================================================
void Tuner::updateDisplay()
{
}