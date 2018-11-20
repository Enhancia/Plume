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
    SymmetricalTuner(const float& val, const Range<float>& totRange, RangedAudioParameter& param, float paramMax,
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
        setBounds (Tuner::getBounds());
        symmetricalSlider->setBounds (Tuner::sliderPlacement.getStart(), H/3 - 6, Tuner::sliderPlacement.getLength(), 15);
        rangeLabel->setBounds (W*3/4+W/16, H/4, W/8, H/5);
        repaint();
    }
    
    void updateComponents() override
    {
        // Sets slider value
        symmetricalSlider->setMinValue (double (-parameter.convertFrom0to1 (parameter.getValue())), dontSendNotification);
        symmetricalSlider->setMaxValue (double (parameter.convertFrom0to1 (parameter.getValue())), dontSendNotification);
        
        // Sets label text
        rangeLabel->setText (String (int (parameter.convertFrom0to1 (parameter.getValue()))), dontSendNotification);
        
        repaint();
    }
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override
    {
        if (lbl == rangeLabel)
        {
            // checks that the string is numbers only
            if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
            {
                lbl->setText (String (int (parameter.convertFrom0to1 (parameter.getValue()))), dontSendNotification);
                return;
            }
            
            float val;
        
            // Gets the float value of the text 
            if (valueUnit != "" && lbl->getText().endsWith(valueUnit)) val = lbl->getText().upToFirstOccurrenceOf(valueUnit, false, false).getFloatValue();
            else                                                       val = lbl->getText().getFloatValue();
        
            if (val < 0.0f) val = 0.0f;
            else if (val > parameterMax) val = parameterMax;
        
            setParameter (val);
        
            lbl->setText (String (val), dontSendNotification);
            symmetricalSlider->setMinValue (-val, dontSendNotification);
            symmetricalSlider->setMaxValue (val, dontSendNotification);
        }
    }
    
    void sliderValueChanged (Slider* sldr) override
    {
        // Locks the slider at center
        if (sldr->getMaxValue() < 0.0 || sldr->getMinValue() > 0.0)
        {
            symmetricalSlider->setMinValue(0.0, dontSendNotification);
            symmetricalSlider->setMaxValue(0.0, dontSendNotification);
            setParameter (0.0f);
        }
        
        // min value changed
		if (float(sldr->getMinValue()) != -parameter.convertFrom0to1 (parameter.getValue()))
		{
			setParameter (float(-sldr->getMinValue()));
			sldr->setMaxValue(-sldr->getMinValue(), dontSendNotification);
		}

		// max value changed
		else if (float(sldr->getMaxValue()) != parameter.convertFrom0to1 (parameter.getValue()))
		{
			setParameter (float(sldr->getMaxValue()));
			sldr->setMinValue(-sldr->getMaxValue(), dontSendNotification);
		}
        
		rangeLabel->setText (String (int (parameter.convertFrom0to1 (parameter.getValue()))), dontSendNotification);
    }
    
private:
    void createSlider()
    {
        Tuner::addAndMakeVisible (symmetricalSlider = new Slider ("Symmetrical Slider"));
        
        // Slider style
	    symmetricalSlider->setSliderStyle(Slider::TwoValueHorizontal);
        symmetricalSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        
	    
        // Slider values
        symmetricalSlider->setRange (double (-parameterMax), double (parameterMax), 1.0);
        symmetricalSlider->setMinValue (double (-parameter.convertFrom0to1 (parameter.getValue())), dontSendNotification);
        symmetricalSlider->setMaxValue (double (parameter.convertFrom0to1 (parameter.getValue())), dontSendNotification);
        
        symmetricalSlider->setBounds (Tuner::sliderPlacement.getStart(), H/3 - 7, Tuner::sliderPlacement.getLength(), 15);
        symmetricalSlider->addListener (this);
    }
    
    void createLabels()
    {
        Tuner::addAndMakeVisible (rangeLabel = new Label ("Value Label", TRANS (String(int(parameter.convertFrom0to1 (parameter.getValue()))))));
        
        // Label style
        rangeLabel->setEditable (true, false, false);
        rangeLabel->setFont (Font (13.0f, Font::plain));
        rangeLabel->setJustificationType (Justification::centred);
        
        rangeLabel->setBounds (W*(3/4+1/16), H/4, W/8, H/4);
        rangeLabel->addListener (this);
    }
    
    void setParameter (float value)
    {
        parameter.beginChangeGesture();
        parameter.setValueNotifyingHost (parameter.convertTo0to1 (value));
        parameter.endChangeGesture();
    }
    
    //==============================================================================
    RangedAudioParameter& parameter;
    const float parameterMax;
    
    ScopedPointer<Slider> symmetricalSlider;
    ScopedPointer<Label> rangeLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SymmetricalTuner)
};