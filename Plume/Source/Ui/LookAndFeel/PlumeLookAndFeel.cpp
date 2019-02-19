/*
  ==============================================================================

    PlumeLookAndFeel.cpp
    Created: 29 Oct 2018 11:34:21am
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "PlumeLookAndFeel.h"

using namespace PLUME::UI;

PlumeLookAndFeel::PlumeLookAndFeel()
{
	// Sets the default colour scheme ( Default colors except
	// 3rd value that sets the comboBox's menu background): ========
	/*
	setColourScheme ({ 0xffee00ff, 0xfffffd00, 0xff323232,
                       0xff8e989b, 0xffe5e5e5, 0xff42a2c8,
                       0xffe5e5e5, 0xff323232, 0xffe5e5e5 });
	*/
                           
	// Sets the several components colours: ========================
	setColours();
	
	
}

PlumeLookAndFeel::~PlumeLookAndFeel()
{
}

void PlumeLookAndFeel::setColours()
{
    using namespace PLUME;
    
    // Label
	setColour (Label::textColourId, Colour (0xffffffff));
	setColour (Label::backgroundColourId, Colour (0xff000000));
	setColour (Label::outlineColourId, Colour (0x00000000));

    // Text 
    setColour (TextEditor::outlineColourId, Colour (0x00000000));
    setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    setColour (TextEditor::focusedOutlineColourId, Colour (0x10a0a0a0));
    setColour (TextEditor::highlightColourId, Colour (0x50ffffff));
    setColour (CaretComponent::caretColourId, Colour (0xffffffff));

	// Slider
	setColour (Slider::thumbColourId, Colour (UI::currentTheme[colour::PlumeColourID::basePanelGestureHighlightedBackground]));
	setColour (Slider::trackColourId, Colour (UI::currentTheme[colour::PlumeColourID::basePanelGestureHighlightedBackground]));
	setColour (Slider::backgroundColourId, Colour (0xff101010));

	// TextButton
	setColour (TextButton::buttonColourId, Colour (0xff505050));

	// ComboBox
	setColour (ComboBox::backgroundColourId, Colour (0x00000000));
	setColour (ComboBox::textColourId, Colour (0xff000000));
	setColour (ComboBox::arrowColourId, Colour (0xff000000));
	setColour (ComboBox::outlineColourId, Colour (0x00000000)); // Necessary to not draw Label outlines due to an Error in the JUCE code..
	
	// PopupMenu
	setColour (PopupMenu::backgroundColourId, Colour (0xff323232));
	setColour (PopupMenu::textColourId, Colour (0xffe5e5e5));
	setColour (PopupMenu::highlightedBackgroundColourId, Colour (0xaae5e5e5));
	setColour (PopupMenu::highlightedTextColourId, Colour (0xff323232));
}

Font PlumeLookAndFeel::getLabelFont (Label& lbl)
{
    return PLUME::UI::plumeFont;
}

Font PlumeLookAndFeel::getTextButtonFont (TextButton& bttn, int buttonHeight)
{
    //return Font (PLUME::UI::font, float (buttonHeight*6)/10.0f, Font::plain);
    return PLUME::UI::plumeFont;
}

void PlumeLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos,
                                       float minSliderPos,
                                       float maxSliderPos,
                                       const Slider::SliderStyle style, Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
                                          : Rectangle<float> (x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
    }
    else
    {
        auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = jmin (6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);

        Point<float> startPoint (slider.isHorizontal() ? x : x + width * 0.5f,
                                 slider.isHorizontal() ? y + height * 0.5f : height + y);

        Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                               slider.isHorizontal() ? startPoint.y : y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, {  2.0f /*trackWidth*/, PathStrokeType::curved, PathStrokeType::rounded }); // changed track width

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : width * 0.5f,
                         slider.isHorizontal() ? height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : width * 0.5f,
                               slider.isHorizontal() ? height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : width * 0.5f,
                         slider.isHorizontal() ? height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
            auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        auto thumbWidth = getSliderThumbRadius (slider);

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        
        PathStrokeType (3.0f).createStrokedPath (valueTrack, valueTrack);
		

		g.fillPath(valueTrack);
		g.setColour(slider.findColour(Slider::backgroundColourId));
		g.strokePath(valueTrack, { 2.0f /*trackWidth*/, PathStrokeType::curved, PathStrokeType::rounded });
        

        if (! isTwoVal)
        {
            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillEllipse (Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre (isThreeVal ? thumbPoint : maxPoint));
        }

        if (isTwoVal || isThreeVal)
        {
            auto sr = jmin (trackWidth, (slider.isHorizontal() ? height : width) * 0.4f);
            auto pointerColour = slider.findColour (Slider::thumbColourId);
            auto trackColour = slider.findColour (Slider::backgroundColourId); // added: gets backgroundtrack colour
            
            // drawPointer also gets the track colour argument
            if (slider.isHorizontal())
            {
                drawPointer (g, minSliderPos - sr,
                             jmax (0.0f, y + height * 0.5f - trackWidth * 2.0f),
                             trackWidth * 2.0f, pointerColour, trackColour, 2);

                drawPointer (g, maxSliderPos - trackWidth,
                             jmin (y + height - trackWidth * 2.0f, y + height * 0.5f),
                             trackWidth * 2.0f, pointerColour, trackColour, 4);
            }
            else
            {
                drawPointer (g, jmax (0.0f, x + width * 0.5f - trackWidth * 2.0f),
                             minSliderPos - trackWidth,
                             trackWidth * 2.0f, pointerColour, trackColour, 1);

                drawPointer (g, jmin (x + width - trackWidth * 2.0f, x + width * 0.5f), maxSliderPos - sr,
                             trackWidth * 2.0f, pointerColour, trackColour, 3);
            }
        }
    }
}

void PlumeLookAndFeel::drawPointer (Graphics& g, const float x, const float y, const float diameter,
                                  const Colour& colour1, const Colour& colour2, const int direction) noexcept
{
    Path p;
    p.startNewSubPath (x + diameter * 0.5f, y);
    p.lineTo (x + diameter * 0.5f, y + diameter * 0.9f);
    p.quadraticTo (x + diameter * 1.4f, y + diameter * 0.9f, x + diameter * 1.4f, y);
    p.quadraticTo (x + diameter * 1.4f, y - diameter * 0.9f, x + diameter * 0.5f, y - diameter * 0.9f);
    p.closeSubPath();

    p.applyTransform (AffineTransform::rotation (direction * MathConstants<float>::halfPi,
                                                 x + diameter * 0.5f, y + diameter * 0.5f));
    g.setColour (colour1);
    g.fillPath (p);
    // Added a outline
    g.setColour (colour2);
    g.strokePath (p, PathStrokeType (2.0f));
}


//==============================================================================================================================
// TwoRangeTuner:

void TwoRangeTunerLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos,
                                       float minSliderPos,
                                       float maxSliderPos,
                                       const Slider::SliderStyle style, Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
                                          : Rectangle<float> (x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
    }
    else
    {
        auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = jmin (6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);

        Point<float> startPoint (slider.isHorizontal() ? x : x + width * 0.5f,
                                 slider.isHorizontal() ? y + height * 0.5f : height + y);

        Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                               slider.isHorizontal() ? startPoint.y : y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, {  2.0f //Old: trackWidth
									      , PathStrokeType::curved, PathStrokeType::rounded }); // changed track width

        Path valueTrack;
        Point<float> minPoint, maxPoint, thumbPoint;

        if (isTwoVal || isThreeVal)
        {
            minPoint = { slider.isHorizontal() ? minSliderPos : width * 0.5f,
                         slider.isHorizontal() ? height * 0.5f : minSliderPos };

            if (isThreeVal)
                thumbPoint = { slider.isHorizontal() ? sliderPos : width * 0.5f,
                               slider.isHorizontal() ? height * 0.5f : sliderPos };

            maxPoint = { slider.isHorizontal() ? maxSliderPos : width * 0.5f,
                         slider.isHorizontal() ? height * 0.5f : maxSliderPos };
        }
        else
        {
            auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
            auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;

            minPoint = startPoint;
            maxPoint = { kx, ky };
        }

        auto thumbWidth = getSliderThumbRadius (slider);

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        
        PathStrokeType (3.0f).createStrokedPath (valueTrack, valueTrack);
		

		g.fillPath(valueTrack);
		g.setColour(slider.findColour(Slider::backgroundColourId));
		g.strokePath(valueTrack, { 2.0f //Old: trackWidth
								   , PathStrokeType::curved, PathStrokeType::rounded });
        

        if (! isTwoVal)
        {
            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillEllipse (Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre (isThreeVal ? thumbPoint : maxPoint));
        }

        if (isTwoVal || isThreeVal)
        {
            auto sr = jmin (trackWidth, (slider.isHorizontal() ? height : width) * 0.4f);
            auto pointerColour = slider.findColour (Slider::thumbColourId);
            auto trackColour = slider.findColour (Slider::backgroundColourId);
            
            // Only this following part changes from the PlumeLookAndFeel version of the method.
            // Here, checks if the slider is a left or right range and changes the pointer accordlingly.
            if (slider.isHorizontal())
            {
                if (leftSlider)
                {
                    drawPointer (g, minSliderPos - sr,
                                jmax (0.0f, y + height * 0.5f - trackWidth * 2.0f),
                                trackWidth * 2.0f, pointerColour, trackColour, 2);
                    
                    // Smaller, facing the wrong way and background colour
                    drawPointer (g, maxSliderPos - trackWidth * 0.5f,
                                jmax (0.0f, y + height * 0.5f - trackWidth * 1.0f),
                                trackWidth * 1.0f, trackColour, trackColour, 2);
                }
                else
                {
                    drawPointer (g, maxSliderPos - trackWidth,
                                jmin (y + height - trackWidth * 2.0f, y + height * 0.5f),
                                trackWidth * 2.0f, pointerColour, trackColour, 4);
                                
                    // Smaller, facing the wrong way and background colour
                    drawPointer (g, minSliderPos - sr * 0.5f,
                                jmin (y + height - trackWidth * 1.0f, y + height * 0.5f),
                                trackWidth * 1.0f, trackColour, trackColour, 4);
                }
            }
            else
            {
                drawPointer (g, jmax (0.0f, x + width * 0.5f - trackWidth * 2.0f),
                             minSliderPos - trackWidth,
                             trackWidth * 2.0f, pointerColour, trackColour, 1);

                drawPointer (g, jmin (x + width - trackWidth * 2.0f, x + width * 0.5f), maxSliderPos - sr,
                             trackWidth * 2.0f, pointerColour, trackColour, 3);
            }
        }
    }
}