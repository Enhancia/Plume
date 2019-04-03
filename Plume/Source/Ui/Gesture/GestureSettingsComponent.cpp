/*
  ==============================================================================

    GestureSettingsComponent.cpp
    Created: 28 Mar 2019 2:09:04pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Tuner/GesturesTuner.h"
#include "Ui/Gesture/Mapper/MapperComponent.h"
#include "GestureSettingsComponent.h"

//==============================================================================
GestureSettingsComponent::GestureSettingsComponent (Gesture& gest, GestureArray& gestArray, PluginWrapper& wrap)
                            : gesture (gest), gestureArray (gestArray), wrapper (wrap)
{
    createTuner();
    addAndMakeVisible( gestMapper = new MapperComponent(gesture, gestureArray, wrapper));
}

GestureSettingsComponent::~GestureSettingsComponent()
{
    TRACE_IN;
    gestMapper = nullptr;
    gestTuner = nullptr;
}

//==============================================================================
const String GestureSettingsComponent::getInfoString()
{
    return "Gesture settings:\n\n"
           "- Use this panel to configure the selected gesture.";
}

void GestureSettingsComponent::update()
{
    gestTuner->updateComponents();
    gestMapper->updateComponents();
}

//==============================================================================
void GestureSettingsComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    auto area = getLocalBounds().reduced (2*MARGIN).withLeft (0);

    g.setColour (Colours::white);
    g.fillRoundedRectangle (area.toFloat(), 3.0f);

    area.reduce (MARGIN, MARGIN);

    // Gesture Name text
    g.setColour (Colour(0xff323232));                    
    g.setFont (PLUME::font::plumeFontBold.withHeight (15.0f));
    g.drawText (gesture.name,
                area.removeFromTop (20),
                Justification::centredLeft, false);

    //Gesture Type Text
    g.setFont (PLUME::font::plumeFont.withHeight (13.0f));
    g.drawText (gesture.getTypeString (true),
                area.removeFromTop (20),
                Justification::centredLeft, false);
}

void GestureSettingsComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds().reduced (3*MARGIN).withLeft (MARGIN).withTrimmedTop (40);

    gestTuner->setBounds (area.removeFromTop (area.getHeight()/2).reduced (MARGIN));

    gestMapper->setBounds (area.reduced (MARGIN));
    repaint();
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
int GestureSettingsComponent::getGestureId()
{
    return gesture.id;
}

Tuner& GestureSettingsComponent::getTuner()
{
    return *gestTuner;
}

void GestureSettingsComponent::updateDisplay()
{
    if (gesture.isActive()) gestTuner->updateDisplay();
    if (gesture.isMapped()) gestMapper->updateDisplay();
}

void GestureSettingsComponent::updateComponents()
{
    //Buttons etc..
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