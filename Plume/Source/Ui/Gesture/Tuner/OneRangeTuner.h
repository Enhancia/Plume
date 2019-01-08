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

#define W getWidth()
#define H getHeight()

class OneRangeTuner:    public Tuner,
                        private Slider::Listener,
                        private Label::Listener
{
public:
    //==============================================================================
    OneRangeTuner(const float& val, NormalisableRange<float> gestureRange,
                  RangedAudioParameter& rangeL, RangedAudioParameter& rangeH, const Range<float> paramMax,
                  const String unit = "", bool show = true);
    ~OneRangeTuner();
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    void updateComponents() override;
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void sliderValueChanged (Slider* sldr) override;
    
private:
    void createSlider();
    void createLabels();
    
    void setRangeLow (float value);
    void setRangeHigh (float value);
    
    float getRangeLow();
    float getRangeHigh();
    
    //==============================================================================
    const Range<float> parameterMax;
    RangedAudioParameter& rangeLow;
    RangedAudioParameter& rangeHigh;
    
    ScopedPointer<Slider> rangeSlider;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    
    //==============================================================================
    PLUME::OneRangeTunerLookAndFeel oneRangeTunerLookAndFeel;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneRangeTuner)
};