/*
  ==============================================================================

    SymetricTuner.h
    Created: 19 Jul 2018 2:31:30pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#define W getWidth()
#define H getHeight()

class SymetricTuner:    public Tuner,
                        private Slider::Listener,
                        private Label::Listener
{
public:
    //==============================================================================
    SymetricTuner(const float& val, const Range<float>& totRange, const String unit = "", int freqHz = 60, float& param)
        :   Tuner (val, totRange, unit, freqHz), parameter (param)
    {
        createSliders();
    }

    ~Tuner()
    {
    }
    
    //==============================================================================
    void paint (Graphics& g) override
    {
        Tuner::paint(g);
        
        {
            // Writes the "range" text
            int x = W*3/4,
                y = 0,
                width = W/4,
                height = H/4;
                
            String text (TRANS("Range"));
            Colour fillColour = Colours::white;

            g.setColour (fillColour);
            g.setFont (Font (12.0f, Font::plain).withTypefaceStyle ("Regular"));
            g.drawText (text, x, y, width, height,
                        Justification::centred, true);
        }
    }

    void resized() override
    {
        
    }
    
    void createSliders()
    {
        // leftSlider
        addAndMakeVisible (leftSlider = new Slider ("Vibrato Slider"));
        //vibratoSlider->setColour (Slider::backgroundColourId, Colour (0xffc0c0c0));
        leftSlider->setColour (Slider::thumbColourId, Colour (0xff909090));
	    leftSlider->setSliderStyle(Slider::LinearHorizontal);
        leftSlider->setRange(0, 500, 1);
        leftSlider->setValue(350, dontSendNotification);
        leftSlider->setTextBoxStyle (Slider::TextBoxRight, true, 40, 40);
        leftSlider->setTextBoxIsEditable (true);
        leftSlider->setBounds(150, 480, 400, 20);
        leftSlider->addListener (this);
    }
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override
    {
    }
    
    void sliderValueChanged (Slider* sldr) override
    {
    }
    
private:
    //==============================================================================
    float& parameter;
    ScopedPointer<Slider> leftSlider;
    ScopedPointer<Slider> rightSlider;
    ScopedPointer<Label> rangeLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SymetricTuner)
};