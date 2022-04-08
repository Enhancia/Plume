/*
  ==============================================================================

    VibratoTuner.h
    Created: 4 Jul 2019 1:44:31pm
    Author:  Enhancia Dev

  ==============================================================================
*/ 

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Gesture/Vibrato.h"
#include "../../LookAndFeel/PlumeLookAndFeel.h"
#include "Tuner.h"

class VibratoTuner:    public Tuner,
                       private Slider::Listener,
                       private Label::Listener
{
public:
    //==============================================================================
    VibratoTuner (Vibrato& vib);
    ~VibratoTuner();
    
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    void updateComponents() override;
    void updateDisplay() override;
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;
    void editorHidden (Label* lbl, TextEditor& ted) override;
    void sliderValueChanged (Slider* sldr) override;

    //==============================================================================
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    void setColour (const Colour newColour) override;
    
private:
    //==============================================================================
	VibratoTuner (const std::atomic<float>& val, NormalisableRange<float> gestRange,
                  const std::atomic<float>& vibratoIntensity, NormalisableRange<float> intRange,
                  float& vibGain, const NormalisableRange<float> gainMax,
                  float& thresh, const NormalisableRange<float> threshMax);

    //==============================================================================
    void createSliders();
    void createLabels();
    void updateLabelBounds (Label* labelToUpdate);

    //==============================================================================
    void drawValueCursor (Graphics& g);
    void drawIntensityCursor (Graphics& g);
    void setThresholdSliderColour();
    void computeSmoothIntensity (float smoothnessRamp);

    //==============================================================================
    void setGain (float value);
    void setThreshold (float value);
    std::atomic<float>& getIntensityReference();

    float getGain();
    float getThreshold();
    float getIntensity();
    
    //==============================================================================
    const std::atomic<float>& value;
    float lastValue = -1.0f;
    juce::Rectangle<int> tunerArea;
    const NormalisableRange<float> gestureRange;

    const std::atomic<float>& intensity;
	float lastIntensity = -1.0f;
    const NormalisableRange<float> intensityRange;
    float smoothIntensity = intensity;
    float incrementalSmoothFactor = 1.0f;

    float& gain;
    float& threshold;
    const NormalisableRange<float> parameterMaxGain;
    const NormalisableRange<float> parameterMaxThreshold;

    
    std::unique_ptr<Slider> gainSlider;
    std::unique_ptr<Slider> thresholdSlider;
    std::unique_ptr<Label> gainLabel;
    std::unique_ptr<Label> thresholdLabel;
    
    PLUME::UI::TestTunerLookAndFeel vibratoTunerLookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoTuner)
};