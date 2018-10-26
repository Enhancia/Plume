/*
  ==============================================================================

    PlumeLookAndFeel.h
    Created: 26 Jun 2018 11:25:00am
    Author:  Alex

  ==============================================================================
*/

#pragma once

class PlumeLookAndFeel : public LookAndFeel_V4
{
public:
	PlumeLookAndFeel()
	{
		// Sets the several components colours:

		// Label
		setColour(Label::textColourId, Colour(0xffffffff));
		setColour(Label::backgroundColourId, Colour(0xff000000));

		// Slider
		setColour(Slider::thumbColourId, Colour(0xffe6e6e6));
		setColour(Slider::trackColourId, Colour(0xffb7b7b7));
		setColour(Slider::backgroundColourId, Colour(0xff101010));

		// TextButton
		setColour(TextButton::buttonColourId, Colour(0xff505050));

		// ComboBox
		setColour(ComboBox::backgroundColourId, Colour(0x0000000));
		setColour(ComboBox::textColourId, Colour(0xff000000));
		setColour(ComboBox::arrowColourId, Colour(0xff000000));
		setColour(ComboBox::outlineColourId, Colour(0xff000000));
	}

	~PlumeLookAndFeel()
	{
	}

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlumeLookAndFeel)
};