/*
  ==============================================================================

    PlumeLookAndFeel.h
    Created: 26 Jun 2018 11:25:00am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../../../JuceLibraryCode/JuceHeader.h"

class PlumeLookAndFeel : public LookAndFeel_V4
{
public:
	PlumeLookAndFeel()
	{
	    // Sets the default colour scheme
	    ColourScheme& plumeScheme = getCurrentColourScheme();
	    plumeScheme.setUIColour (LookAndFeel_V4::ColourScheme::windowBackground, Colour (0xffe0e0e0));
	    plumeScheme.setUIColour (LookAndFeel_V4::ColourScheme::widgetBackground, Colour (0xffa0a0a0));
	    plumeScheme.setUIColour (LookAndFeel_V4::ColourScheme::outline, Colour (0xff505050));
	    
		// Sets the several components colours:

		// Label
		setColour(Label::textColourId, Colour(0xffffffff));
		setColour(Label::backgroundColourId, Colour(0xff000000));
		setColour(Label::outlineColourId, Colour(0x00000000));

		// Slider
		setColour(Slider::thumbColourId, Colour(0xffe6e6e6));
		setColour(Slider::trackColourId, Colour(0xffb7b7b7));
		setColour(Slider::backgroundColourId, Colour(0xff101010));

		// TextButton
		setColour(TextButton::buttonColourId, Colour(0xff505050));

		// ComboBox
		setColour(ComboBox::backgroundColourId, Colour(0x00000000));
		setColour(ComboBox::textColourId, Colour(0xff000000));
		setColour(ComboBox::arrowColourId, Colour(0xff000000));
		setColour(ComboBox::outlineColourId, Colour(0x00000000));
	}

	~PlumeLookAndFeel()
	{
	}

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlumeLookAndFeel)
};