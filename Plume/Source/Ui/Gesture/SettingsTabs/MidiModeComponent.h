/*
  ==============================================================================

    MidiModeComponent.h
    Created: 17 Oct 2018 2:08:20pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Gesture/Gesture.h"

//==============================================================================
/*
*/
class MidiModeComponent    : public Component,
                             private Label::Listener,
                             private ComboBox::Listener
{
public:
    MidiModeComponent(Gesture& gest);
    ~MidiModeComponent();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    void labelTextChanged (Label* lbl);
    void comboBoxChanged (ComboBox* box);
    
    //==============================================================================
    void updateComponents();

private:
    //==============================================================================
    void createComboBox();
    void createLabels();
    void createDefaultToggle();

    void setComponentsVisibility();
    
    //==============================================================================
    ScopedPointer<ComboBox> midiTypeBox;
    ScopedPointer<Label> ccLabel;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    ScopedPointer<DualTextToggle> defaultMidiToggle;

    //==============================================================================
    Gesture& gesture;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiModeComponent)
};

class MidiBanner : public Component
{
public:
    //==============================================================================
    MidiBanner (Gesture& gest);
    ~MidiBanner();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    Gesture& gesture;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiBanner)
};