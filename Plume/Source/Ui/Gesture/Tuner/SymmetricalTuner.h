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

class SymmetricalTuner:    public Tuner,
                           private Slider::Listener,
                           protected Label::Listener
{
public:
    //==============================================================================
    SymmetricalTuner(const float& val, NormalisableRange<float> gestureRange, RangedAudioParameter& param, float paramMax,
                     const String unit = "", bool show = true);
    ~SymmetricalTuner();
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    void updateComponents() override;
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void sliderValueChanged (Slider* sldr) override;
    
private:
    //==============================================================================
    void createSlider();
    void createLabels();
    void setParameter (float value);
    
    //==============================================================================
    RangedAudioParameter& parameter;
    const float parameterMax;
    
    ScopedPointer<Slider> symmetricalSlider;
    ScopedPointer<Label> rangeLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SymmetricalTuner)
};