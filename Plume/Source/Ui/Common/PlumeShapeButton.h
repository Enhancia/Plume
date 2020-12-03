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
    //==============================================================================
	PlumeShapeButton (const String &name, Colour backgroundColour,
					  Colour normalOffColour, Colour overOffColour, Colour downOffColour,
					  Colour normalOnColour, Colour overOnColour, Colour downOnColour);

	PlumeShapeButton (const String &name, Colour backgroundColour,
					  Colour normalColour, Colour overColour, Colour downColour);

	PlumeShapeButton (const String &name, Colour backgroundColour, Colour pathColour);

	PlumeShapeButton (const String &name, Colour backgroundColour,
		              Colour pathColourOff, Colour pathColourOn);

	~PlumeShapeButton();

    //==============================================================================
	void resized() override;
	void paintButton (Graphics&, bool, bool) override;
	void setBorderSize (BorderSize<int>);
	void resetBorderSize();

    //==============================================================================
	void setBackgroundColour (const Colour newColour);

    //==============================================================================
	void setStrokeColours (const Colour newStrokeNormalColour,
						   const Colour newStrokeOverColour,
						   const Colour newStrokeDownColour);

	void setStrokeOnColours (const Colour newStrokeOnNormalColour,
						     const Colour newStrokeOnOverColour,
						     const Colour newStrokeOnDownColour);

	void setStrokeOffAndOnColours (const Colour newStrokeOffNormalColour,
						   		   const Colour newStrokeOffOverColour,
						   		   const Colour newStrokeOffDownColour,
						   		   const Colour newStrokeOnNormalColour,
						   		   const Colour newStrokeOnOverColour,
						   		   const Colour newStrokeOnDownColour);

	void setStrokeOffAndOnColours (const Colour newStrokeOffColour,
						           const Colour newStrokeOnColour);

    //==============================================================================
    enum PaintMode
    {
    	stroke =0,
    	fill,
    	fillAndStroke
    };

    void setPaintMode (const PaintMode newPaintMode);



private:
	Colour backgroundFill;
	
	PaintMode paintMode = stroke;
	Colour pathStrokeOffNormal, pathStrokeOffOver, pathStrokeOffDown;
	Colour pathStrokeOnNormal, pathStrokeOnOver, pathStrokeOnDown;
	bool usePlumeBorders = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeShapeButton)
};