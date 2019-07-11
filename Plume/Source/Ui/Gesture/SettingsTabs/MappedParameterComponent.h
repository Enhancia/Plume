/*
  ==============================================================================

    MappedParameterComponent.h
    Created: 24 Jul 2018 8:43:58pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Gesture/Gesture.h"

#ifndef W 
#define W Component::getWidth()
#endif

#ifndef H 
#define H Component::getHeight()
#endif

//==============================================================================
/*
*/
class MappedParameterComponent    : public Component,
                                    private Label::Listener,
									private Button::Listener,
                                    private Slider::Listener
{
public:
    //==============================================================================
    MappedParameterComponent (Gesture& gest, Gesture::MappedParameter& mappedParam,
                              const int id);
    ~MappedParameterComponent();

    //==============================================================================
    void paint (Graphics& g) override;
	void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void labelTextChanged (Label* lbl) override;
    void sliderValueChanged (Slider* sldr) override;

    //==============================================================================
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    //==============================================================================
    Gesture::MappedParameter& getMappedParameter();
    void updateDisplay();
    
private:
    //==============================================================================
    enum DraggableObject
    {
        none = 0,

        lowThumb,
        highThumb,
        middleArea
    };

    //==============================================================================
	void createLabels();
    void createSliders();

    //==============================================================================
    int getThumbY (DraggableObject thumb);
    void setLabelBounds (Label& labelToResize);

    //==============================================================================
    DraggableObject getObjectToDrag (const MouseEvent& e);
    void drawCursor (Graphics& g);
    void drawSliderBackground (Graphics& g);
    
    //==============================================================================
    Gesture& gesture;
    Gesture::MappedParameter& mappedParameter;
    const int paramId;
    DraggableObject objectBeingDragged = none;
    float lastValue = -1.0f;
    
    //==============================================================================
    bool allowDisplayUpdate = true;
    const Colour highlightColour;

    //==============================================================================
    ScopedPointer<ImageButton> closeButton;
    ScopedPointer<ImageButton> reverseButton;
    ScopedPointer<Label> valueLabel;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    ScopedPointer<Slider> lowSlider;
    ScopedPointer<Slider> highSlider;

    PLUME::UI::TestTunerLookAndFeel sliderLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappedParameterComponent)
};