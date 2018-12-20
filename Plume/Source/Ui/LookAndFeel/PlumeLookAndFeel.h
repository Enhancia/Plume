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
    //==============================================================================
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
	PlumeLookAndFeel();
	~PlumeLookAndFeel();
	
    //==============================================================================
	Colour getPlumeColour (int colourId);
	void setPlumeColour (int colourId, Colour colourValue);
    
    //==============================================================================
	void drawLinearSlider (Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle, Slider&) override;
                           
    void drawPointer (Graphics&, float x, float y, float diameter,
                      const Colour&, const Colour&, int direction) noexcept;
    //==============================================================================
    
    //void drawPopupMenuBackground (Graphics&, int width, int height) override;
	/*
    void drawPopupMenuItem (Graphics&, const Rectangle<int>& area,
                            bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                            const String& text, const String& shortcutKeyText,
                            const Drawable* icon, const Colour* textColour) override;
	*/
    //==============================================================================
        
protected:
    Colour plumePalette[PlumeColour::numColours];

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlumeLookAndFeel)
};



//==================================================================================================
// DERIVED CLASSES: For other gesture sensibility schemes


class OneRangeTunerLookAndFeel : public PlumeLookAndFeel
{
public:
    OneRangeTunerLookAndFeel()
    {
	    setColour (Slider::thumbColourId, findColour (Slider::backgroundColourId));
    }
    
    ~OneRangeTunerLookAndFeel() {}
                      
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OneRangeTunerLookAndFeel)
};



class TwoRangeTunerLookAndFeel : public PlumeLookAndFeel
{
public:
    
	TwoRangeTunerLookAndFeel() { leftSlider = true; }
	//TwoRangeTunerLookAndFeel (bool shouldBeLeft) { leftSlider = shouldBeLeft; }
	~TwoRangeTunerLookAndFeel() {}

	//==============================================================================
	void drawLinearSlider(Graphics&, int x, int y, int width, int height,
		float sliderPos, float minSliderPos, float maxSliderPos,
		const Slider::SliderStyle, Slider&) override;
		
		
	//==============================================================================
	void setSliderLeft (bool shouldBeLeft)
    {
        leftSlider = shouldBeLeft;
    }

private:
	bool leftSlider;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TwoRangeTunerLookAndFeel)
};

