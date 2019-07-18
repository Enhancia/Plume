/*
  ==============================================================================

    PlumeShapeButton.h
    Created: 18 Jul 2019 11:31:12am
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"

class PlumeShapeButton	: public ShapeButton
{
public:
	PlumeShapeButton (const String &name, Colour backgroundColour,
					  Colour normalColour, Colour overColour, Colour downColour,
					  Colour normalOutlineColour, Colour overOutlineColour, Colour downOutlineColour);

	PlumeShapeButton (const String &name, Colour backgroundColour,
					  Colour normalColour, Colour overColour, Colour downColour);

	~PlumeShapeButton();

	void resized() override;
	void paintButton (Graphics&, bool, bool) override;
	void setBackgroundColour (const Colour newColour);
	void setStrokeColours (const Colour newStrokeNormalColour,
						   const Colour newStrokeOverColour,

						   const Colour newStrokeDownColour);

private:
	Colour backgroundFill;
	Colour pathStrokeNormal, pathStrokeOver, pathStrokeDown;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeShapeButton)
};