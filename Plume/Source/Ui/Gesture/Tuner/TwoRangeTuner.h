/*
  ==============================================================================

    SymetricTuner.h
    Created: 19 Jul 2018 2:31:30pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/Gesture/Tuner/Tuner.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

class TwoRangeTuner:    public Tuner,
                        private Slider::Listener,
                        private Label::Listener
{
public:
    //==============================================================================
    TwoRangeTuner(const float& val, const NormalisableRange<float> gestureRange,
                  RangedAudioParameter& rangeLL, RangedAudioParameter& rangeLH,
                  RangedAudioParameter& rangeRL, RangedAudioParameter& rangeRH,
                  const Range<float> paramMax,
                  const String unit = "", bool show = true);
    ~TwoRangeTuner();
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void updateComponents() override;
    void updateDisplay() override;
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void sliderValueChanged (Slider* sldr) override;
    
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
    
    //==============================================================================
    void setRangeLeftLow (float value);
    void setRangeLeftHigh (float value);
    void setRangeRightLow (float value);
    void setRangeRightHigh (float value);
    
    float getRangeLeftLow();
    float getRangeLeftHigh();
    float getRangeRightLow();
    float getRangeRightHigh();
    
    //==============================================================================
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

    double getAngleFromMouseEventRadians (const MouseEvent& e);
    double getThumbAngleRadians (const DraggableObject thumb);

    DraggableObject getObjectToDrag (const MouseEvent& e);

    void drawTunerSliderBackground (Graphics& g);
    void updateLabelBounds (Label* labelToUpdate);

    float getValueAngle();
    void drawValueCursor (Graphics& g);
    
    //==============================================================================
    const float& value;
    const NormalisableRange<float> gestureRange;
    
    //==============================================================================
    RangedAudioParameter& rangeLeftLow;
    RangedAudioParameter& rangeLeftHigh;
    RangedAudioParameter& rangeRightLow;
    RangedAudioParameter& rangeRightHigh;
    const Range<float> parameterMax;
    
    //==============================================================================
    ScopedPointer<Slider> leftLowSlider;
    ScopedPointer<Slider> leftHighSlider;
    ScopedPointer<Slider> rightLowSlider;
    ScopedPointer<Slider> rightHighSlider;
    
    ScopedPointer<Label> rangeLabelMinLeft;
    ScopedPointer<Label> rangeLabelMaxLeft;
    ScopedPointer<Label> rangeLabelMinRight;
    ScopedPointer<Label> rangeLabelMaxRight;
    
    //==============================================================================
    Colour tunerColour = Colour (0xff7c80de);

    DraggableObject objectBeingDragged = none;
    float previousCursorAngle = value;

    juce::Rectangle<int> sliderBounds;
    float sliderRadius;
    Point<int> sliderCentre;
    float startAngle;
    float endAngle;

    //==============================================================================
    PLUME::UI::OneRangeTunerLookAndFeel slidersLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TwoRangeTuner)
};