/*
  ==============================================================================

    SymetricTuner.h
    Created: 19 Jul 2018 2:31:30pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../LookAndFeel/PlumeLookAndFeel.h"
#include "Tuner.h"

class OneRangeTuner:  public Tuner,
                      private Slider::Listener,
                      private Label::Listener,
                      private Button::Listener
{
public:
    enum TunerStyle
    {
        tilt =0,
        roll,
        wave
    };

    enum DraggableObject
    {
        none = -1,
        lowThumb,
        highThumb,
        middleArea
    };

    //==============================================================================
    OneRangeTuner(const std::atomic<float>& val, NormalisableRange<float> gestureRange,
                  float& rangeL, float& rangeH, const NormalisableRange<float> paramMax,
                  const String unit = "", TunerStyle style = wave);
    ~OneRangeTuner();
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void updateComponents() override;
    void updateComponents (DraggableObject thumbThatShouldUpdate);

    void updateDisplay() override;

    void setColour (const Colour newColour) override;
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void editorHidden (Label* lbl, TextEditor& ted) override;
    void sliderValueChanged (Slider* sldr) override;
    void buttonClicked (Button* bttn) override;

    //==============================================================================
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    MouseCursor getMouseCursor() override;

    //==============================================================================
    void setStyle (TunerStyle newStyle);
    void setAngles (float startAngle, float endAngle);
    
private:
    //==============================================================================
    void createSliders();
    void resizeSliders();
    void createLabels();
    void createButtons();
    void resizeButtons();
    
    //==============================================================================
    void setRangeLow (float value, const bool createChangeGesture = false);
    void setRangeHigh (float value, const bool createChangeGesture = false);
    
    float getRangeLow();
    float getRangeHigh();

    double getAngleFromMouseEventRadians (const MouseEvent& e);
    double getThumbAngleRadians (const DraggableObject thumb);

    DraggableObject getObjectToDrag (const MouseEvent& e);
    void handleSingleClick (const MouseEvent& e);
    void handleDoubleClick (const MouseEvent& e);


    void drawTunerSliderBackground (Graphics& g);
    void updateLabelBounds (Label* labelToUpdate);

    float getValueAngle();
    void drawValueCursor (Graphics& g);
    void drawLineFromSliderCentre (Graphics&, float angleRadian);
    void drawThumbsAndToleranceLines (Graphics& g);
    
    //==============================================================================
    const std::atomic<float>& value;
    const NormalisableRange<float> gestureRange;

    //==============================================================================
    const NormalisableRange<float> parameterMax;
    float& rangeLow;
    float& rangeHigh;
    
    std::unique_ptr<Slider> lowSlider;
    std::unique_ptr<Slider> highSlider;
    std::unique_ptr<Label> rangeLabelMin;
    std::unique_ptr<Label> rangeLabelMax;
    std::unique_ptr<TextButton> minAngleButton;
    std::unique_ptr<TextButton> maxAngleButton;
    
    //==============================================================================
    TunerStyle tunerStyle;

    DraggableObject objectBeingDragged = none;
    float previousCursorAngle = value;

    juce::Rectangle<int> sliderBounds;
    float sliderRadius;
    juce::Point<int> sliderCentre;
    float startAngle;
    float endAngle;

    //==============================================================================
    PLUME::UI::OneRangeTunerLookAndFeel oneRangeTunerLookAndFeel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneRangeTuner)
};
