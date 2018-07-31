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

class OneRangeTuner:    public Component,
						public Tuner,
                        private Slider::Listener,
                        private Label::Listener
{
public:
    //==============================================================================
    OneRangeTuner(const float& val, const Range<float>& totRange, Range<float>& paramRange, const Range<float> paramMax, const String unit = "")
        :   Tuner (val, totRange, unit), parameterRange (paramRange), parameterMax (paramMax)
    {
        Component::setBounds (Tuner::getBounds());
        createSlider();
        createLabels();
    }

    ~OneRangeTuner()
    {
		rangeSlider = nullptr;
		rangeLabelMin = nullptr;
		rangeLabelMax = nullptr;
    }
    
    //==============================================================================
    void paint (Graphics& g) override
    {
		Tuner::paint(g); // Base class paint method
		
		// Sets the text style
        Colour fillColour = Colour (0xffffffff);
        g.setColour (fillColour);
        g.setFont (Font (12.0f, Font::plain).withTypefaceStyle ("Regular"));
        
        // Writes the "min" text
        int x = W*3/4,
            y = 0,
            width = W/4,
            height = H/4;
                
        String text (TRANS("Min"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
                    
        // Writes the "max" text
        y = H/2;
                
        text = String (TRANS("Max"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
        
    }

    void resized() override
    {
        Tuner::resized(); // Base class resized
        
        // Sets bounds and changes the slider and labels position
        Component::setBounds (Tuner::getBounds());
        rangeSlider->setBounds (Tuner::sliderPlacement.getStart(), H/3 - 6, Tuner::sliderPlacement.getLength(), 15);
        rangeLabelMin->setBounds (W*3/4+W/16, H/4, W/8, H/5);
        rangeLabelMax->setBounds (W*3/4+W/16, H*3/4, W/8, H/5);
        Component::repaint();
    }
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override
    {
        // checks that the string is numbers only (and unit)
        if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
        {
            if (lbl == rangeLabelMin)       lbl->setText (String (int (parameterRange.getStart())) + valueUnit, dontSendNotification);
            else if (lbl == rangeLabelMax)  lbl->setText (String (int (parameterRange.getEnd())) + valueUnit, dontSendNotification);

            return;
        }
        
        float val;
        
        // Gets the float value of the text 
        if (valueUnit != "" &&
            lbl->getText().endsWith(valueUnit)) val = lbl->getText().upToFirstOccurrenceOf(valueUnit, false, false).getFloatValue();
        else                                    val = lbl->getText().getFloatValue();
        
        if (val < parameterMax.getStart())    val = parameterMax.getStart();
        else if (val > parameterMax.getEnd()) val = parameterMax.getEnd();
        
        // Sets slider and labels accordingly
        if (lbl == rangeLabelMin)
        {
            if ( val > parameterRange.getEnd()) val = parameterRange.getEnd();
            
            parameterRange.setStart (val);
            rangeSlider->setMinValue (parameterRange.getStart(), dontSendNotification);
            lbl->setText (String (int (parameterRange.getStart())) + valueUnit, dontSendNotification);
        }
        else if (lbl == rangeLabelMax)
        {
            if ( val < parameterRange.getStart()) val = parameterRange.getStart();
            
            parameterRange.setEnd (val);
            rangeSlider->setMaxValue (parameterRange.getEnd(), dontSendNotification);
            lbl->setText (String (int (parameterRange.getEnd())) + valueUnit, dontSendNotification);
        }
    }
    
    void sliderValueChanged (Slider* sldr) override
    {
        // min value changed
		if (float (sldr->getMinValue()) != parameterRange.getStart())
		{
			parameterRange.setStart (float (sldr->getMinValue()));
		    rangeLabelMin->setText (String (int (parameterRange.getStart())) + valueUnit, dontSendNotification);
		}

		// max value changed
		else if (float (sldr->getMaxValue()) != parameterRange.getEnd())
		{
			parameterRange.setEnd (float (sldr->getMaxValue()));
		    rangeLabelMax->setText (String (int (parameterRange.getEnd())) + valueUnit, dontSendNotification);
		}
        
    }
    
private:
    void createSlider()
    {
        Tuner::addAndMakeVisible (rangeSlider = new Slider ("Range Slider"));
        
        // Slider style
        rangeSlider->setColour (Slider::thumbColourId, Colour (0xffe6e6e6));
        rangeSlider->setColour (Slider::trackColourId, Colour (0xffb7b7b7));
        rangeSlider->setColour (Slider::backgroundColourId, Colour (0xff101010));
	    rangeSlider->setSliderStyle(Slider::TwoValueHorizontal);
        rangeSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
	    
        // Slider values
        rangeSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
        rangeSlider->setMinValue (double (parameterRange.getStart()), dontSendNotification);
        rangeSlider->setMaxValue (double (parameterRange.getEnd()), dontSendNotification);
        
        rangeSlider->setBounds (Tuner::sliderPlacement.getStart(), H/3 - 7, Tuner::sliderPlacement.getLength(), 15);
        rangeSlider->addListener (this);
    }
    
    void createLabels()
    {
        Tuner::addAndMakeVisible (rangeLabelMin = new Label ("Min Label", TRANS (String(int(parameterRange.getStart())) + valueUnit)));
        Tuner::addAndMakeVisible (rangeLabelMax = new Label ("Max Label", TRANS (String(int(parameterRange.getEnd())) + valueUnit)));
        
        // LabelMin style
        rangeLabelMin->setEditable (true, false, false);
        rangeLabelMin->setFont (Font (13.0f, Font::plain));
        rangeLabelMin->setColour (Label::textColourId, Colour(0xffffffff));
        rangeLabelMin->setColour (Label::backgroundColourId, Colour(0xff000000));
        rangeLabelMin->setJustificationType (Justification::centred);
        
        // LabelMax style
        rangeLabelMax->setEditable (true, false, false);
        rangeLabelMax->setFont (Font (13.0f, Font::plain));
        rangeLabelMax->setColour (Label::textColourId, Colour(0xffffffff));
        rangeLabelMax->setColour (Label::backgroundColourId, Colour(0xff000000));
        rangeLabelMax->setJustificationType (Justification::centred);
        
        // Labels settings
        rangeLabelMin->setBounds (W*(3/4+1/16), H/4, W/8, H/4);
        rangeLabelMin->addListener (this);
        
        rangeLabelMax->setBounds (W*(3/4+1/16), H*3/4, W/8, H/4);
        rangeLabelMax->addListener (this);
    }
    
    //==============================================================================
    Range<float>& parameterRange;
    const Range<float> parameterMax;
    
    ScopedPointer<Slider> rangeSlider;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneRangeTuner)
};