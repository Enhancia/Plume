/*
  ==============================================================================

    SymetricTuner.h
    Created: 19 Jul 2018 2:31:30pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
using namespace juce;
#include "../../LookAndFeel/PlumeLookAndFeel.h"
#include "Tuner.h"

class TwoRangeTuner:    public Tuner,
                        private Slider::Listener,
                        private Label::Listener,
                        private Button::Listener
{
public:
    enum DraggableObject
    {
        none = -1,
        leftLowThumb,
        leftHighThumb,
        rightLowThumb,
        rightHighThumb,
        middleAreaLeft,
        middleAreaRight
    };

    //==============================================================================
    TwoRangeTuner(const std::atomic<float>& val, const NormalisableRange<float> gestureRange,
                  float& rangeLL, float& rangeLH,
                  float& rangeRL, float& rangeRH,
                  const NormalisableRange<float> paramMax, const String unit = "");
    ~TwoRangeTuner();
    
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
    void setAngles (float startAngle, float endAngle);

private:
    //==============================================================================
    void createSliders();
    void resizeSliders();
    void createLabels();
    void createButtons();
    void resizeButtons();
    
    //==============================================================================
    void setRangeLeftLow (float value, const bool createChangeGesture = false);
    void setRangeLeftHigh (float value, const bool createChangeGesture = false);
    void setRangeRightLow (float value, const bool createChangeGesture = false);
    void setRangeRightHigh (float value, const bool createChangeGesture = false);
    
    float getRangeLeftLow();
    float getRangeLeftHigh();
    float getRangeRightLow();
    float getRangeRightHigh();
    
    //==============================================================================
    double getAngleFromMouseEventRadians (const MouseEvent& e);
    double getThumbAngleRadians (const DraggableObject thumb);

    DraggableObject getObjectToDrag (const MouseEvent& e);
    void handleSingleClick (const MouseEvent& e);
    void handleDoubleClick (const MouseEvent& e);

    void drawTunerSliderBackground (Graphics& g);
    void updateLabelBounds (Label* labelToUpdate);

    float getValueAngle();
    void drawValueCursor (Graphics& g);
    void drawLineFromSliderCentre (Graphics& g, float angleRadian);
    void drawThumbsAndToleranceLines (Graphics& g);
    
    //==============================================================================
    const std::atomic<float>& value;
    const NormalisableRange<float> gestureRange;
    
    //==============================================================================
    float& rangeLeftLow;
    float& rangeLeftHigh;
    float& rangeRightLow;
    float& rangeRightHigh;
    const NormalisableRange<float> parameterMax;
    
    //==============================================================================
    std::unique_ptr<Slider> leftLowSlider;
    std::unique_ptr<Slider> leftHighSlider;
    std::unique_ptr<Slider> rightLowSlider;
    std::unique_ptr<Slider> rightHighSlider;
    
    std::unique_ptr<Label> rangeLabelMinLeft;
    std::unique_ptr<Label> rangeLabelMaxLeft;
    std::unique_ptr<Label> rangeLabelMinRight;
    std::unique_ptr<Label> rangeLabelMaxRight;

    std::unique_ptr<TextButton> minLeftAngleButton;
    std::unique_ptr<TextButton> maxLeftAngleButton;
    std::unique_ptr<TextButton> minRightAngleButton;
    std::unique_ptr<TextButton> maxRightAngleButton;
    
    //==============================================================================
    DraggableObject objectBeingDragged = none;
    float previousCursorAngle = value;

    juce::Rectangle<int> sliderBounds;
    float sliderRadius;
    juce::Point<int> sliderCentre;
    float startAngle;
    float endAngle;

    //==============================================================================
    PLUME::UI::OneRangeTunerLookAndFeel slidersLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TwoRangeTuner)
};