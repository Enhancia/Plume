/*
  ==============================================================================

    PlumeShapeButton.cpp
    Created: 18 Jul 2019 11:31:13am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "PlumeShapeButton.h"


PlumeShapeButton::PlumeShapeButton (const String &name, Colour backgroundColour,
					  				Colour normalColour, Colour overColour, Colour downColour,
					  				Colour normalOutlineColour, Colour overOutlineColour, Colour downOutlineColour)
	: ShapeButton (name, normalColour, overColour, downColour), backgroundFill (backgroundColour),
	  pathStrokeNormal (normalOutlineColour), pathStrokeOver (overOutlineColour), pathStrokeDown (downOutlineColour)
{
}

PlumeShapeButton::PlumeShapeButton (const String &name, Colour backgroundColour,
					  				Colour normalColour, Colour overColour, Colour downColour)
	: PlumeShapeButton (name, backgroundColour,
						normalColour, overColour, downColour,
						normalColour, overColour, downColour)
{
}

PlumeShapeButton::~PlumeShapeButton()
{
}

void PlumeShapeButton::resized()
{
	ShapeButton::resized();
	setBorderSize (BorderSize<int> (jmin (getHeight(), getWidth())/4));
}

void PlumeShapeButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	// Draws background
	Path buttonShape;
	buttonShape.addEllipse (getLocalBounds().withSizeKeepingCentre (jmin (getHeight(), getWidth()),
																	jmin (getHeight(), getWidth()))
											.toFloat());
	g.setColour (backgroundFill);
	g.fillPath (buttonShape);

	// Sets Outline Colour for ShapeButton
	if (shouldDrawButtonAsHighlighted) setOutline (pathStrokeOver,   1.0f);
	else if (shouldDrawButtonAsDown)   setOutline (pathStrokeDown,   1.0f);
	else                               setOutline (pathStrokeNormal, 1.0f);

	// Draws Regular Shape Button on top
	g.saveState();
	g.reduceClipRegion (buttonShape);
	ShapeButton::paintButton (g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
	g.restoreState();
}

void PlumeShapeButton::setBackgroundColour (const Colour newColour)
{
	backgroundFill = newColour;
	repaint();
}

void PlumeShapeButton::setStrokeColours (const Colour newStrokeNormalColour,
					   					 const Colour newStrokeOverColour,
					   					 const Colour newStrokeDownColour)
{
	pathStrokeNormal = newStrokeNormalColour;
	pathStrokeOver   = newStrokeOverColour;
	pathStrokeDown   = newStrokeDownColour;
	repaint();
}