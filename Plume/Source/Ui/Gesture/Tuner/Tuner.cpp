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
	yCursor = getHeight() / 3 - CURSOR_SIZE - 2;
	sliderPlacement = Range<int>((W * 3/4) / 8, (W *3/4) * 7/8);

	addAndMakeVisible (valueLabel = new Label("value Label"));
	valueLabel->setEditable (false, false, false);
	valueLabel->setText (String (int(value)), dontSendNotification);
	valueLabel->setFont (Font(13.0f, Font::plain));

	if (!showValue)
	{
		valueLabel->setVisible(false);
	}

	valueLabel->setJustificationType(Justification::centred);
}

Tuner::~Tuner()
{
	TRACE_IN;
}


	//==============================================================================
void Tuner::paint(Graphics& g)
{
	g.setColour(Colours::black);

	if (showValue && value > displayRange.getStart() && value < displayRange.getEnd())
	{
		drawCursor(g);
	}
}

void Tuner::resized()
{
	yCursor = H / 3 - CURSOR_SIZE - 2;
	sliderPlacement.setStart((W * 3 / 4) / 8);
	sliderPlacement.setEnd((W * 3 / 4) * 7 / 8);

	valueLabel->setBounds((W * 3 / 4) * 7 / 16, H * 2 / 3,
		(W * 3 / 4) / 8, H / 6);
}

	//==============================================================================
void Tuner::updateDisplay()
{
	float convertedValue = gestureRange.convertFrom0to1(value);

	// Normal case
	if (convertedValue >= displayRange.getStart() && convertedValue <= displayRange.getEnd())
	{
		if (showValue)   valueLabel->setText(String(int(convertedValue)) + valueUnit, dontSendNotification);
		repaint();
	}
}

	//==============================================================================
void Tuner::drawCursor(Graphics& g)
{
	int xCursor = 0;
	int valueLab = int(gestureRange.convertFrom0to1(value));

	//if (valueUnit != "") valueLab = valueLabel->getText().upToFirstOccurrenceOf(valueUnit, false, false).getIntValue();
	//else                 valueLab = valueLabel->getText().getIntValue();

	// if out of bounds doesn't draw anything
	if (valueLab < displayRange.getStart() || valueLab > displayRange.getEnd()) return;

	// Formula: x = length * (val - start)/(end - start) + x0;
	xCursor = (sliderPlacement.getLength() - 10)*(valueLab - displayRange.getStart()) / (displayRange.getEnd() - displayRange.getStart())
		+ sliderPlacement.getStart() + 5;

	/*
	// Placement if value exceeds limits
	else
	{
	if (valueLab < displayRange.getStart())      xCursor = sliderPlacement.getStart() + 5;
	else if (valueLab > displayRange.getEnd())   xCursor = sliderPlacement.getEnd() - 5;
	*/

	// draws the cursor
	Path cursorPath;

	cursorPath.startNewSubPath(xCursor - CURSOR_SIZE, yCursor);
	cursorPath.lineTo(xCursor, yCursor + CURSOR_SIZE);
	cursorPath.lineTo(xCursor + CURSOR_SIZE, yCursor);

	g.setColour(getLookAndFeel().findColour(Slider::backgroundColourId));
	g.strokePath(cursorPath, { 2.0f, PathStrokeType::curved, PathStrokeType::rounded });
}