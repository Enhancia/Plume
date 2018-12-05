/*
  ==============================================================================

    Tuner.cpp
    Created: 30 Nov 2018 4:16:50pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Tuner/Tuner.h"

#define CURSOR_SIZE 4

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))

Tuner::Tuner (const float& val, NormalisableRange<float> gestRange, const Range<float> dispRange, const String unit, bool show)
	: value(val), gestureRange(gestRange), displayRange(dispRange), valueUnit(unit), showValue(show)
{
	TRACE_IN;
	sliderPlacement = Range<int>((W * 3/4) / 8, (W *3/4) * 7/8);

	if (showValue)
	{
	    addAndMakeVisible (valueLabel = new Label ("value Label"));
	    valueLabel->setEditable (false, false, false);
	    valueLabel->setText (String (int(value)), dontSendNotification);
	    valueLabel->setFont (Font(13.0f, Font::plain));
	    valueLabel->setJustificationType(Justification::centred);
	    
	    addAndMakeVisible (cursor = new MovingCursor (value, displayRange, gestureRange, CURSOR_SIZE, 5));
	}
}

Tuner::~Tuner()
{
	TRACE_IN;
}


	//==============================================================================
void Tuner::paint(Graphics& g)
{
}

void Tuner::resized()
{
	sliderPlacement.setStart ((W* 3/4)/8);
	sliderPlacement.setEnd ((W*3/4)*7/8);

	if (showValue)
    {
	    cursor->setBounds (sliderPlacement.getStart(), H/3 - CURSOR_SIZE - 2, sliderPlacement.getLength(), CURSOR_SIZE);
	    valueLabel->setBounds ((W*3/4)*7/16, H*2/3, (W*3/4)/8, H/6);
    }
}

	//==============================================================================
void Tuner::updateDisplay()
{
    if (showValue)
	{
	    float convertedValue = gestureRange.convertFrom0to1 (value);

	    if (convertedValue >= displayRange.getStart() && convertedValue <= displayRange.getEnd())
	    {
		    valueLabel->setText (String (int (convertedValue)) + valueUnit, dontSendNotification);
			if (!(cursor->isSamePosition()))
			{
				cursor->repaint();
			}
		}
	}
}