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

    enum PlumeColour
    {
        background =0,
        topPanelBackground,
        topPanelObjects,
        gestureActiveBackground,
        gestureInactiveBackground,
        gestureActiveMapButton,
        
        numColours
    };
    
    //==============================================================================
	PlumeLookAndFeel()
	{
	    // Sets plume UI colours: ======================================
	    
	    const Colour c[] = {Colour (0xffd0d0d0),  // background
	                        Colour (0xff909090),  // topPanelBackground
	                        Colour (0xff323232),  // topPanelObjects
	                        Colour (0xffa0a0a0),  // gestureActiveBackground 
	                        Colour (0xff606060),  // gestureInactiveBackground
	                        Colour (0xff943cb0)}; // gestureActiveMapButton
	    
	    // Checks if there are enough colours in c[]
	    jassert (sizeof(c)/sizeof(c[0]) == numColours);
	        
	    for (int i=0; i<numColours; i++)
	    {
	        plumePalette[i] = c[i];
	    }
	    
	    // Sets the default colour scheme ( Default colors except
	    // 3rd value that sets the comboBox's menu background): ========
	    
	    setColourScheme ({ 0xff323e44, 0xff263238, 0xff323232,
                           0xff8e989b, 0xffffffff, 0xff42a2c8,
                           0xffffffff, 0xff181f22, 0xffffffff });
	    
                           
		// Sets the several components colours: ========================

		// Label
		setColour (Label::textColourId, Colour (0xffffffff));
		setColour (Label::backgroundColourId, Colour (0xff000000));
		setColour (Label::outlineColourId, Colour (0x00000000));

		// Slider
		setColour (Slider::thumbColourId, Colour (0xffe6e6e6));
		setColour (Slider::trackColourId, Colour (0xffb7b7b7));
		setColour (Slider::backgroundColourId, Colour (0xff101010));

		// TextButton
		setColour (TextButton::buttonColourId, Colour (0xff505050));

		// ComboBox
		setColour (ComboBox::backgroundColourId, Colour (0x00000000));
		setColour (ComboBox::textColourId, Colour (0xff000000));
		setColour (ComboBox::arrowColourId, Colour (0xff000000));
		setColour (ComboBox::outlineColourId, Colour (0x00000000));
	}

	~PlumeLookAndFeel()
	{
	}
	
    //==============================================================================
	Colour getPlumeColour (int colourId)
	{
	    if (colourId > numColours)
	    {
	        return plumePalette[1];
	    }
	    
	    return plumePalette[colourId];
	}
	
	void setPlumeColour (int colourId, Colour colourValue)
    {
        if (colourId > numColours) return;
        
        plumePalette[colourId] = colourValue;
    }
        
private:
    
    Colour plumePalette[PlumeColour::numColours];
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlumeLookAndFeel)
};