/*
  ==============================================================================

    PlumeLookAndFeel.cpp
    Created: 29 Oct 2018 11:34:21am
    Author:  Alex

  ==============================================================================
*/

#include "PlumeLookAndFeel.h"

using namespace PLUME::UI;

PlumeLookAndFeel::PlumeLookAndFeel()
{                   
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
	setColour (Label::backgroundColourId, Colour (0x00000000));
	setColour (Label::outlineColourId, Colour (0x00000000));

    // Text 
    setColour (TextEditor::outlineColourId, Colour (0x00000000));
    setColour (TextEditor::backgroundColourId, Colour (0x00000000));
    setColour (TextEditor::focusedOutlineColourId, Colour (0x10a0a0a0));
    setColour (TextEditor::highlightColourId, Colour (0x50ffffff));
    setColour (CaretComponent::caretColourId, Colour (0xffffffff));

	// Slider
	setColour (Slider::thumbColourId, getPlumeColour (tunerSliderThumb));
	setColour (Slider::trackColourId, getPlumeColour (tunerSliderBackground));
	setColour (Slider::backgroundColourId, Colour (0xff101010));

	// TextButton
    setColour (TextButton::buttonColourId, getPlumeColour (tunerButtonFill));

	// ComboBox
	setColour (ComboBox::backgroundColourId, Colour (0x00000000));
	setColour (ComboBox::textColourId, Colour (0xff000000));
	setColour (ComboBox::arrowColourId, Colour (0xff000000));
	setColour (ComboBox::outlineColourId, Colour (0x00000000)); // Necessary to not draw Label outlines due to an Error in the JUCE code..
	
	// PopupMenu
	setColour (PopupMenu::backgroundColourId, getPlumeColour (popupMenuBackground));
	setColour (PopupMenu::textColourId, getPlumeColour (popupMenuText));
	setColour (PopupMenu::highlightedBackgroundColourId, getPlumeColour (popupMenuSelectedBackground));
	setColour (PopupMenu::highlightedTextColourId, getPlumeColour (popupMenuSelectedText));
}
/*
Font PlumeLookAndFeel::getLabelFont (Label& lbl)
{
    return PLUME::font::plumeFont;
}
*/
Font PlumeLookAndFeel::getTextButtonFont (TextButton&, int buttonHeight)
{
    return PLUME::font::plumeFont.withHeight ((buttonHeight * 6.0f) / 10.0f);
}

Font PlumeLookAndFeel::getComboBoxFont (ComboBox& cb)
{
	return PLUME::font::plumeFont.withHeight (jmax (11.0f, jmin (14.0f, (cb.getHeight() * 6.0f) / 10.0f)));
}

int PlumeLookAndFeel::getDefaultScrollbarWidth()
{
    return 8;
}

void PlumeLookAndFeel::drawScrollbar (Graphics& g, ScrollBar& scrollbar,
                                      int x, int y,
                                      int width, int height,
                                      bool isScrollbarVertical,
                                      int thumbStartPosition, int thumbSize,
                                      bool isMouseOver, bool isMouseDown)
{
    ignoreUnused (isMouseDown);

    juce::Rectangle<int> thumbBounds;

    if (isScrollbarVertical)
        thumbBounds = { x + int (std::ceil (width/2.0f)), thumbStartPosition,
                        int (std::floor (width/2.0f)),     thumbSize };
    else
        thumbBounds = { thumbStartPosition, y, thumbSize, height };

    auto c = scrollbar.findColour (ScrollBar::ColourIds::thumbColourId);
    g.setColour (isMouseOver ? c.brighter (0.25f) : c);
    g.fillRect (thumbBounds.reduced (1).toFloat());
}

void PlumeLookAndFeel::drawPopupMenuBackground (Graphics& g, int width, int height)
{
    g.fillAll (findColour (PopupMenu::backgroundColourId));
    g.setColour (getPlumeColour (popupMenuOutline));
    g.drawRect (0, 0, width, height);
}

int PlumeLookAndFeel::getPopupMenuBorderSize() { return 0; }

void PlumeLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                         float sliderPos,
                                         float minSliderPos,
                                         float maxSliderPos,
                                         const Slider::SliderStyle style, Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? juce::Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
                                          : juce::Rectangle<float> (x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
    }
    else
    {
        auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = jmin (6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);

        juce::Point<float> startPoint (slider.isHorizontal() ? x : x + width * 0.5f,
                                 slider.isHorizontal() ? y + height * 0.5f : height + y);

        juce::Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                               slider.isHorizontal() ? startPoint.y : y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, {  2.0f /*trackWidth*/, PathStrokeType::curved, PathStrokeType::rounded }); // changed track width

        Path valueTrack;
        juce::Point<float> minPoint, maxPoint, thumbPoint;

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
            g.fillEllipse (juce::Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre (isThreeVal ? thumbPoint : maxPoint));
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
// OneRangeTunerLookAndFeel:

OneRangeTunerLookAndFeel::OneRangeTunerLookAndFeel()
{
    setColour (Slider::thumbColourId, Colour (0xffffffff));
    
    // Label
    setColour (Label::textColourId, Colour (0xff7c80de));
    setColour (Label::backgroundColourId, Colour (0x00000000));
    setColour (Label::textWhenEditingColourId , Colour (0xff6060de));

    // Text 
    setColour (TextEditor::textColourId, Colour (0xff6060de));
    setColour (TextEditor::focusedOutlineColourId, Colour (0x00000000));
    setColour (TextEditor::highlightColourId, Colour (0x507c80de));
    setColour (TextEditor::highlightedTextColourId, Colour (0xff6060de));
    setColour (CaretComponent::caretColourId, Colour (0xff7c80de));
}

void OneRangeTunerLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (1.5f /*lineW*/, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (1.5f /*lineW*/, PathStrokeType::curved, PathStrokeType::rounded));
    }

    auto thumbWidth = 6.0f;
    juce::Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                             bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));

    g.setColour (slider.findColour (Slider::thumbColourId));
    g.fillEllipse (juce::Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
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
        g.fillRect (slider.isHorizontal() ? juce::Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
                                          : juce::Rectangle<float> (x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
    }
    else
    {
        auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        auto trackWidth = jmin (6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);

        juce::Point<float> startPoint (slider.isHorizontal() ? x : x + width * 0.5f,
                                 slider.isHorizontal() ? y + height * 0.5f : height + y);

        juce::Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                               slider.isHorizontal() ? startPoint.y : y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, {  2.0f //Old: trackWidth
									      , PathStrokeType::curved, PathStrokeType::rounded }); // changed track width

        Path valueTrack;
        juce::Point<float> minPoint, maxPoint, thumbPoint;

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
            g.fillEllipse (juce::Rectangle<float> (static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre (isThreeVal ? thumbPoint : maxPoint));
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

//==============================================================================================================================
// TestTuner:

TestTunerLookAndFeel::TestTunerLookAndFeel()
{
    setColour (Slider::thumbColourId, Colour (0xffffffff));
    
    // Label
    setColour (Label::textColourId, Colour (0xff7c80de));
    setColour (Label::backgroundColourId, Colour (0x00000000));
    setColour (Label::textWhenEditingColourId , Colour (0xff6060de));

    // Text 
    setColour (TextEditor::textColourId, Colour (0xff6060de));
    setColour (TextEditor::focusedOutlineColourId, Colour (0x00000000));
    setColour (TextEditor::highlightColourId, Colour (0x507c80de));
    setColour (TextEditor::highlightedTextColourId, Colour (0xff6060de));
    setColour (CaretComponent::caretColourId, Colour (0xff7c80de));
}

void TestTunerLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (12.0f, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));
    }

    auto thumbWidth = 6.0f;
    juce::Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                             bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));
	if (slider.getThumbBeingDragged() != -1)
	{
		g.setColour (fill.withAlpha(0.6f));
		g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
	}

    g.setColour (slider.findColour (Slider::thumbColourId));
    g.fillEllipse (juce::Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
}

void TestTunerLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                             float sliderPos,
                                             float minSliderPos,
                                             float maxSliderPos,
                                             const Slider::SliderStyle style, Slider& slider)
{
	using juce::Rectangle;

    if (slider.isBar())
    {
        g.setColour (slider.findColour (Slider::trackColourId));
        g.fillRect (slider.isHorizontal() ? juce::Rectangle<float> (static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
                                          : juce::Rectangle<float> (x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
    }
    else
    {
        auto isTwoVal   = (style == Slider::SliderStyle::TwoValueVertical   || style == Slider::SliderStyle::TwoValueHorizontal);
        auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

        juce::Point<float> startPoint (slider.isHorizontal() ? x : x + width * 0.5f,
                                 slider.isHorizontal() ? y + height * 0.5f : height + y);

        juce::Point<float> endPoint (slider.isHorizontal() ? width + x : startPoint.x,
                               slider.isHorizontal() ? startPoint.y : y);

        Path backgroundTrack;
        backgroundTrack.startNewSubPath (startPoint);
        backgroundTrack.lineTo (endPoint);
        g.setColour (slider.findColour (Slider::backgroundColourId));
        g.strokePath (backgroundTrack, {  12.0f, PathStrokeType::curved, PathStrokeType::rounded }); // changed track width

        Path valueTrack;
        juce::Point<float> minPoint, maxPoint, thumbPoint;

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

        auto thumbWidth = 6.0f;

        valueTrack.startNewSubPath (minPoint);
        valueTrack.lineTo (isThreeVal ? thumbPoint : maxPoint);
        g.setColour (slider.findColour (Slider::trackColourId));
        g.strokePath(valueTrack, { 6.0f //Old: trackWidth
                                   , PathStrokeType::curved, PathStrokeType::rounded });
        

        if (! isTwoVal)
        {
			if (slider.getThumbBeingDragged() != -1 &&
                slider.findColour (Slider::trackColourId) != Colour (0x00000000))
			{
				g.setColour(slider.findColour (Slider::trackColourId).withAlpha (0.6f));
				g.fillEllipse(juce::Rectangle<float> (25.0f, 25.0f).withCentre (maxPoint));
			}

            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillEllipse (juce::Rectangle<float> (static_cast<float> (thumbWidth),
                                                   static_cast<float> (thumbWidth)).withCentre (maxPoint));
        }

        if (isTwoVal || isThreeVal)
        {
            if (slider.getThumbBeingDragged() != -1 &&
                slider.findColour (Slider::trackColourId) != Colour (0x00000000))
            {
                g.setColour (slider.findColour (Slider::trackColourId).withAlpha (0.6f));
                g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f)
                                  .withCentre (slider.getThumbBeingDragged() == 1 ? minPoint
                                                                                  : maxPoint));
            }

            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillEllipse (juce::Rectangle<float> (static_cast<float> (thumbWidth),
                                                   static_cast<float> (thumbWidth)).withCentre (maxPoint));
            g.fillEllipse (juce::Rectangle<float> (static_cast<float> (thumbWidth),
                                                   static_cast<float> (thumbWidth)).withCentre (minPoint));
        }
    }
}
