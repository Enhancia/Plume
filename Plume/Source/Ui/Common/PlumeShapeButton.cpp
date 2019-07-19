/*
  ==============================================================================

    PlumeShapeButton.cpp
    Created: 18 Jul 2019 11:31:13am
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "PlumeShapeButton.h"


PlumeShapeButton::PlumeShapeButton (const String &name, Colour backgroundColour,
					  				Colour normalOffColour, Colour overOffColour, Colour downOffColour,
					  				Colour normalOnColour, Colour overOnColour, Colour downOnColour)
	: ShapeButton (name, Colour (0x00000000),
						 Colour (0x00000000),
						 Colour (0x00000000)),
	  backgroundFill (backgroundColour),
	  pathStrokeOffNormal (normalOffColour), pathStrokeOffOver (overOffColour), pathStrokeOffDown (downOffColour),
	  pathStrokeOnNormal (normalOnColour),   pathStrokeOnOver (overOnColour),   pathStrokeOnDown (downOnColour)
{
}

PlumeShapeButton::PlumeShapeButton (const String &name, Colour backgroundColour,
					  				Colour normalColour, Colour overColour, Colour downColour)
	: PlumeShapeButton (name, backgroundColour,
						normalColour, overColour, downColour,
						normalColour, overColour, downColour)
{
}

PlumeShapeButton::PlumeShapeButton (const String &name, Colour backgroundColour, Colour pathColour)
	: PlumeShapeButton (name, backgroundColour, pathColour,
							  				    pathColour.withAlpha (0.5f),
							  					pathColour.withAlpha (0.3f))
{
}

PlumeShapeButton::PlumeShapeButton (const String &name, Colour backgroundColour,
									Colour pathColourOff, Colour pathColourOn)
	: PlumeShapeButton (name, backgroundColour, pathColourOff,
							  				    pathColourOff.withAlpha (0.5f),
							  					pathColourOff.withAlpha (0.3f),
							  					pathColourOn,
							  				    pathColourOn.withAlpha (0.5f),
							  					pathColourOn.withAlpha (0.3f))
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
	if (shouldDrawButtonAsHighlighted) setOutline (getToggleState() ? pathStrokeOnOver
																	: pathStrokeOffOver,
												   1.0f);

	else if (shouldDrawButtonAsDown)   setOutline (getToggleState() ? pathStrokeOnDown
																	: pathStrokeOffDown,
												   1.0f);

	else                               setOutline (getToggleState() ? pathStrokeOnNormal
																	: pathStrokeOffNormal,
												   1.0f);

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
	setStrokeOffAndOnColours (newStrokeNormalColour, newStrokeOverColour, newStrokeDownColour,
					          newStrokeNormalColour, newStrokeOverColour, newStrokeDownColour);
}

void PlumeShapeButton::setStrokeOnColours (const Colour newStrokeOnNormalColour,
					     				   const Colour newStrokeOnOverColour,
					     				   const Colour newStrokeOnDownColour)
{
	pathStrokeOnNormal = newStrokeOnNormalColour;
	pathStrokeOnOver   = newStrokeOnOverColour;
	pathStrokeOnDown   = newStrokeOnDownColour;

	repaint();
}

void PlumeShapeButton::setStrokeOffAndOnColours (const Colour newStrokeOffNormalColour,
					   					 		 const Colour newStrokeOffOverColour,
					   					 		 const Colour newStrokeOffDownColour,
					   					 		 const Colour newStrokeOnNormalColour,
					   					 		 const Colour newStrokeOnOverColour,
					   					 		 const Colour newStrokeOnDownColour)
{
	pathStrokeOffNormal = newStrokeOffNormalColour;
	pathStrokeOffOver   = newStrokeOffOverColour;
	pathStrokeOffDown   = newStrokeOffDownColour;

	pathStrokeOnNormal = newStrokeOnNormalColour;
	pathStrokeOnOver   = newStrokeOnOverColour;
	pathStrokeOnDown   = newStrokeOnDownColour;

	repaint();
}

void PlumeShapeButton::setStrokeOffAndOnColours (const Colour newStrokeOffColour,
					   					 		 const Colour newStrokeOnColour)
{
	setStrokeOffAndOnColours (newStrokeOffColour, newStrokeOffColour.withAlpha (0.5f), 
												  newStrokeOffColour.withAlpha (0.3f),
					          newStrokeOnColour,  newStrokeOnColour.withAlpha  (0.5f),  
					          					  newStrokeOnColour.withAlpha  (0.3f));
}

