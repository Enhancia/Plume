/*
  ==============================================================================

    DualTextToggleComponent.cpp
    Created: 9 Apr 2019 2:40:13pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "DualTextToggle.h"

//==============================================================================
DualTextToggle::DualTextToggle (String falseStateText, String trueStateText)
	: stateFalseText (falseStateText), stateTrueText (trueStateText)
{
}

DualTextToggle::DualTextToggle (String falseStateText, String trueStateText,
                    		    Colour falseStateColour, Colour trueStateColour)
	: stateFalseText (falseStateText), stateTrueText (trueStateText),
	  stateFalseColour (falseStateColour), stateTrueColour (trueStateColour)
{
}

DualTextToggle::~DualTextToggle()
{
}

void DualTextToggle::paint (Graphics& g)
{
	if      (style == oneStateVisible) paintOneStateVisibleToggle (g);
	else if (style == twoStatesVisible) paintTwoStateVisibleToggle (g);
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
	g.drawText (stateToPaint ? stateTrueText : stateFalseText, areaToPaint, Justification::centred, true);
}

void DualTextToggle::resized()
{
}

void DualTextToggle::mouseUp (const MouseEvent &event)
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

void DualTextToggle::setFalseStateColour (Colour coulourToSet)
{
	stateFalseColour = coulourToSet;
	repaint();
}

void DualTextToggle::setTrueStateColour (Colour coulourToSet)
{
	stateTrueColour = coulourToSet;
	repaint();
}


AudioParameterBoolToggle::AudioParameterBoolToggle (AudioParameterBool& p) : param (p)
{
}

AudioParameterBoolToggle::~AudioParameterBoolToggle()
{
}