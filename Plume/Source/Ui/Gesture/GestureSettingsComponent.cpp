/*
  ==============================================================================

    GestureSettingsComponent.cpp
    Created: 28 Mar 2019 2:09:04pm
    Author:  Alex

  ==============================================================================
*/

#include "Tuner/GesturesTuner.h"
#include "SettingsTabs/mappercomponent.h"
#include "GestureSettingsComponent.h"

//==============================================================================
GestureSettingsComponent::GestureSettingsComponent (Gesture& gest, GestureArray& gestArray, PluginWrapper& wrap)
                            : gesture (gest), gestureArray (gestArray),
                              wrapper (wrap), gestureId (gest.id)
{
    setComponentID ("Gesture Settings");

    createTuner();
    createPanels();
    createToggles();
}

GestureSettingsComponent::~GestureSettingsComponent()
{
    gestureArray.cancelMapMode();
    disabled = true;
    gestTuner = nullptr;
    midiParameterToggle = nullptr;
    descriptionPanel = nullptr;
}

//==============================================================================
const String GestureSettingsComponent::getInfoString()
{
    const String bullet = " " + String::charToString (juce_wchar(0x2022));

    return "Gesture settings:\n\n"
           + bullet + " Use this panel to configure the selected gesture.";
}

void GestureSettingsComponent::update()
{
    if (disabled) return;

	retractablePanel->update();
    descriptionPanel->update();
    gestTuner->setColour (gesture.getHighlightColour());
    gestTuner->updateComponents();
    muteButton->setToggleState (gesture.isActive(), dontSendNotification);
    midiParameterToggle->setToggleState (gesture.generatesMidi());
}


void GestureSettingsComponent::update (const String& parameterThatChanged)
{
    if (disabled) return;

    if (parameterThatChanged.isEmpty())
    {
        update();
    }
    else
    {
        if (auto* oneRangeTuner = dynamic_cast<OneRangeTuner*> (gestTuner.get()))
        {
            if (parameterThatChanged.endsWith ("m_0"))
            {
                oneRangeTuner->updateComponents (OneRangeTuner::lowThumb);
            }
            else if (parameterThatChanged.endsWith ("m_1"))
            {
                oneRangeTuner->updateComponents (OneRangeTuner::highThumb);
            }
        }

        else if (auto* twoRangeTuner = dynamic_cast<TwoRangeTuner*> (gestTuner.get()))
        {
            if (parameterThatChanged.endsWith ("m_0"))
            {
                twoRangeTuner->updateComponents (TwoRangeTuner::leftLowThumb);
            }
            else if (parameterThatChanged.endsWith ("m_1"))
            {
                twoRangeTuner->updateComponents (TwoRangeTuner::leftHighThumb);
            }
            else if (parameterThatChanged.endsWith ("m_2"))
            {
                twoRangeTuner->updateComponents (TwoRangeTuner::rightLowThumb);
            }
            else if (parameterThatChanged.endsWith ("m_3"))
            {
                twoRangeTuner->updateComponents (TwoRangeTuner::rightHighThumb);
            }
        }
        else
        {
            gestTuner->updateComponents();
        }
    }
}

//==============================================================================
void GestureSettingsComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    // Background
    paintBackground (g);

    // HeaderText
    auto headerArea = getLocalBounds().removeFromTop (this->HEADER_HEIGHT);

    //Advanced Settings text
    g.setColour (getPlumeColour (detailPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight (10.0f));
    g.drawText ("ADVANCED PANEL",
                headerArea.removeFromLeft (getWidth()/3).reduced (MARGIN, MARGIN_SMALL),
                Justification::bottomLeft, false);

    // Gesture Name text
    if (auto* gesturePtr = gestureArray.getGesture (gestureId))
    {
        g.setColour (getPlumeColour (detailPanelMainText));                    
        g.setFont (PLUME::font::plumeFontMedium.withHeight (15.0f));
        g.drawText (gesturePtr->getName().toUpperCase(),
                    headerArea.removeFromLeft (getWidth()/3).reduced (MARGIN, 0),
                    Justification::centred, false);
    }
}

void GestureSettingsComponent::paintBackground (Graphics& g)
{
    using namespace PLUME::UI;
    auto area = getLocalBounds().withBottom (retractablePanel->getBottom());

    // MidiMap Background
    g.setColour (getPlumeColour (midiMapBodyBackground));
    g.fillRoundedRectangle (area.withTop (retractablePanel->getY()).toFloat(), 10.0f);

    // Tuner backGround
    g.setColour (getPlumeColour (tunerBackground));
    g.fillRoundedRectangle (area.withBottom (retractablePanel->getY() + retractablePanel->bannerHeight)
                                .toFloat(),
                            10.0f);

    Path tile;
    tile.addEllipse (0, 0, 2, 2);

    PLUME::UI::paintTiledPath (g, tile,
                               area.withBottom (retractablePanel->getY() + retractablePanel->bannerHeight)
                                   .toFloat(),
                               15.0f, 15.0f, getPlumeColour (midiMapBodyBackground), Colour (0), 0.0f);

    g.saveState();

    //Header Fill
    g.reduceClipRegion (0, 0, getWidth(), 30);
    g.setColour (getPlumeColour (detailPanelHeaderFill));
    g.fillRoundedRectangle (area.withBottom (retractablePanel->getY() + retractablePanel->bannerHeight)
                                .toFloat(),
                            10.0f);

    g.restoreState();
    g.saveState();

    //MidiMap banner fill
    g.reduceClipRegion (0, retractablePanel->getY(), getWidth(), retractablePanel->bannerHeight);
    g.setColour (getPlumeColour (midiMapBannerBackground));
    g.fillRoundedRectangle (area.withBottom (retractablePanel->getY() + retractablePanel->bannerHeight)
                                .toFloat(),
                            10.0f);

    g.restoreState();
}

void GestureSettingsComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds();
    auto headerArea = area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN_SMALL);

    muteButton->setBounds (headerArea.removeFromRight (20).withSizeKeepingCentre (18, 18));

    retractablePanel->setBounds (area.removeFromBottom (getHeight()/2 - HEADER_HEIGHT));
    area.removeFromBottom (MARGIN);
    gestTuner->setBounds (area);

    midiParameterToggle->setBounds (retractablePanel->getBounds()
                                            .withHeight (retractablePanel->bannerHeight)
                                            .withLeft (getWidth() - 50)
                                            .reduced (3*MARGIN_SMALL, MARGIN));
}

//==============================================================================
void GestureSettingsComponent::buttonClicked (Button*)
{
}
    

//==============================================================================
const int GestureSettingsComponent::getGestureId()
{
    return gesture.id;
}

Tuner& GestureSettingsComponent::getTuner()
{
    return *gestTuner;
}

void GestureSettingsComponent::updateDisplay()
{
    if (!disabled) disableIfGestureWasDeleted();

    if (!disabled)
    {
        if (gesture.isActive()) gestTuner->updateDisplay();
        
        retractablePanel->updateDisplay();
    }
}

void GestureSettingsComponent::updateMappedParameters()
{
    retractablePanel->initializeParamCompArray();
}

void GestureSettingsComponent::disableIfGestureWasDeleted()
{
    if (gestureArray.getGesture (gestureId) == nullptr)
    {
        disabled = true;
    }
}

void GestureSettingsComponent::createTuner()
{
	if (gesture.type == Gesture::vibrato)
    {
        Vibrato& vib = dynamic_cast<Vibrato&> (gesture);
        gestTuner.reset (new VibratoTuner (vib));
        addAndMakeVisible (*gestTuner);
    }
    
    else if (gesture.type == Gesture::pitchBend)
    {
        PitchBend& pitchBend = dynamic_cast<PitchBend&> (gesture);
        gestTuner.reset (new PitchBendTuner (pitchBend));
        addAndMakeVisible (*gestTuner);
    }
    
    else if (gesture.type == Gesture::tilt)
    {
        Tilt& tilt = dynamic_cast<Tilt&> (gesture);
        gestTuner.reset (new TiltTuner (tilt));
        addAndMakeVisible (*gestTuner);
    }
    /*  Un-comment when the wave gesture is implemented
    else if (gesture.type == Gesture::wave)
    {
        Wave& wave = dynamic_cast<Wave&> (gesture);
        gestTuner.reset (new WaveTuner (wave));
        addAndMakeVisible (*gestTuner);
    }
    */
    else if (gesture.type == Gesture::roll)
    {
        Roll& roll = dynamic_cast<Roll&> (gesture);
        gestTuner.reset (new RollTuner (roll));
        addAndMakeVisible (*gestTuner);
    }
    else
    {
        DBG ("Unknown Gesture type. No tuner was created.");
    }

    gestTuner->setColour (gesture.getHighlightColour());
}

void GestureSettingsComponent::createToggles()
{
    addAndMakeVisible (*(midiParameterToggle = std::make_unique<DualTextToggle> ("MIDI", "MIDI",
                                                                 getPlumeColour (plumeBackground),
                                                                 gesture.getHighlightColour())));
    midiParameterToggle->setStyle (DualTextToggle::toggleWithTopText);
    midiParameterToggle->setToggleState (gesture.generatesMidi());
    midiParameterToggle->setStateIndependentTextColour (getPlumeColour (detailPanelMainText));
    midiParameterToggle->setToggleThumbColour (getPlumeColour (tunerSliderBackground));
    midiParameterToggle->onStateChange = [this] ()
    {
        gesture.setGeneratesMidi (midiParameterToggle->getToggleState());
        showAppropriatePanel();
        getParentComponent()->repaint();
    };

    addAndMakeVisible (*(muteButton = std::make_unique<PlumeShapeButton> ("Mute Button",
                                                          getPlumeColour (plumeBackground),
                                                          Gesture::getHighlightColour (gesture.type, false),
                                                          Gesture::getHighlightColour (gesture.type))));

    muteButton->setShape (PLUME::path::createPath (PLUME::path::onOff), false, true, false);
    muteButton->setToggleState (gesture.isActive(), dontSendNotification);
    muteButton->setClickingTogglesState (true);
    muteButton->onClick = [this] ()
    {
        gesture.setActive (muteButton->getToggleState());

        PLUME::log::writeToLog ("Gesture " + gesture.getName() + " (Id " + String (gesture.id) + (muteButton->getToggleState() ? ") Muting." : ") Unmuting."),
                                PLUME::log::gesture);
        update();
        if (auto* gestComp = dynamic_cast<PlumeComponent*> (getParentComponent()->findChildWithID ("gestComp" + String (gestureId))))
        {
            gestComp->update();
        }
    };
}

void GestureSettingsComponent::createPanels()
{
    addAndMakeVisible (*(descriptionPanel = std::make_unique<DescriptionPanel> (gesture)));
    addAndMakeVisible (*(retractablePanel = std::make_unique <RetractableMapAndMidiPanel> (gesture, gestureArray,
																		  wrapper, gestTuner->getColour())));
	showAppropriatePanel();
}

void GestureSettingsComponent::showAppropriatePanel()
{
    retractablePanel->setPanelMode (gesture.generatesMidi() ? RetractableMapAndMidiPanel::midiMode
                                                            : RetractableMapAndMidiPanel::parameterMode);
}
