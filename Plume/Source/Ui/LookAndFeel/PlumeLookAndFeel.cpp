/*
  ==============================================================================

    PlumeLookAndFeel.cpp
    Created: 29 Oct 2018 11:34:21am
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "PlumeLookAndFeel.h"

PlumeLookAndFeel::PlumeLookAndFeel()
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
	setColour (Slider::thumbColourId, Colour (plumePalette[gestureActiveBackground]));
	setColour (Slider::trackColourId, Colour (plumePalette[gestureActiveBackground]));
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
	setColour (PopupMenu::highlightedBackgroundColourId, Colour (0xffe5e5e5));
	setColour (PopupMenu::highlightedTextColourId, Colour (0xff323232));
	
}

PlumeLookAndFeel::~PlumeLookAndFeel()
{
}

Colour PlumeLookAndFeel::getPlumeColour (int colourId)
{
    if (colourId > numColours)
	{
	    return plumePalette[1];
	}
	    
	return plumePalette[colourId];
}

void PlumeLookAndFeel::setPlumeColour (int colourId, Colour colourValue)
{
    if (colourId > numColours) return;
        
    plumePalette[colourId] = colourValue;
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