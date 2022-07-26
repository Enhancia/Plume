/*
  ==============================================================================

    DualTextToggleComponent.cpp
    Created: 9 Apr 2019 2:40:13pm
    Author:  Alex

  ==============================================================================
*/

#include "DualTextToggle.h"

//==============================================================================
DualTextToggle::DualTextToggle (String falseStateText, String trueStateText, DualToggleStyle initialStyle)
	: stateFalseText (falseStateText), stateTrueText (trueStateText), style (initialStyle)
{
}

DualTextToggle::DualTextToggle (String falseStateText, String trueStateText,
                    		    Colour falseStateColour, Colour trueStateColour,
                    		    DualToggleStyle initialStyle)
	: stateFalseText (falseStateText), stateTrueText (trueStateText),
	  stateFalseColour (falseStateColour), stateTrueColour (trueStateColour),
	  style (initialStyle)
{
}

DualTextToggle::~DualTextToggle()
{
}

void DualTextToggle::paint (Graphics& g)
{
	switch (style)
	{
		case oneStateVisible:   paintOneStateVisibleToggle (g);    break;
		case twoStatesVisible:  paintTwoStateVisibleToggle (g);    break;
		case toggle:            paintToggle (g, getLocalBounds()); break;
		case toggleWithTopText: paintToggleWithTopText (g);        break;
		default: break;
	}
}

void DualTextToggle::paintOneStateVisibleToggle (Graphics& g)
{
	paintStateInAreaWithAlpha (g, state, getLocalBounds());
}

void DualTextToggle::paintTwoStateVisibleToggle (Graphics& g)
{
	auto area = getLocalBounds();

	paintStateInAreaWithAlpha (g, false, area.removeFromLeft (getWidth()/2), state == false ? 1.0f : 0.2f);
	paintStateInAreaWithAlpha (g, true, area, state == true ? 1.0f : 0.2f);
}

void DualTextToggle::paintStateInAreaWithAlpha (Graphics& g, bool stateToPaint,
                                                             juce::Rectangle <int> areaToPaint,
															 float alpha)
{
	if (alpha < 0.0f || alpha > 1.0f)
	{
		// The alpha value should be between 0 and 1 !
		jassertfalse;
		alpha = jmin (jmax (alpha, 0.0f), 1.0f);
	}

	g.setColour (stateToPaint ? stateTrueColour.withAlpha (stateTrueColour.getFloatAlpha() * alpha)
					          : stateFalseColour.withAlpha (stateFalseColour.getFloatAlpha() * alpha));
	g.fillRect (areaToPaint.reduced (1));

	g.setColour (Colours::black.withAlpha (alpha));
	g.setFont (PLUME::font::plumeFont.withHeight (getHeight() * 6.0f/10));
	g.drawFittedText (stateToPaint ? stateTrueText : stateFalseText,
		              areaToPaint.reduced (areaToPaint.getWidth()/4, areaToPaint.getHeight()/4),
		              Justification::centred, 1);
}

void DualTextToggle::paintToggle (Graphics& g, juce::Rectangle<int> areaToPaint)
{
	Path togglePath, thumbPath;
	togglePath.setUsingNonZeroWinding (false);

	int toggleHeight = (areaToPaint.getHeight() > areaToPaint.getWidth()*2/3)
	 						? areaToPaint.getWidth()*2/3
	 						: areaToPaint.getHeight();

	togglePath.addRoundedRectangle (areaToPaint.withSizeKeepingCentre (areaToPaint.getWidth(), toggleHeight)
											   .toFloat(),
									toggleHeight/2.0f);

	thumbPath.addEllipse (areaToPaint.withSizeKeepingCentre (toggleHeight, toggleHeight)
									 .withX (state ? areaToPaint.getWidth() - toggleHeight
									   			   : 0)
									 .reduced (1)
							     	 .toFloat());
	togglePath.addPath (thumbPath);

	g.setColour (state ? stateTrueColour : stateFalseColour);
	g.fillPath (togglePath);

	g.setColour (thumbColour);
	g.fillPath (thumbPath);
}

void DualTextToggle::paintToggleWithTopText (Graphics& g)
{
	paintToggle (g, getLocalBounds().withTop (getHeight()/2));

	g.setColour (textColour != Colour (0) ? textColour
										  : state ? stateTrueColour
										 		  : stateFalseColour);

	g.setFont (PLUME::font::plumeFontBold.withHeight (getHeight()/2.0f - 1.0f));
	g.drawText (state ? stateTrueText : stateFalseText,
				getLocalBounds().withBottom (getHeight()/2),
				Justification::centred);
}

void DualTextToggle::resized()
{
}

void DualTextToggle::mouseUp (const MouseEvent &)
{
	switchToggleState();
}

void DualTextToggle::setStyle (DualToggleStyle styleToSet)
{
	if (style != styleToSet)
	{
		style = styleToSet;
		repaint();
	}
}

void DualTextToggle::setToggleState (bool stateToSet)
{
	if (state != stateToSet)
	{
		switchToggleState();
	}
}

bool DualTextToggle::getToggleState()
{
	return state;
}

void DualTextToggle::switchToggleState()
{
	state = !state;
	if (onStateChange != nullptr) onStateChange();
	repaint();
}

void DualTextToggle::setFalseStatetext (String textToSet)
{
	stateFalseText = textToSet;
	repaint();
}

void DualTextToggle::setTrueStatetext (String textToSet)
{
	stateTrueText = textToSet;
	repaint();
}

void DualTextToggle::setFalseStateColour (const Colour coulourToSet)
{
	stateFalseColour = coulourToSet;
	repaint();
}

void DualTextToggle::setTrueStateColour (const Colour coulourToSet)
{
	stateTrueColour = coulourToSet;
	repaint();
}

void DualTextToggle::setStateIndependentTextColour (const Colour textColourToSet)
{
	textColour = textColourToSet;
	repaint();
} 

void DualTextToggle::setToggleThumbColour (const Colour thumbColourToSet)
{
	thumbColour = thumbColourToSet;
	repaint();
}

AudioParameterBoolToggle::AudioParameterBoolToggle (AudioParameterBool& p) : param (p)
{
}

AudioParameterBoolToggle::~AudioParameterBoolToggle()
{
}