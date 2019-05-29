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
    createToggles();
    createPanels();
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

    gestTuner->updateComponents();
    midiPanel->updateComponents();
    mappedParametersPanel->updateComponents();
    descriptionPanel->update();
}

//==============================================================================
void GestureSettingsComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;

    //Gradient for horizontal lines
    auto grad = ColourGradient::horizontal (Colour (0x15323232),
                                            float(MARGIN), 
                                            Colour (0x15323232),
                                            float(getWidth() - MARGIN));
    grad.addColour (0.5, Colour (0x50323232));
    
    auto area = getLocalBounds().reduced (2*MARGIN).withLeft (0);

    g.setColour (Colours::white);
    g.fillRoundedRectangle (area.toFloat(), 3.0f);

    area.reduce (MARGIN, MARGIN);

    // Gesture Name text
    g.setColour (Colour(0xff323232));                    
    g.setFont (PLUME::font::plumeFontBold.withHeight (15.0f));
    g.drawText (gesture.getName(),
                area.removeFromTop (20),
                Justification::centredLeft, false);

    //Gesture Type Text
    g.setFont (PLUME::font::plumeFont.withHeight (13.0f));
    g.drawText (gesture.getTypeString (true),
                area.removeFromTop (20),
                Justification::centredLeft, false);

    g.setGradientFill (grad);
    g.drawHorizontalLine (area.removeFromTop (80).getY(),
                          float(area.getX() + 4*MARGIN), float(area.getWidth() - 4*MARGIN));
    g.drawHorizontalLine (area.getY(),
                          float(area.getX() + 4*MARGIN), float(area.getWidth() - 4*MARGIN));

    // Toggle on/off Text
    g.setColour (Colour(0xff323232));  
    g.setFont (PLUME::font::plumeFont.withHeight (13.0f));
    g.drawText ("Toggle On/Off : ",
                area.removeFromTop (30).withWidth (area.getWidth()/2).reduced (2*MARGIN, 0),
                Justification::centredRight, false);

    // Control Type Text
    g.setColour (Colour(0xff323232));  
    g.setFont (PLUME::font::plumeFont.withHeight (13.0f));
    g.drawText ("Control Type : ",
                area.removeFromTop (30).withWidth (area.getWidth()/2).reduced (2*MARGIN, 0),
                Justification::centredRight, false);
}

void GestureSettingsComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds().reduced (3*MARGIN).withLeft (MARGIN).withTrimmedTop (40);

    gestTuner->setBounds (area.removeFromTop (80).reduced (MARGIN));

    onOffToggle->setBounds (area.removeFromTop (30).withLeft (area.getWidth()/2)
                                                   .withWidth (120)
                                                   .reduced (MARGIN));

    midiParameterToggle->setBounds (area.removeFromTop (30).withLeft (area.getWidth()/2)
                                                           .withWidth (120)
                                                           .reduced (MARGIN));

    descriptionPanel->setBounds (area.removeFromBottom (area.getHeight()/3)
                                     .reduced (MARGIN, 3*MARGIN));

    midiPanel->setBounds (area.reduced (MARGIN));
    mappedParametersPanel->setBounds (area.reduced (MARGIN));

    //repaint();
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
        
        mappedParametersPanel->updateDisplay();
    }
}

void GestureSettingsComponent::updateMappedParameters()
{
    mappedParametersPanel->initializeParamCompArray();
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
    addAndMakeVisible (midiParameterToggle = new DualTextToggle ("MIDI", "Param"));
    midiParameterToggle->setStyle (DualTextToggle::twoStatesVisible);
    midiParameterToggle->setToggleState (!gesture.isMidiMapped());
    midiParameterToggle->onStateChange = [this] ()
    { 
        gesture.setMidiMapped (!midiParameterToggle->getToggleState());
        showAppropriatePanel();
        getParentComponent()->repaint();
    };
    
    addAndMakeVisible (onOffToggle = new DualTextToggle ("Off", "On", Colour (0xffe0e0e0), Colour (0xffa0f0a0)));
    onOffToggle->setStyle (DualTextToggle::twoStatesVisible);
    onOffToggle->setToggleState (gesture.isActive());
    onOffToggle->onStateChange = [this] ()
    { 
        gesture.setActive (onOffToggle->getToggleState());
        getParentComponent()->repaint();
    };
}

void GestureSettingsComponent::createPanels()
{
    addAndMakeVisible (midiPanel = new MidiModeComponent (gesture));
    addAndMakeVisible (mappedParametersPanel = new MapperComponent (gesture, gestureArray, wrapper));
    addAndMakeVisible (descriptionPanel = new DescriptionPanel (gesture));

    gesture.isMidiMapped() ? mappedParametersPanel->setVisible (false) : midiPanel->setVisible (false);
}

void GestureSettingsComponent::showAppropriatePanel()
{
    midiPanel->setVisible (gesture.isMidiMapped());
    mappedParametersPanel->setVisible (!gesture.isMidiMapped());
}