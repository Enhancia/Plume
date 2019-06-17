/*
  ==============================================================================

    RetractableMapAndMidiPanel.cpp
    Created: 13 Jun 2019 10:39:01am
    Author:  Alex

  ==============================================================================
*/

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "RetractableMapAndMidiPanel.h"

//==============================================================================
RetractableMapAndMidiPanel::RetractableMapAndMidiPanel (Gesture& gest, GestureArray& gestArr, PluginWrapper& wrap)
    : gesture (gest), gestureArray (gestArr), wrapper (wrap)
{
	addAndMakeVisible (parametersBanner = new MapperBanner (gesture, gestureArray, wrapper));
	addAndMakeVisible (parametersBody = new MapperComponent (gesture, gestureArray, wrapper));
	addAndMakeVisible (midiBanner = new MidiBanner());
	addAndMakeVisible (midiBody = new MidiModeComponent (gesture));

	parametersRetractable.setComponents (parametersBanner, parametersBody);
	midiRetractable.setComponents (midiBanner, midiBody);

	addAndMakeVisible (hideBodyButton = new ShapeButton ("Hide Body Button",
                                                         Colour (0x00000000),
                                                         Colour (0x00000000),
                                                         Colour (0x00000000)));
	initializeHideBodyButton();

	setPanelMode (parameterMode);
    
    gesture.removeAllChangeListeners(); // In case the gesture had a previous listener
    gesture.addChangeListener (this);
    //gestureArray.addChangeListener (this);
    wrapper.addChangeListener (this);
}

RetractableMapAndMidiPanel::~RetractableMapAndMidiPanel()
{
    //gestureArray.removeChangeListener (this);
    wrapper.removeChangeListener (this);
}

const String RetractableMapAndMidiPanel::getInfoString()
{
	if (panelMode == parameterMode)
	{
		return String ("Parameters Panel:\n\n - Map plugin parameters to the gesture using the \"Map\" button.\n") +
			   String (!wrapper.isWrapping() ? "- You must have a wrapped plugin to use this feature!" : "");
	}
	else
	{
		return String ("MIDI Panel:\n\n - Select the type of MIDI to send to ") +
			   String (wrapper.isWrapping() ? wrapper.getWrappedPluginName() : "the plugin") +
			   + ".\n";
	}
}

void RetractableMapAndMidiPanel::update()
{
	if (panelMode == parameterMode)
	{
		parametersBody->updateComponents();
	}
	else if (panelMode == midiMode)
	{
		midiBody->updateComponents();
	}
}

void RetractableMapAndMidiPanel::updateDisplay()
{
	if (panelMode == parameterMode)
	{
		parametersBody->updateDisplay();
	}
}

void RetractableMapAndMidiPanel::paint (Graphics& g)
{
	using namespace PLUME::UI;

	//Gradient for horizontal lines
    auto grad = ColourGradient::horizontal (Colour (0x10323232),
                                            float(MARGIN), 
                                            Colour (0x10323232),
                                            float(getWidth() - MARGIN));
    grad.addColour (0.5, Colour (0x50323232));

    auto area = getLocalBounds();
    area.removeFromTop (bannerHeight);

    g.setGradientFill (grad);
    g.drawHorizontalLine (area.getY(),
                          float(area.getX() + 2*MARGIN), float(area.getWidth() - 2*MARGIN));
}

void RetractableMapAndMidiPanel::resized()
{
	resized (parametersRetractable);
	resized (midiRetractable);

	auto bannerArea = getLocalBounds().removeFromTop (bannerHeight);
	hideBodyButton->setBounds (bannerArea.removeFromBottom (bannerHeight/2)
								         .removeFromRight (getWidth()/4)
								         .reduced (3));
	createHideBodyButtonPath();
}

void RetractableMapAndMidiPanel::resized (Retractable& retractableToResize)
{
	auto area = getLocalBounds();

	retractableToResize.banner->setBounds (area.removeFromTop (bannerHeight));
	if (!retracted) retractableToResize.body->setBounds (area.reduced (PLUME::UI::MARGIN));
}

void RetractableMapAndMidiPanel::buttonClicked (Button* bttn)
{
	if (bttn == hideBodyButton)
	{
		setRetracted (hideBodyButton->getToggleState());
	}
}

void RetractableMapAndMidiPanel::changeListenerCallback(ChangeBroadcaster* source)
{
	/* [OUTDATED.. You can only map one gesture at the time with the new interface]
    // if Another gesture wants to be mapped
    // Draws the map button in non-map colour
    if (source == &gestureArray && gestureArray.mapModeOn && gesture.mapModeOn == false)
    {
        mapButton->setColour (TextButton::buttonColourId, getLookAndFeel().findColour (TextButton::buttonColourId));
        return;
    }
    */

    // If gesture mapping changed
    // Recreates the array of parameterComponent, and redraws the mapperComponent
    if (source == &gesture)
    {
        parametersBody->initializeParamCompArray();
        parametersBody->resized();
    }
    
    // If the editor is closed with map mode still on
    // Cancels map mode and colors map button to non-map
    else if (source == &wrapper && gesture.mapModeOn)
    {
        gestureArray.cancelMapMode();
    }

    parametersBanner->updateComponents();
}

void RetractableMapAndMidiPanel::setPanelMode (PanelMode newMode)
{
	if (newMode == parameterMode)
	{
		midiRetractable.banner->setVisible (false);
		midiRetractable.body->setVisible (false);

		parametersBody->updateComponents();
		parametersRetractable.banner->setVisible (true);
		if (!retracted) parametersRetractable.body->setVisible (true);
	}
	else if (newMode == midiMode)
	{
		parametersRetractable.banner->setVisible (false);
		parametersRetractable.body->setVisible (false);

		midiBody->updateComponents();
		midiRetractable.banner->setVisible (true);
		if (!retracted) midiRetractable.body->setVisible (true);
	}
}

void RetractableMapAndMidiPanel::setRetracted (bool shouldBeRetracted)
{
	if (retracted != shouldBeRetracted)
	{
		retracted = shouldBeRetracted;
		setRetracted (parametersRetractable);
		setRetracted (midiRetractable);

		resized();
		if (auto* parentComponent = getParentComponent())
			parentComponent->repaint();
	}
}

void RetractableMapAndMidiPanel::setRetracted (Retractable& retractableToResize)
{
	if (retractableToResize.banner->isVisible())
	{
		retractableToResize.body->setVisible (!retracted);
	}
}

void RetractableMapAndMidiPanel::initializeParamCompArray()
{
	parametersBody->initializeParamCompArray();
}

bool RetractableMapAndMidiPanel::isRetracted()
{
	return retracted;
}

void RetractableMapAndMidiPanel::initializeHideBodyButton()
{
    hideBodyButton->setOutline (Colour (0xff000000), 1.5f);
    hideBodyButton->setToggleState (retracted, dontSendNotification);
    hideBodyButton->setClickingTogglesState (true);
    createHideBodyButtonPath();
    hideBodyButton->addMouseListener (this, false);
    hideBodyButton->addListener (this);
}

void RetractableMapAndMidiPanel::createHideBodyButtonPath()
{
    using namespace PLUME::UI;
    Path p;
    
    if (hideBodyButton->getToggleState())
    {
        p.startNewSubPath (0,0);
        p.lineTo (1, 2);
        p.lineTo (2, 0);
    }
    else
    {
        p.startNewSubPath (0, 2);
        p.lineTo (1, 0);
        p.lineTo (2, 2);
    }
    
    hideBodyButton->setShape (p, false, true, false);
}

//==============================================================================
// Retractable

void RetractableMapAndMidiPanel::Retractable::setComponents (Component* bannerComponent, Component* bodyComponent)
{
	// You're adding nullptrs for either the banner or body....
	jassert (bannerComponent != nullptr && bodyComponent != nullptr);

	banner = bannerComponent;
	body = bodyComponent;
}