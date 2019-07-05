/*
  ==============================================================================

    VibratoTestTuner.h
    Created: 4 Jul 2019 1:44:31pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/Gesture/Tuner/Tuner.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"
#include "Gesture/Vibrato.h"

class VibratoTestTuner:    public Tuner,
                           private Slider::Listener,
                           protected Label::Listener
{
public:
    //==============================================================================
    VibratoTestTuner (Vibrato& vib);
    ~VibratoTestTuner();
    
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
    
private:
    //==============================================================================
	VibratoTestTuner (const float& val, NormalisableRange<float> gestRange,
					  const float& vibratoIntensity, float maxIntens,
    				  RangedAudioParameter& gain, const Range<float> gainMax,
    				  RangedAudioParameter& thresh, const Range<float> threshMax);

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
    float& getIntensityReference();

    float getGain();
    float getThreshold();
    
    //==============================================================================
    const float& value;
    float lastValue = -1.0f;

    const NormalisableRange<float> gestureRange;
    const float& intensity;
    const float maxIntensity;
	float lastIntensity = -1.0f;
    float smoothIntensity = intensity;
    float incrementalSmoothFactor = 1.0f;

    RangedAudioParameter& gain;
    RangedAudioParameter& threshold;
    const Range<float> parameterMaxGain;
    const Range<float> parameterMaxThreshold;

    
    ScopedPointer<Slider> gainSlider;
    ScopedPointer<Slider> thresholdSlider;
    ScopedPointer<Label> gainLabel;
    ScopedPointer<Label> thresholdLabel;
    
    PLUME::UI::TestTunerLookAndFeel vibratoTunerLookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoTestTuner)
};