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
    SymmetricalTuner(const float& val, const Range<float>& totRange, float& param, float paramMax,
                     const String unit = "", bool show = true)
        :   Tuner (val, totRange, unit, show), parameter (param), parameterMax (paramMax)
    {
        Component::setBounds (Tuner::getBounds());
        createSlider();
        createLabels();
    }

    ~SymmetricalTuner()
    {
		symmetricalSlider = nullptr;
		rangeLabel = nullptr;
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
            Colour fillColour = Colour (0xffffffff);

            g.setColour (fillColour);
            g.setFont (Font (12.0f, Font::plain).withTypefaceStyle ("Regular"));
            g.drawText (text, x, y, width, height,
                        Justification::centred, true);
        }
    }

    void resized() override
    {
        Tuner::resized(); // Base class resized
        
        // Sets bounds and changes the slider position
        Component::setBounds (Tuner::getBounds());
        symmetricalSlider->setBounds (Tuner::sliderPlacement.getStart(), H/3 - 6, Tuner::sliderPlacement.getLength(), 15);
        rangeLabel->setBounds (W*3/4+W/16, H/4, W/8, H/5);
        Component::repaint();
    }
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override
    {
        if (lbl == rangeLabel)
        {
            // checks that the string is numbers only
            if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
            {
                lbl->setText (String (int (parameter)), dontSendNotification);
                return;
            }
            
            float val;
        
            // Gets the float value of the text 
            if (valueUnit != "" && lbl->getText().endsWith(valueUnit)) val = lbl->getText().upToFirstOccurrenceOf(valueUnit, false, false).getFloatValue();
            else                                                       val = lbl->getText().getFloatValue();
        
            if (val < 0.0f) val = 0.0f;
            else if (val > parameterMax) val = parameterMax;
        
            parameter = val;
        
            lbl->setText (String (int (parameter)), dontSendNotification);
            symmetricalSlider->setMinValue(-parameter, dontSendNotification);
            symmetricalSlider->setMaxValue(parameter, dontSendNotification);
        }
    }
    
    void sliderValueChanged (Slider* sldr) override
    {
        // Locks the slider at center
        if (sldr->getMaxValue() < 0.0 || sldr->getMinValue() > 0.0)
        {
            symmetricalSlider->setMinValue(0.0, dontSendNotification);
            symmetricalSlider->setMaxValue(0.0, dontSendNotification);
            parameter = 0.0f;
        }
        
        // min value changed
		if (float(sldr->getMinValue()) != -parameter)
		{
			parameter = float(-sldr->getMinValue());
			sldr->setMaxValue(-sldr->getMinValue(), dontSendNotification);
		}

		// max value changed
		else if (float(sldr->getMaxValue()) != parameter)
		{
			parameter = float(sldr->getMaxValue());
			sldr->setMinValue(-sldr->getMaxValue(), dontSendNotification);
		}
        
		rangeLabel->setText (String (int (parameter)), dontSendNotification);
    }
    
private:
    void createSlider()
    {
        Tuner::addAndMakeVisible (symmetricalSlider = new Slider ("Symmetrical Slider"));
        
        // Slider style
        symmetricalSlider->setColour (Slider::thumbColourId, Colour (0xffe6e6e6));
        symmetricalSlider->setColour (Slider::trackColourId, Colour (0xffb7b7b7));
        symmetricalSlider->setColour (Slider::backgroundColourId, Colour (0xff101010));
	    symmetricalSlider->setSliderStyle(Slider::TwoValueHorizontal);
        symmetricalSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        
	    
        // Slider values
        symmetricalSlider->setRange (double (-parameterMax), double (parameterMax), 1.0);
        symmetricalSlider->setMinValue (double (-parameter), dontSendNotification);
        symmetricalSlider->setMaxValue (double (parameter), dontSendNotification);
        
        symmetricalSlider->setBounds (Tuner::sliderPlacement.getStart(), H/3 - 7, Tuner::sliderPlacement.getLength(), 15);
        symmetricalSlider->addListener (this);
    }
    
    void createLabels()
    {
        Tuner::addAndMakeVisible (rangeLabel = new Label ("Value Label", TRANS (String(int(parameter)))));
        
        // Label style
        rangeLabel->setEditable (true, false, false);
        rangeLabel->setFont (Font (13.0f, Font::plain));
        rangeLabel->setColour (Label::textColourId, Colour(0xffffffff));
        rangeLabel->setColour (Label::backgroundColourId, Colour(0xff000000));
        rangeLabel->setJustificationType (Justification::centred);
        
        rangeLabel->setBounds (W*(3/4+1/16), H/4, W/8, H/4);
        rangeLabel->addListener (this);
    }
    
    //==============================================================================
    float& parameter;
    const float parameterMax;
    
    ScopedPointer<Slider> symmetricalSlider;
    ScopedPointer<Label> rangeLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SymmetricalTuner)
};