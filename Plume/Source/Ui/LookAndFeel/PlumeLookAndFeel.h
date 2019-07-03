/*
  ==============================================================================

    PlumeLookAndFeel.h
    Created: 26 Jun 2018 11:25:00am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"

namespace PLUME
{
namespace UI
{
        
class PlumeLookAndFeel : public LookAndFeel_V4
{
public:
    //==============================================================================
	PlumeLookAndFeel();
	~PlumeLookAndFeel();
	
    //==============================================================================
	void setColours();
    
    //==============================================================================
	void drawLinearSlider (Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle, Slider&) override;
                           
    void drawPointer (Graphics&, float x, float y, float diameter,
                      const Colour&, const Colour&, int direction) noexcept;
    //==============================================================================
    //Font getLabelFont (Label& lbl) override;
    Font getTextButtonFont (TextButton& bttn, int buttonHeight) override;
    Font getComboBoxFont (ComboBox& cb) override;
    
    //void drawPopupMenuBackground (Graphics&, int width, int height) override;
	/*
    void drawPopupMenuItem (Graphics&, const Rectangle<int>& area,
                            bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                            const String& text, const String& shortcutKeyText,
                            const Drawable* icon, const Colour* textColour) override;
	*/
    //==============================================================================
    
private:
    
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlumeLookAndFeel)
};



//==================================================================================================
// INHERITED CLASSES: For other gesture sensibility schemes


class OneRangeTunerLookAndFeel : public PlumeLookAndFeel
{
public:
    OneRangeTunerLookAndFeel();
    ~OneRangeTunerLookAndFeel() {}

    void drawRotarySlider (Graphics &,
                           int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           Slider &) override;
                      
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

class TestTunerLookAndFeel : public PlumeLookAndFeel
{
public:
    TestTunerLookAndFeel();
    ~TestTunerLookAndFeel() {}
                      
    void drawRotarySlider (Graphics &,
                           int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           Slider &);
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestTunerLookAndFeel)
};

} // namespace UI
} // namespace PLUME