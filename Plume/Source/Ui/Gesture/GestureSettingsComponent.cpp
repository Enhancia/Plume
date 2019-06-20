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
}

//==============================================================================
void GestureSettingsComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;

    //Gradient for horizontal lines
    auto grad = ColourGradient::horizontal (Colour (0x10323232),
                                            float(MARGIN), 
                                            Colour (0x10323232),
                                            float(getWidth() - MARGIN));
    grad.addColour (0.5, Colour (0x50323232));
    
    auto area = getLocalBounds();
    g.setColour (Colours::white);

    auto retractableArea = area.removeFromBottom (retractablePanel->getBounds().expanded (MARGIN).getHeight() + 2*MARGIN);
    retractableArea.setTop (retractableArea.getY() + 2*MARGIN);
    //if (retractablePanel->isRetracted()) retractableArea.setHeight (retractablePanel->bannerHeight + 2*MARGIN);

    g.fillRoundedRectangle (area.toFloat(), 3.0f);
    g.fillRoundedRectangle (retractableArea.toFloat(), 3.0f);

    area.reduce (MARGIN, MARGIN);
    auto headerArea = area.removeFromTop (this->HEADER_HEIGHT);

    //Gesture Type Text
    g.setColour (Colour(0xff626262));
    g.setFont (PLUME::font::plumeFont.withHeight (13.0f));
    g.drawText (gesture.getTypeString (true),
                headerArea.removeFromLeft (getWidth()/4).reduced (MARGIN, 0),
                Justification::centred, false);

    // Gesture Name text
    g.setColour (Colour(0xff323232));                    
    g.setFont (PLUME::font::plumeFontBold.withHeight (15.0f));
    g.drawText (gesture.getName(),
                headerArea.removeFromLeft (getWidth()/2).reduced (MARGIN, 0),
                Justification::centred, false);

    g.setGradientFill (grad);

    g.drawHorizontalLine (area.removeFromTop (getHeight()*6/10).getY(),
                              float(area.getX() + 2*MARGIN), float(area.getWidth() - 2*MARGIN));

    //g.drawHorizontalLine (area.getY(),
    //                      float(area.getX() + 2*MARGIN), float(area.getWidth() - 2*MARGIN));
}

void GestureSettingsComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds().reduced (MARGIN);
    auto headerArea = area.removeFromTop (HEADER_HEIGHT);

    muteToggle->setBounds (headerArea.removeFromRight (getWidth()/4)
                                      .withTrimmedRight (30)
                                      .reduced (0.5*MARGIN, MARGIN));

    retractablePanel->setBounds (area.removeFromBottom (retractablePanel->isRetracted() ? 
                                                            retractablePanel->bannerHeight :
                                                            getHeight()*2/5 - HEADER_HEIGHT)
                                     .reduced (MARGIN, 0));

    gestTuner->setBounds (area.withSizeKeepingCentre (jmin (getWidth() - 2*MARGIN, 300), 
                                                      jmin (getHeight()*6/10 - 6*MARGIN, 100)));


    if (gesture.type != Gesture::pitchBend && gesture.type != Gesture::vibrato)
    {
        midiParameterToggle->setBounds (retractablePanel->getBounds()
                                            .withHeight (retractablePanel->bannerHeight/2)
                                            .withLeft (area.getWidth()*3/4)
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
    addAndMakeVisible (retractablePanel = new RetractableMapAndMidiPanel (gesture, gestureArray, wrapper));
	showAppropriatePanel();
}

void GestureSettingsComponent::showAppropriatePanel()
{
    retractablePanel->setPanelMode (gesture.generatesMidi() ? RetractableMapAndMidiPanel::midiMode
                                                           : RetractableMapAndMidiPanel::parameterMode);
}