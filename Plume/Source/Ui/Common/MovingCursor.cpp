/*
  ==============================================================================

    MovingCursor.cpp
    Created: 4 Dec 2018 9:55:10am
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Common/MovingCursor.h"

//==============================================================================
MovingCursor::MovingCursor(const float& val, const Range<float> disp, NormalisableRange<float> convt, int size, int offsetFromSide)
	: value(val), displayRange(disp)
{
	convertRange = convt;
	cursorSize = size;
	lastx = getWidth()/2;
	offsetFromSide < getWidth()/2 ? offset = offsetFromSide : offset = 0;
}

MovingCursor::~MovingCursor()
{
}

//==============================================================================
void MovingCursor::paint (Graphics& g)
{
	drawCursor(g);
}

void MovingCursor::resized()
{
    repaint();
}

//==============================================================================
bool MovingCursor::isSamePosition()
{
	return getPosition() == lastx;
}

//==============================================================================
int MovingCursor::getPosition()
{
	int valueConv = int(convertRange.convertFrom0to1(value));

	// Placement if value exceeds limits
	if (valueConv < displayRange.getStart())      return offset;
	else if (valueConv > displayRange.getEnd())   return getWidth() - offset;

	// New position, with formula: x = length * (val - start)/(end - start) + x0;
	return int ((getWidth() - 2*offset) * (valueConv - displayRange.getStart()) / (displayRange.getEnd() - displayRange.getStart()) + offset);
}

void MovingCursor::drawCursor (Graphics& g)
{
    int xCursor = lastx;

	// New position
	xCursor = getPosition();

	// doesn't paint if the cursor is at either extreme position (ie the value is outside the display range)
	if (xCursor == offset || xCursor == getWidth() - offset) return;

	lastx = xCursor;

	// draws the cursor
	Path cursorPath;

	cursorPath.startNewSubPath(float (xCursor - cursorSize), 0.0f);
	cursorPath.lineTo (float (xCursor), float(getHeight()));
	cursorPath.lineTo (float (xCursor + cursorSize), 0.0f);

	g.setColour(getLookAndFeel().findColour(Slider::backgroundColourId));
	g.strokePath(cursorPath, { 2.0f, PathStrokeType::curved, PathStrokeType::rounded });
}