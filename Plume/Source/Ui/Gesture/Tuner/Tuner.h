/*
  ==============================================================================

    Tuner.h
    Created: 18 Jul 2018 3:58:44pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

#define W getWidth()
#define H getHeight()
//==============================================================================
/*
*/
class Tuner    : public Component
{
public:
    //==============================================================================
	Tuner(const float& val, NormalisableRange<float> gestRange, const Range<float> dispRange, const String unit = "", bool show = true);

	~Tuner();

    //==============================================================================
	virtual void paint(Graphics& g) override;

	virtual void resized() override;
    
    //==============================================================================
	void updateDisplay();
    
	virtual void updateComponents() = 0;
    
protected:
    Range<int> sliderPlacement;
    const String valueUnit;
    
private:
    //==============================================================================
	void drawCursor (Graphics& g);
    
    //==============================================================================
    const float& value;
    const Range<float> displayRange;
    NormalisableRange<float> gestureRange;
    const bool showValue;
    
    int yCursor;
    
    //==============================================================================
    //ScopedPointer<Path> displayTriangle;
    ScopedPointer<Label> valueLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tuner)
};


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

class OneRangeTuner:    public Tuner,
                        private Slider::Listener,
                        private Label::Listener
{
public:
    //==============================================================================
    OneRangeTuner(const float& val, NormalisableRange<float>& gestureRange,
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
    OneRangeTunerLookAndFeel oneRangeTunerLookAndFeel;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneRangeTuner)
};


class TwoRangeTuner:    public Tuner,
                        private Slider::Listener,
                        private Label::Listener
{
public:
    //==============================================================================
    TwoRangeTuner(const float& val, const NormalisableRange<float>& gestureRange,
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