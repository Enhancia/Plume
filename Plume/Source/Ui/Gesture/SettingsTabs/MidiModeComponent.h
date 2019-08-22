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
#include "Gesture/GestureArray.h"

//==============================================================================
/*
*/
class MidiRangeTuner : public Component,
                       private Label::Listener,
                       private Slider::Listener
{
public:
    //==============================================================================
    MidiRangeTuner(Gesture& gest);
    ~MidiRangeTuner();

    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;

    //==============================================================================
    void labelTextChanged(Label* lbl) override;
    void editorHidden(Label* lbl, TextEditor& ted) override;
    void sliderValueChanged(Slider* sldr) override;

    //==============================================================================
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    //==============================================================================
    void updateDisplay();
    void updateHighlightColour();

private:
    //==============================================================================
    enum DraggableObject
    {
        none = 0,

        lowThumb,
        highThumb,
        middleArea
    };

    //==============================================================================
    void createLabels();
    void createSliders();

    //==============================================================================
    float getThumbX(DraggableObject thumb);
    void setLabelBounds(Label& labelToResize);

    void handleSliderClick(const MouseEvent& e);
    DraggableObject getObjectToDrag(const MouseEvent& e);

    //==============================================================================
    void drawCursor(Graphics& g);
    void drawSliderBackground(Graphics& g);

    //==============================================================================
    Gesture& gesture;

    //==============================================================================
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    ScopedPointer<Slider> lowSlider;
    ScopedPointer<Slider> highSlider;

    DraggableObject objectBeingDragged = none;
    Colour highlightColour;

    PLUME::UI::TestTunerLookAndFeel sliderLookAndFeel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiRangeTuner)
};


//==================================================================================

class MidiModeComponent    : public Component,
                             private Label::Listener,
                             private ComboBox::Listener
{
public:
    MidiModeComponent (Gesture& gest, GestureArray& gestArray);
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
    
    //==============================================================================
    ScopedPointer<ComboBox> midiTypeBox;
    ScopedPointer<Label> ccLabel;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    ScopedPointer<MidiRangeTuner> midiRangeTuner;

    //==============================================================================
    Gesture& gesture;
    GestureArray& gestureArray;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiModeComponent)
};

//==================================================================================

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