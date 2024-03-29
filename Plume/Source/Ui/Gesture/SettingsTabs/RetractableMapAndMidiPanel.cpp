/*
  ==============================================================================

    RetractableMapAndMidiPanel.cpp
    Created: 13 Jun 2019 10:39:01am
    Author:  Alex

  ==============================================================================
*/

#include "RetractableMapAndMidiPanel.h"

//==============================================================================
RetractableMapAndMidiPanel::RetractableMapAndMidiPanel (Gesture& gest, GestureArray& gestArr,
														PluginWrapper& wrap)
    : gesture (gest), gestureArray (gestArr), wrapper (wrap)
{
	addAndMakeVisible (*(parametersBanner = std::make_unique<MapperBanner> (gesture, gestureArray, wrapper)));
	addAndMakeVisible (*(parametersBody = std::make_unique<MapperComponent> (gesture, gestureArray, wrapper)));
	addAndMakeVisible (*(midiBanner = std::make_unique<MidiBanner> (gesture)));
	addAndMakeVisible (*(midiBody = std::make_unique<MidiModeComponent> (gesture, gestureArray)));

	parametersRetractable.setComponents (parametersBanner.get(), parametersBody.get());
	midiRetractable.setComponents (midiBanner.get(), midiBody.get());

	addAndMakeVisible (*(hideBodyButton = std::make_unique<ShapeButton> ("Hide Body Button",
                                                         Colour (0x00000000),
                                                         Colour (0x00000000),
                                                         Colour (0x00000000))));
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
    const String bullet = " " + String::charToString (juce_wchar(0x2022));
    
	if (panelMode == parameterMode)
	{
		return String ("Parameters Panel:\n\n" + bullet + " Map plugin parameters to the gesture using the \"Map\" button.\n") +
			   String (!wrapper.isWrapping() ? (bullet + " You must have a wrapped plugin to use this feature!") : "");
	}
	else
	{
		return String ("MIDI Panel:\n\n" + bullet + " Select the type of MIDI to send to ") +
			   String (wrapper.isWrapping() ? wrapper.getWrappedPluginName() : "the plugin") +
			   + ".\n";
	}
}

void RetractableMapAndMidiPanel::update()
{
	if (panelMode == parameterMode)
	{
		parametersBanner->updateComponents();
		parametersBody->updateComponents();
	}
	else if (panelMode == midiMode)
	{
		midiBanner->repaint();
		midiBody->updateComponents();
	}
}

void RetractableMapAndMidiPanel::updateDisplay()
{
	if (panelMode == parameterMode)
	{
		parametersBody->updateDisplay();
	}
	else if (panelMode == midiMode)
	{
		midiBody->updateDisplay();
	}
}

void RetractableMapAndMidiPanel::updateMidiRange (MidiRangeTuner::DraggableObject thumbToUpdate)
{
	midiBody->getTuner().updateComponents (thumbToUpdate);
}	

void RetractableMapAndMidiPanel::paint (Graphics&)
{
}

void RetractableMapAndMidiPanel::resized()
{
	resized (parametersRetractable);
	resized (midiRetractable);
}

void RetractableMapAndMidiPanel::resized (Retractable& retractableToResize)
{
	auto area = getLocalBounds();

	retractableToResize.banner->setBounds (area.removeFromTop (bannerHeight));
	if (!retracted) retractableToResize.body->setBounds (area);
}

void RetractableMapAndMidiPanel::buttonClicked (Button* bttn)
{
	if (bttn == hideBodyButton.get())
	{
		setRetracted (hideBodyButton->getToggleState());
	}
}

void RetractableMapAndMidiPanel::changeListenerCallback(ChangeBroadcaster* source)
{
    // If gesture mapping changed
    // Recreates the array of parameterComponent, and redraws the mapperComponent
    if (source == &gesture)
    {
        parametersBody->updateParamCompArray();
        parametersBody->resized();

        if (!gestureArray.mapModeOn) // Wrapper cancelled mapMode after adding parameter
        {
        	wrapper.clearWrapperEditor();
        }

        getParentComponent()->getParentComponent()->repaint();
        gestureArray.getOwnerProcessor()
        			.updateHostDisplay (AudioProcessor::ChangeDetails().withParameterInfoChanged (true));
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
	panelMode = newMode;
	
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
		if (auto* parentComp = getParentComponent())
		{
			parentComp->resized();
			parentComp->repaint();
		}
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
        p.startNewSubPath (0, 2);
        p.lineTo (1, 0);
        p.lineTo (2, 2);
    }
    else
    {
        p.startNewSubPath (0,0);
        p.lineTo (1, 2);
        p.lineTo (2, 0);
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
