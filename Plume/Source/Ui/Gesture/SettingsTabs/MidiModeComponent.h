/*
  ==============================================================================

    MidiModeComponent.h
    Created: 17 Oct 2018 2:08:20pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/PlumeCommon.h"
#include "../../../Gesture/Gesture.h"
#include "../../../Gesture/GestureArray.h"

//==============================================================================
/*
*/
class MidiRangeTuner : public Component,
                       private Label::Listener,
                       private Slider::Listener
{
public:
    //==============================================================================
    enum DraggableObject
    {
        none = 0,

        lowThumb,
        highThumb,
        middleArea
    };

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
    void updateComponents (DraggableObject thumbThatShouldUpdate);
    void updateHighlightColour();

private:
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
    int lastValue = -1;

    //==============================================================================
    std::unique_ptr<Label> rangeLabelMin;
    std::unique_ptr<Label> rangeLabelMax;
    std::unique_ptr<Slider> lowSlider;
    std::unique_ptr<Slider> highSlider;

    DraggableObject objectBeingDragged = none;
    Colour highlightColour;

    PLUME::UI::TestTunerLookAndFeel sliderLookAndFeel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiRangeTuner)
};


//==================================================================================

class MidiModeComponent    : public Component,
                             private Label::Listener,
                             private ComboBox::Listener,
                             private Button::Listener
{
public:
    MidiModeComponent (Gesture& gest, GestureArray& gestArray);
    ~MidiModeComponent();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void buttonClicked (Button* bttn) override;
    void comboBoxChanged (ComboBox* box) override;
    
    //==============================================================================
    void updateComponents();
    void updateDisplay();

    MidiRangeTuner& getTuner();

private:
    //==============================================================================
    void createComboBox();
    void createLabels();
    void createButton();
    
    //==============================================================================
    std::unique_ptr<ComboBox> midiTypeBox;
    std::unique_ptr<Label> ccLabel;
    std::unique_ptr<Label> rangeLabelMin;
    std::unique_ptr<Label> rangeLabelMax;
    std::unique_ptr<MidiRangeTuner> midiRangeTuner;
    std::unique_ptr<TextButton> reverseButton;

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
