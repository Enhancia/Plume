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

#define W Component::getWidth()
#define H Component::getHeight()

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
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void sliderValueChanged (Slider* sldr) override;
    
private:
    void createSliders();
    void createLabels();
    
    void setRangeLeftLow (float value);
    void setRangeLeftHigh (float value);
    void setRangeRightLow (float value);
    void setRangeRightHigh (float value);
    
    float getRangeLeftLow();
    float getRangeLeftHigh();
    float getRangeRightLow();
    float getRangeRightHigh();
    
    //==============================================================================
    RangedAudioParameter& rangeLeftLow;
    RangedAudioParameter& rangeLeftHigh;
    RangedAudioParameter& rangeRightLow;
    RangedAudioParameter& rangeRightHigh;
    const Range<float> parameterMax;
    
    //==============================================================================
    ScopedPointer<Slider> rangeSliderLeft;
    ScopedPointer<Slider> rangeSliderRight;
    ScopedPointer<Label> rangeLabelMinLeft;
    ScopedPointer<Label> rangeLabelMaxLeft;
    ScopedPointer<Label> rangeLabelMinRight;
    ScopedPointer<Label> rangeLabelMaxRight;
    
    //==============================================================================
    TwoRangeTunerLookAndFeel leftLookAndFeel;
    TwoRangeTunerLookAndFeel rightLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TwoRangeTuner)
};