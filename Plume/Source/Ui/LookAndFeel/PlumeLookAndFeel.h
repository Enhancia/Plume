/*
  ==============================================================================

    PlumeLookAndFeel.h
    Created: 26 Jun 2018 11:25:00am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"

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
    int getDefaultScrollbarWidth() override;

    void drawScrollbar (Graphics& g, ScrollBar& scrollbar,
                        int x, int y,
                        int width, int height,
                        bool isScrollbarVertical,
                        int thumbStartPosition, int thumbSize,
                        bool isMouseOver, bool isMouseDown) override;

    //==============================================================================
    void drawPopupMenuBackground (Graphics& g, int width, int height) override;
    int getPopupMenuBorderSize() override;

    //==============================================================================
    //Font getLabelFont (Label& lbl) override;
    Font getTextButtonFont (TextButton& bttn, int buttonHeight) override;
    Font getComboBoxFont (ComboBox& cb) override;

    //==============================================================================
    void drawButtonBackground (Graphics& g,
                               Button& buttonToDraw,
                               const Colour &   backgroundColour,
                               bool  shouldDrawButtonAsHighlighted,
                               bool  shouldDrawButtonAsDown ) override;

    void drawComboBox (Graphics& g, int width, int height, bool,
                       int, int, int, int, ComboBox& box) override;

    //==============================================================================
    void drawDocumentWindowTitleBar (DocumentWindow& window, Graphics& g,
                                     int w, int h, int titleSpaceX, int titleSpaceW,
                                     const Image* icon, bool drawTitleTextOnLeft) override;
    
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
                           Slider &) override;
    
    void drawLinearSlider (Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle, Slider&) override;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestTunerLookAndFeel)
};

} // namespace UI
} // namespace PLUME
