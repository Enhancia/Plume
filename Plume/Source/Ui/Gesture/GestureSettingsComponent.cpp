/*
  ==============================================================================

    GestureSettingsComponent.cpp
    Created: 28 Mar 2019 2:09:04pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Tuner/GesturesTuner.h"
#include "Ui/Gesture/SettingsTabs/MapperComponent.h"
#include "GestureSettingsComponent.h"

//==============================================================================
GestureSettingsComponent::GestureSettingsComponent (Gesture& gest, GestureArray& gestArray, PluginWrapper& wrap)
                            : gesture (gest), gestureArray (gestArray), wrapper (wrap), gestureId (gest.id)
{
    TRACE_IN;
    createTuner();
    createPanels();
    createToggles();
}

GestureSettingsComponent::~GestureSettingsComponent()
{
    TRACE_IN;
    gestureArray.cancelMapMode();
    disabled = true;
    gestTuner = nullptr;
    midiParameterToggle = nullptr;
    descriptionPanel = nullptr;
}

//==============================================================================
const String GestureSettingsComponent::getInfoString()
{
    return "Gesture settings:\n\n"
           "- Use this panel to configure the selected gesture.";
}

void GestureSettingsComponent::update()
{
    if (disabled) return;

	retractablePanel->update();
    descriptionPanel->update();
    gestTuner->updateComponents();
}

//==============================================================================
void GestureSettingsComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;

    // Enhancia Text

    // Background
    paintBackground (g);

    // HeaderText
    auto headerArea = getLocalBounds().removeFromTop (this->HEADER_HEIGHT);

    //Advanced Settings text
    g.setColour (getPlumeColour (detailPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight (10.0f));
    g.drawText ("ADVANCED PANEL",
                headerArea.removeFromLeft (getWidth()/3).reduced (MARGIN),
                Justification::bottomLeft, false);

    // Gesture Name text
    g.setColour (getPlumeColour (detailPanelMainText));                    
    g.setFont (PLUME::font::plumeFontBold.withHeight (15.0f));
    g.drawText (gesture.getName().toUpperCase(),
                headerArea.removeFromLeft (getWidth()/3).reduced (MARGIN, 0),
                Justification::centred, false);
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
    auto headerArea = area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN);

    muteToggle->setBounds (headerArea.removeFromRight (getWidth()/4)
                                      .withTrimmedRight (30));

    retractablePanel->setBounds (area.removeFromBottom (getHeight()/2 - HEADER_HEIGHT));
    area.removeFromBottom (MARGIN);
    gestTuner->setBounds (area);


    if (gesture.type != Gesture::pitchBend && gesture.type != Gesture::vibrato)
    {
        midiParameterToggle->setBounds (retractablePanel->getBounds()
                                            .withHeight (retractablePanel->bannerHeight)
                                            .withLeft (getWidth()*3/4)
                                            .withSizeKeepingCentre (area.getWidth()/4 - MARGIN,
                                                                    retractablePanel->bannerHeight/2));
    }
}

//==============================================================================
void GestureSettingsComponent::buttonClicked (Button*)
{
	// Sets all subcomponents active/inactive depending of the button state
	
	//gesture.setActive (onOffButton->getToggleState()); // Sets gesture active or inactive.
	//gestureArray.checkPitchMerging();
	
	//onOffButton->setState(onOffButton->getState() ? Button::buttonNormal:
	//												Button::buttonDown);
													
	//repaint(); // repaints to get the inactive/active appearance
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
        addAndMakeVisible (gestTuner = new VibratoTuner (vib));
    }
    
    else if (gesture.type == Gesture::pitchBend)
    {
        PitchBend& pitchBend = dynamic_cast<PitchBend&> (gesture);
        addAndMakeVisible (gestTuner = new PitchBendTuner (pitchBend));
    }
    
    else if (gesture.type == Gesture::tilt)
    {
        Tilt& tilt = dynamic_cast<Tilt&> (gesture);
        addAndMakeVisible (gestTuner = new TiltTuner (tilt));
    }
    /*  Un-comment when the wave gesture is implemented
    else if (gesture.type == Gesture::wave)
    {
        Wave& wave = dynamic_cast<Wave&> (gesture);
        addAndMakeVisible (gestTuner = new WaveTuner (wave));
    }
    */
    else if (gesture.type == Gesture::roll)
    {
        Roll& roll = dynamic_cast<Roll&> (gesture);
        addAndMakeVisible (gestTuner = new RollTuner (roll));
    }
    else
    {
        DBG ("Unknown Gesture type. No tuner was created.");
    }
}

void GestureSettingsComponent::createToggles()
{
    addAndMakeVisible (midiParameterToggle = new DualTextToggle ("MIDI", "Param",
                                                                 Colour (0xffd0d010), Colour (0xff10d0d0)));
    midiParameterToggle->setStyle (DualTextToggle::oneStateVisible);
    midiParameterToggle->setToggleState (!gesture.generatesMidi());
    midiParameterToggle->onStateChange = [this] ()
    { 
        gesture.setGeneratesMidi (!midiParameterToggle->getToggleState());
        showAppropriatePanel();
        getParentComponent()->repaint();
    };
    
    
    addAndMakeVisible (muteToggle = new DualTextToggle ("Mute", "Mute",
                                                        Colour (0xff7c80de), Colour (0xffe0e0e0)));
    muteToggle->setStyle (DualTextToggle::oneStateVisible);
    muteToggle->setToggleState (gesture.isActive());
    muteToggle->onStateChange = [this] ()
    { 
        gesture.setActive (muteToggle->getToggleState());
        getParentComponent()->repaint();
    };
}

void GestureSettingsComponent::createPanels()
{
    addAndMakeVisible (descriptionPanel = new DescriptionPanel (gesture));
    addAndMakeVisible (retractablePanel = new RetractableMapAndMidiPanel (gesture, gestureArray,
																		  wrapper, gestTuner->getColour()));
	showAppropriatePanel();
}

void GestureSettingsComponent::showAppropriatePanel()
{
    retractablePanel->setPanelMode (gesture.generatesMidi() ? RetractableMapAndMidiPanel::midiMode
                                                           : RetractableMapAndMidiPanel::parameterMode);
}