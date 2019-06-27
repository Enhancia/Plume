/*
  ==============================================================================

    TestTuner.h
    Created: 21 Jun 2019 9:50:24am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/Gesture/Tuner/Tuner.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

class TestTuner:    public Tuner,
                    private Slider::Listener,
                    private Label::Listener
{
public:
    //==============================================================================
    TestTuner(const float& val, NormalisableRange<float> gestureRange,
                  RangedAudioParameter& rangeL, RangedAudioParameter& rangeH, const Range<float> paramMax,
                  const String unit = "", bool show = true);
    ~TestTuner();
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void updateComponents() override;
    void updateDisplay() override {}
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void sliderValueChanged (Slider* sldr) override;


    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    MouseCursor getMouseCursor() override;
    
private:
    void createSliders();
    void resizeSliders();
    void createLabels();
    
    void setRangeLow (float value);
    void setRangeHigh (float value);
    
    float getRangeLow();
    float getRangeHigh();

    enum DraggableObject
    {
        none = -1,
        lowThumb,
        highThumb,
        middleArea
    };

    double getAngleFromMouseEventRadians (const MouseEvent& e);
    double getThumbAngleRadians (const DraggableObject thumb);

    DraggableObject getObjectToDrag (const MouseEvent& e);

    void drawTunerSliderBackground (Graphics& g);
    void updateLabelBounds (Label* labelToUpdate);
    
    //==============================================================================
    const float& value;
    const NormalisableRange<float> gestureRange;

    //==============================================================================
    const Range<float> parameterMax;
    RangedAudioParameter& rangeLow;
    RangedAudioParameter& rangeHigh;
    
    juce::Rectangle<int> sliderBounds;
    ScopedPointer<Slider> lowSlider;
    ScopedPointer<Slider> highSlider;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    
    //==============================================================================
    DraggableObject objectBeingDragged = none;
    //==============================================================================
    PLUME::UI::TestTunerLookAndFeel testTunerLookAndFeel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestTuner)
};