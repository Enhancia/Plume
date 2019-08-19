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
    OneRangeTuner(const float& val, NormalisableRange<float> gestureRange,
                  RangedAudioParameter& rangeL, RangedAudioParameter& rangeH, const Range<float> paramMax,
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
    void setRangeLow (float value);
    void setRangeHigh (float value);
    
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
    const float& value;
    const NormalisableRange<float> gestureRange;

    //==============================================================================
    const Range<float> parameterMax;
    RangedAudioParameter& rangeLow;
    RangedAudioParameter& rangeHigh;
    
    ScopedPointer<Slider> lowSlider;
    ScopedPointer<Slider> highSlider;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    ScopedPointer<TextButton> minAngleButton;
    ScopedPointer<TextButton> maxAngleButton;
    
    //==============================================================================
    TunerStyle tunerStyle;

    DraggableObject objectBeingDragged = none;
    float previousCursorAngle = value;

    juce::Rectangle<int> sliderBounds;
    float sliderRadius;
    Point<int> sliderCentre;
    float startAngle;
    float endAngle;

    //==============================================================================
    PLUME::UI::OneRangeTunerLookAndFeel oneRangeTunerLookAndFeel;

    CriticalSection rangeUpdateLock;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneRangeTuner)
};