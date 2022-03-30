/*
  ==============================================================================

    HeaderComponent.cpp
    Created: 12 Dec 2018 4:53:18pm
    Author:  Alex

  ==============================================================================
*/

#include "HeaderComponent.h"

//==============================================================================
HeaderComponent::HeaderComponent (PlumeProcessor& proc, Component& newPrst)  : processor (proc),
                                                                               newPresetPanel (newPrst)
{
	Component::setName ("Header");
    setComponentID ("header");
    
    // Plugin Name
    pluginNameLabel.reset (new Label ("Plugin Name Label", processor.getWrapper().getWrappedPluginName()));
    addAndMakeVisible (*pluginNameLabel);
    pluginNameLabel->setFont (PLUME::font::plumeFont.withHeight (PLUME::font::HEADER_LABEL_FONT_H));
    pluginNameLabel->setJustificationType (Justification::centred);
    pluginNameLabel->setEditable (false, false, false);
    pluginNameLabel->setColour (Label::backgroundColourId, Colour (0));
    pluginNameLabel->setColour (Label::textColourId, getPlumeColour (headerText));
    pluginNameLabel->addMouseListener (this, false);
    
    // Preset Name
    presetNameLabel.reset (new Label ("Preset Name Label", processor.getPresetHandler().getCurrentPresetName()));
    addAndMakeVisible (*presetNameLabel);
    presetNameLabel->setFont (PLUME::font::plumeFont.withHeight (PLUME::font::HEADER_LABEL_FONT_H));
    presetNameLabel->setJustificationType (Justification::centred);
    presetNameLabel->setEditable (false, false, false);
    presetNameLabel->setColour (Label::backgroundColourId, Colour (0));
    presetNameLabel->setColour (Label::textColourId, getPlumeColour (headerText));

    // Battery
    batteryComponent = std::make_unique<BatteryComponent> (proc.getDataReader ()->getBatteryReference (), *proc.getDataReader());
    addAndMakeVisible (*batteryComponent);
    //this->setBatteryVisible (true);

    createButtons();
    
	// Plugin List menu
    createPluginMenu (KnownPluginList::sortByManufacturer);
    
    processor.getParameterTree().addParameterListener ("track_arm", this);
}

HeaderComponent::~HeaderComponent()
{
    processor.getParameterTree().removeParameterListener ("track_arm", this);
    pluginNameLabel = nullptr;
    presetNameLabel = nullptr;
    pluginListButton = nullptr;
    savePresetButton = nullptr;
    batteryComponent = nullptr;
}

//==============================================================================
const String HeaderComponent::getInfoString()
{
    const String bullet = " " + String::charToString (static_cast<juce_wchar>(0x2022));

    return "Header :\n\n" 
            + bullet + " Click on the arrow to display all the available plugins to use.\n"
            + bullet + " Click on the plugin name to open its interface.\n";
}

void HeaderComponent::update()
{
    pluginNameLabel->setText (processor.getWrapper().getWrappedPluginName(), dontSendNotification);
    presetNameLabel->setText (processor.getPresetHandler().getCurrentPresetName(), dontSendNotification);
    createPluginMenu (KnownPluginList::sortByManufacturer);
    batteryComponent->update ();
}

void HeaderComponent::setBatteryVisible (bool shouldBeVisible)
{
    if (batteryComponent->isVisible () != shouldBeVisible)
    {
        batteryComponent->setVisible (shouldBeVisible);

        if (shouldBeVisible)
        {
            batteryComponent->startTimer (int (BatteryComponent::batteryCheckTimer), 5000);
        }
        else
        {
            batteryComponent->stopTimer (int (BatteryComponent::batteryCheckTimer));
        }
    }
}

//==============================================================================
void HeaderComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    
    // Background
    g.setColour (getPlumeColour (headerBackground));
    g.fillRoundedRectangle (getLocalBounds().reduced (MARGIN_SMALL).toFloat(), 15.0f);

    // header buttons left and right to preset area
    g.setColour (getPlumeColour (headerButtonBackground));
    g.fillRoundedRectangle (presetNameLabel->getBounds().reduced (0, 6)
                                                        .withLeft (trackArmButton->getX() - MARGIN_SMALL/2)
                                                        .withRight (savePresetButton->getRight() + MARGIN_SMALL/2)
                                                        .toFloat(),
                            10.0f);

    // Preset Name Area
    g.setColour (getPlumeColour (presetDisplayBackground));
    g.fillRoundedRectangle (presetNameLabel->getBounds().reduced (0, 6)
                                                        .withLeft (leftArrowButton->getX())
                                                        .withRight (rightArrowButton->getX() +
                                                                    rightArrowButton->getWidth())
                                                        .toFloat(),
                            0.0f);

    // Plugin Name Area
    g.saveState();
    g.reduceClipRegion (getLocalBounds().withLeft (pluginNameLabel->getX()));
    g.setColour (getPlumeColour (pluginDisplayBackground));

    g.fillRoundedRectangle (getLocalBounds().reduced (MARGIN_SMALL).toFloat(), 15.0f);
    g.restoreState();

    // Ring Path
    /*
    g.setColour (getPlumeColour (headerText).withAlpha (0.9f));
    Path ringPath = PLUME::path::createPath (PLUME::path::ring);

    g.fillPath (ringPath,
                ringPath.getTransformToScaleToFit (juce::Rectangle<float> (16.0f, 16.0f)
                                                        .withCentre ({float (trackArmButton->getBounds().getRight()) + 15.0f,
                                                                     float (trackArmButton->getBounds().getCentreY())}),
                                                   true));*/
}

void HeaderComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();

    // Plugin Area
    {
        auto pluginArea = area.removeFromRight (getWidth()/5).withTrimmedRight (MARGIN_SMALL);

        pluginListButton->setBounds (pluginArea.removeFromRight (getHeight()).reduced (MARGIN_SMALL));
        pluginNameLabel->setBounds (pluginArea.reduced (0, MARGIN_SMALL));
    }

    // Preset Area
    {
        auto presetArea = getLocalBounds().withSizeKeepingCentre (270, area.getHeight());
        
        trackArmButton->setBounds (presetArea.removeFromLeft (20).reduced (0, MARGIN));
        savePresetButton->setBounds (presetArea.removeFromRight (20).reduced (0, MARGIN));

        leftArrowButton->setBounds (presetArea.removeFromLeft (HEADER_HEIGHT - 2*MARGIN).reduced (0, MARGIN));
        rightArrowButton->setBounds (presetArea.removeFromRight (HEADER_HEIGHT - 2*MARGIN).reduced (0, MARGIN));
        presetNameLabel->setBounds (presetArea.reduced (0, MARGIN_SMALL));
    }

    // Battery Area
    {
        const auto globalBatteryArea = area.removeFromLeft (getWidth ()).withTrimmedLeft (MARGIN_SMALL);
        batteryComponent->setBounds (globalBatteryArea);
    }
}

//==============================================================================
void HeaderComponent::mouseUp (const MouseEvent &event)
{
	if (event.eventComponent == pluginNameLabel.get())
	{
		if (processor.getWrapper().isWrapping())
		{
			if (auto* editor = getParentComponent())
			{
				processor.getWrapper().createWrapperEditor(editor);
			}
        }
	}
}


void HeaderComponent::mouseEnter (const MouseEvent &event)
{
    if (event.eventComponent == pluginNameLabel.get() && processor.getWrapper().isWrapping())
	{
        pluginNameLabel->setColour (Label::textColourId,
                                    getPlumeColour (headerText).withAlpha (0.5f));
	}
}

void HeaderComponent::mouseExit (const MouseEvent &event)
{
    if (event.eventComponent == pluginNameLabel.get())
	{
        pluginNameLabel->setColour (Label::textColourId,
                                    getPlumeColour (headerText));
	}
}

void HeaderComponent::buttonClicked (Button* bttn) 
{
    if (bttn == pluginListButton.get())
    {
        if (processor.getWrapper().getList().getNumTypes() == 0 || pluginListMenu.getNumItems() == 1)
        {
            processor.sendActionMessage (PLUME::commands::scanRequired);
        }
        else
        {
            pluginListMenu.showMenuAsync (PopupMenu::Options().withParentComponent (getParentComponent())
                                                              .withMinimumWidth (100)
                                                              .withMaximumNumColumns (3)
                                                              .withPreferredPopupDirection
                                                                  (PopupMenu::Options::PopupDirection::downwards)
                                                              .withTargetScreenArea
                                                                  (juce::Rectangle<int> (getScreenX() + getWidth()-2,
                                                                                         getScreenY() + getHeight(),
                                                                                         1, 1)),
                                          ModalCallbackFunction::forComponent (pluginMenuCallback, this));
        }
    }
    else if (bttn == trackArmButton.get())
    {
        RangedAudioParameter* trackArmParameter = processor.getParameterTree().getParameter("track_arm");

        if (trackArmParameter != nullptr)
        {
            trackArmParameter->beginChangeGesture();
            trackArmParameter->setValueNotifyingHost(trackArmButton->getToggleState() ? 1.0f : 0.0f);
            trackArmParameter->endChangeGesture();
        }
    }
    else if (bttn == savePresetButton.get())
    {
        newPresetPanel.setVisible (true);
    }
    else if (bttn == leftArrowButton.get())
    {
        setPreviousPreset();
    }
    else if (bttn == rightArrowButton.get())
    {
        setNextPreset();
    }
}


void HeaderComponent::parameterChanged (const String &, float newValue)
{
    trackArmButton->setToggleState (newValue == 1.0f, dontSendNotification);
}

void HeaderComponent::pluginMenuCallback (int result, HeaderComponent* header)
{
    if (header != nullptr)
    {
		header->handlePluginChoice (result);
    }
}

void HeaderComponent::handlePluginChoice (int chosenId)
{
    if (chosenId == 0)
    {   
        // resets the button colour if no choice was made
        pluginListButton->setOutline (getPlumeColour (headerText), 2.0f);
    }
    else
    {
        if (processor.getGestureArray().mapModeOn)
        {
            // Cancels map mode in case it was on, this prevents the plugin load
            // from mapping a parameter without the user wanting.
            processor.getGestureArray().cancelMapMode();
        }

        if (processor.getWrapper().rewrapPlugin (chosenId))
        {
            pluginNameLabel->setText (processor.getWrapper().getWrappedPluginName(), dontSendNotification);

            // Creates the wrapper editor object as a child of the editor
            createPluginWindow();
        }

        pluginNameLabel->setText (processor.getWrapper().getWrappedPluginName(), dontSendNotification); // TO DELETE ?
    }
}

void HeaderComponent::createPluginMenu (const KnownPluginList::SortMethod sort)
{
	pluginListMenu.clear();
	pluginListMenu.addSectionHeader ("Plugins:");
    processor.getWrapper().addPluginsToMenu (pluginListMenu, sort);
}

void HeaderComponent::createPluginWindow()
{
    if (auto* editor = getParentComponent())
    {
        processor.getWrapper().createWrapperEditor (editor);
    }
}

void HeaderComponent::createButtons()
{
    // Plugin List Button
    Path arrowDown;
    arrowDown.startNewSubPath (0.0f, 0.0f);
    arrowDown.lineTo (3.0f, 3.0f);
    arrowDown.lineTo (6.0f, 0.0f);
    
    pluginListButton.reset (new PlumeShapeButton ("Plugin List Button",
                                                  Colour(0),
                                                  getPlumeColour (headerButtonStroke)));
    addAndMakeVisible (*pluginListButton);
    pluginListButton->setShape (arrowDown, false, true, false);
    pluginListButton->setBorderSize (BorderSize<int> (12, 12,
                                                      12, 12));
    pluginListButton->addListener (this);

    // Save Preset Button
    savePresetButton.reset (new PlumeShapeButton ("Save Preset Button",
                                                  Colour(0),
                                                  getPlumeColour (headerButtonStroke)));
    addAndMakeVisible (*savePresetButton);


    savePresetButton->setShape (PLUME::path::createPath (PLUME::path::floppyDisk), false, true, false);
    savePresetButton->addListener (this);

    // Preset Change buttons
    Path arrowLeft;
    arrowLeft.startNewSubPath (5.0f, 0.0f);
    arrowLeft.lineTo (0.0f, 5.0f);
    arrowLeft.lineTo (5.0f, 10.0f);

    Path arrowRight;
    arrowRight.startNewSubPath (0.0f, 0.0f);
    arrowRight.lineTo (5.0f, 5.0f);
    arrowRight.lineTo (0.0f, 10.0f);

    leftArrowButton.reset (new PlumeShapeButton ("Change Preset Left Button",
                                                 Colour(0),
                                                 getPlumeColour (headerButtonStroke)));
    leftArrowButton->setShape (arrowLeft, false, true, false);
    leftArrowButton->setBorderSize (BorderSize<int> (PLUME::UI::HEADER_HEIGHT/4));
    leftArrowButton->addListener (this);
    addAndMakeVisible (*leftArrowButton);

    rightArrowButton.reset (new PlumeShapeButton ("Change Preset Right Button",
                                                  Colour(0),
                                                  getPlumeColour (headerButtonStroke)));
    rightArrowButton->setShape (arrowRight, false, true, false);
    rightArrowButton->setBorderSize (BorderSize<int> (PLUME::UI::HEADER_HEIGHT/4));
    rightArrowButton->addListener (this);
    addAndMakeVisible (*rightArrowButton);

    trackArmButton.reset (new PlumeShapeButton ("Automation Button",
                                                Colour(0),
                                                getPlumeColour (headerButtonStroke).withAlpha (0.6f),
                                                getPlumeColour (headerButtonStroke)));
    trackArmButton->setToggleState (processor.getParameterTree().getParameter ("track_arm")->getValue() == 1.0f, dontSendNotification);
    trackArmButton->setShape (PLUME::path::createPath (PLUME::path::onOff), false, true, false);
    trackArmButton->setClickingTogglesState (true);
    trackArmButton->setGetsHighlighted(false);
    trackArmButton->addListener (this);
    addAndMakeVisible (*trackArmButton);
}

void HeaderComponent::setPreviousPreset()
{
    setPresetWithOffset (-1);
}

void HeaderComponent::setNextPreset()
{
    setPresetWithOffset (1);
}

void HeaderComponent::setPresetWithOffset (const int offset)
{
    if (offset == 0)
    {
        // Does nothing if the offset is 0...
        jassertfalse;
        return;
    }

    PresetHandler& presetHandler = processor.getPresetHandler();

    // Invalid preset / no current preset / current preset not in search list
    // Loads first or last preset in list
    if (presetHandler.getCurrentPresetIdInSearchList() == -1 && presetHandler.getNumSearchedPresets() > 0)
    {
        if (offset < 0) prepareGesturePanelAndLoadPreset (presetHandler.getNumSearchedPresets() - 1);
        else            prepareGesturePanelAndLoadPreset (0);
    }

    // Valid preset in search list. Loads preset depending on offset
    else if (presetHandler.getNumSearchedPresets() > std::abs (offset))
    {
        int newPresetId = presetHandler.getCurrentPresetIdInSearchList() + offset;

        if (newPresetId >= 0 && newPresetId < presetHandler.getNumSearchedPresets())
        {
            prepareGesturePanelAndLoadPreset (newPresetId);
        }
    }
    
}


void HeaderComponent::prepareGesturePanelAndLoadPreset (const int presetId)
{
    // Prepares GesturePanel For Preset Change
    if (GesturePanel* gesturePanel = dynamic_cast<GesturePanel*> ( getParentComponent()
                                                                       ->findChildWithID("gesturePanel")))
    {
        gesturePanel->stopTimer();
        gesturePanel->removeListenerForAllParameters();
    }

    // Sets Preset

    // Gets the preset Xml and loads it using the processor
    if (std::unique_ptr<XmlElement> presetXml = processor.getPresetHandler().getPresetXmlToLoad (presetId))
    {

        PLUME::log::writeToLog ("Loading preset (using top arrows) : " + processor.getPresetHandler().getPresetForId (presetId).getName(), PLUME::log::LogCategory::presets);

        MemoryBlock presetData;
        AudioProcessor::copyXmlToBinary (*presetXml, presetData);

        processor.getWrapper().removeAllChangeListeners();

        // Calls the plugin's setStateInformation method to load the preset
        processor.setStateInformation (presetData.getData(), static_cast<int>(presetData.getSize()));
        presetXml->deleteAllChildElements(); // frees the memory
    }
    else // failed to get preset xml somehow
    {
        processor.getPresetHandler().resetPreset();
    }
}

//==============================================================================
HeaderComponent::BatteryComponent::BatteryComponent (const float& batteryValRef, DataReader& dataReaderRef) :
	batteryValueRef (batteryValRef),
	dataReader (dataReaderRef)
	//lastBattery(jmax(jmin(PLUME::data::convertRawBatteryToPercentage(batteryValRef), 1.0f), 0.0f)),
	//lastRawBattery(batteryValueRef)
{
    lastBattery = jmax (jmin (PLUME::data::convertRawBatteryToPercentage (batteryValueRef, dataReader.getRingIsCharging()), 1.0f), 0.0f);

    lastRawBattery = batteryValueRef;
    lastConnectionState = dataReader.getRingIsConnected ();

	startTimer (static_cast<int>(batteryCheckTimer), 30000);
    if (lastConnectionState)
        startTimer (static_cast<int>(blinkTimer), 1000);

	//startTimer (static_cast<int>(delayedRepaintTimer), 1000);
	launchDelayedRepaint (3000, true);
}

HeaderComponent::BatteryComponent::~BatteryComponent ()
{
    stopTimer (static_cast<int>(batteryCheckTimer));
}

void HeaderComponent::BatteryComponent::paint (Graphics& g)
{
    g.setColour (getPlumeColour (headerText).withAlpha (0.9f));

    auto area = getLocalBounds().reduced (PLUME::UI::MARGIN);
    const auto ringArea = area.removeFromLeft (32).toFloat();

	drawRingPath (g, ringArea);

    g.setColour (getPlumeColour (sideBarSubText));

    indicatorsArea = area.withTrimmedLeft (PLUME::UI::MARGIN);

    // Ring is connected
	if (lastConnectionState)
	{
		drawBatteryPath (g, indicatorsArea.toFloat ());
	}
    //Ring is disconected
	else
	{
        const auto newIndicatorsArea = indicatorsArea.removeFromLeft(20).withSizeKeepingCentre (15, area.getHeight () * 3 / 4).toFloat ();
        drawConnectedPath (g, newIndicatorsArea);
	}
}

void HeaderComponent::BatteryComponent::timerCallback (int timerID)
{
	if (timerID == static_cast<int>(batteryCheckTimer) && !waitForRepaint)
	{
		repaintIfNeeded ();
	}
	else if (timerID == static_cast<int>(blinkTimer))
	{
        blinkState = !blinkState;
        repaintBlinkingIndicators ();
	}
    else if (timerID == static_cast<int>(delayedRepaintTimer))
    {
        repaintIfNeeded (mForceRepaint);
        mForceRepaint = false;
        stopTimer(static_cast<int>(delayedRepaintTimer));
    }
}

void HeaderComponent::BatteryComponent::repaintIfNeeded (bool forceRepaint)
{
	const float battery = jmax (jmin (PLUME::data::convertRawBatteryToPercentage (batteryValueRef, dataReader.getRingIsCharging()), 1.0f), 0.0f);
	const float newRawBattery = batteryValueRef;

	//DBG ("----------------"
	//	<< "\nComputing battery level"
	//	<< "\nRaw      " << batteryValueRef
	//	<< "\nRounded  " << battery
	//	<< "\nCharging " << (lastChargeState ? "Yes" : "No"));

	if (batteryValueRef < 3.4f)
	{
		if (dataReader.getRingIsCharging () != lastChargeState)
		{
			lastChargeState = dataReader.getRingIsCharging ();
		}

		numBlinkingIndicators = (!lastChargeState && lastBattery == 0.0f) ? 4 : (lastChargeState && lastBattery != 1.0f) ? 1 : 0;

        // Specific case : ring just got into charging and sends a 0-level battery before computing its actual battery level.
		launchDelayedRepaint (500);
	}
	else if (
        forceRepaint ||
        lastConnectionState &&
			((battery != lastBattery &&
                ((!lastChargeState &&
                    (newRawBattery - lastRawBattery) < 0.0f) ||
                    (lastChargeState &&
                        (newRawBattery - lastRawBattery) > 0.01f))) ||
                dataReader.getRingIsCharging () != lastChargeState))
	{
		lastBattery = battery;
		lastRawBattery = newRawBattery;
		lastChargeState = dataReader.getRingIsCharging ();

		numIndicators = battery < 0.0f ? 0
			: (battery == 0.0f && !lastChargeState) ? 4
			: battery < 0.2f ? 1
			: battery < 0.4 ? 2
			: battery < 0.7 ? 3
			: 4;

		numBlinkingIndicators = (!lastChargeState && lastBattery == 0.0f) ? 4
			: (lastChargeState && lastBattery != 1.0f) ? 1
			: 0;

		repaint ();
	}
	else if (dataReader.getRingIsConnected () != lastConnectionState)
	{
		lastConnectionState = dataReader.getRingIsConnected ();

		if (lastConnectionState)
			startTimer (static_cast<int>(blinkTimer), 1000);
		else
			stopTimer (static_cast<int>(blinkTimer));

		if (lastConnectionState)
		{
			numIndicators = battery < 0.0f ? 0
				: (battery == 0.0f && !lastChargeState) ? 4
				: battery < 0.2f ? 1
				: battery < 0.4 ? 2
				: battery < 0.7 ? 3
				: 4;

			numBlinkingIndicators = (!lastChargeState && lastBattery == 0.0f) ? 4
				: (lastChargeState && lastBattery != 1.0f) ? 1
				: 0;
		}

		repaint ();
	}
}

void HeaderComponent::BatteryComponent::repaintBlinkingIndicators ()
{
	if (numBlinkingIndicators != 0)
	{
		if (numBlinkingIndicators == 4)
		{
			repaint ();
		}
		else if (numBlinkingIndicators == 1)
		{
            constexpr int indicatorAreaWidth = 15;

			repaint (indicatorsArea
				.withWidth (indicatorAreaWidth)
				.withX (indicatorsArea.getX () + indicatorAreaWidth * (numIndicators - 1))
				.withSizeKeepingCentre (6, 6));
		}
	}
}

void HeaderComponent::BatteryComponent::update ()
{
    if(dataReader.getRingIsConnected() != lastConnectionState)
    {
        repaintIfNeeded ();
    }

    if(dataReader.getRingIsCharging() != lastChargeState)
    {
        repaintIfNeeded ();
        repaint (getLocalBounds ().withRight (15));
        launchDelayedRepaint (1000);
    }
}

void HeaderComponent::BatteryComponent::launchDelayedRepaint(const int delayMs, bool forceRepaint)
{

    mForceRepaint = forceRepaint;
	startTimer (static_cast<int>(delayedRepaintTimer), delayMs);

    //auto repaintBatteryLambda = [this]()
    //{
    //    repaintIfNeeded ();
    //    waitForRepaint = false;
    //};

    //auto repaintBatteryLambdaForce = [this]()
    //{
    //    repaintIfNeeded (true);
    //    waitForRepaint = false;
    //};

    //waitForRepaint = true;

    //if (forceRepaint)
    //    Timer::callAfterDelay (delayMs, repaintBatteryLambdaForce);
    //else
    //    Timer::callAfterDelay (delayMs, repaintBatteryLambda);
}

void HeaderComponent::BatteryComponent::drawLightningPath (Graphics& g, juce::Rectangle<float> area)
{
	Path lightning;

	lightning.startNewSubPath (7.0f, 0.0f);
	lightning.lineTo (4.0f, 3.0f);
	lightning.lineTo (8.0f, 3.0f);
	lightning.lineTo (3.0f, 8.0f);
	lightning.lineTo (4.0f, 5.0f);
	lightning.lineTo (0.0f, 5.0f);
	lightning.lineTo (3.0f, 0.0f);
	lightning.closeSubPath ();

	lightning.scaleToFit (area.getX (), area.getY (), area.getWidth (), area.getHeight (), true);

    g.fillPath (lightning);
}

void HeaderComponent::BatteryComponent::drawBatteryPath (Graphics& g, juce::Rectangle<float> area)
{
	const Colour fillColour = lastBattery < 0.2f ? Colours::red
		: lastBattery < 0.4f ? Colours::yellow
		: Colours::lime;

    // Repaint if red to prevent fake low batterie level after hubconnection
    if(fillColour == Colours::red)
    {
	    repaintIfNeeded(true);
    }

    //const int indicatorAreaWidth = area.getWidth () / 4;
    const float indicatorAreaWidth = 15.0f;

	for (int indicator = 0; indicator < 4; indicator++)
	{
		auto indicatorArea = area.removeFromLeft (indicatorAreaWidth);

		if (indicator < numIndicators || numBlinkingIndicators == 4)
		{
			// Blinking fill
			if (numBlinkingIndicators == 4 || (numBlinkingIndicators == 1 && indicator == numIndicators - 1))
			{
				g.setColour (blinkState ? fillColour : getPlumeColour(plumeBackground));
			}
			// Standard fill
			else
			{
				g.setColour (fillColour);
			}
		}
		else
		{
            g.setColour (getPlumeColour(plumeBackground));
		}

        g.fillEllipse (indicatorArea.withSizeKeepingCentre (4, 4));

	}
}

void HeaderComponent::BatteryComponent::drawConnectedPath (Graphics& g, juce::Rectangle<float> area)
{
	g.setColour (lastConnectionState ? getPlumeColour (headerText) : getPlumeColour (headerText).withAlpha (0.2f));

	g.drawArrow (Line<float> (area.getX () + area.getWidth () / 4,
		area.getY () + area.getHeight () / 6,
		area.getX () + area.getWidth () / 4,
		area.getY () + area.getHeight ()),
		1.5f, 7.0f, 5.0f);

	g.drawArrow (Line<float> (area.getX () + area.getWidth () * 3 / 4,
		area.getY () + area.getHeight (),
		area.getX () + area.getWidth () * 3 / 4,
		area.getY () + area.getHeight () / 6),
		1.5f, 7.0f, 5.0f);

	if (!lastConnectionState)
	{
		const auto crossArea = area.withSizeKeepingCentre (jmin (area.getWidth (), area.getHeight (),
			area.getWidth (), area.getHeight ()) / 2,
			jmin (area.getWidth (), area.getHeight (),
				area.getWidth (), area.getHeight ()) / 2)
			.withX (area.getX ())
			.withBottomY (area.getY () + area.getHeight ());

		g.setColour (Colours::red);

		g.drawLine (Line<float> (crossArea.getX (),
			crossArea.getY (),
			crossArea.getX () + crossArea.getWidth (),
			crossArea.getY () + crossArea.getHeight ()), 1.5f);
		g.drawLine (Line<float> (crossArea.getX () + crossArea.getWidth (),
			crossArea.getY (),
			crossArea.getX (),
			crossArea.getY () + crossArea.getHeight ()), 1.5f);
	}
}

void HeaderComponent::BatteryComponent::drawRingPath (Graphics& g, juce::Rectangle<float> area)
{
	g.setColour (getPlumeColour (headerText).withAlpha (0.8f));

    // Ring
	Path ringPath = PLUME::path::createPath (PLUME::path::ring);
	const auto transform = ringPath.getTransformToScaleToFit (12, 11, 18, 18, true);
	g.fillPath (ringPath, transform);

    // Lightning
	auto zipzoopArea = area.removeFromRight (7);
	if (lastChargeState)
	{
		drawLightningPath (g, zipzoopArea);
	}
}
