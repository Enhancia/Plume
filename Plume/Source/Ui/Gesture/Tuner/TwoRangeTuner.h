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

class TwoRangeTuner:    public Tuner,
                        private Slider::Listener,
                        private Label::Listener
{
public:
    //==============================================================================
    TwoRangeTuner(const float& val, const Range<float>& totRange,
                  Range<float>& paramRangeLeft, Range<float>& paramRangeRight, const Range<float> paramMax,
                  const String unit = "", bool show = true)
        :   Tuner (val, totRange, unit, show), parameterRangeLeft (paramRangeLeft), parameterRangeRight (paramRangeRight), parameterMax (paramMax)
    {
        Component::setBounds (Tuner::getBounds());
        createSliders();
        createLabels();
    }

    ~TwoRangeTuner()
    {
		rangeSliderLeft = nullptr;
		rangeSliderRight = nullptr;
		rangeLabelMinLeft = nullptr;
		rangeLabelMaxLeft = nullptr;
		rangeLabelMinRight = nullptr;
		rangeLabelMaxRight = nullptr;
    }
    
    //==============================================================================
    void paint (Graphics& g) override
    {
		Tuner::paint(g); // Base class paint method
		
		// Sets the text style
        Colour fillColour = Colour (0xffffffff);
        g.setColour (fillColour);
        g.setFont (Font (12.0f, Font::plain).withTypefaceStyle ("Regular"));
        
        // Writes the "left" text
        int x = W*3/4,
            y = 0,
            width = W/4,
            height = H/4;
                
        String text (TRANS("Left"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
                    
        // Writes the "right" text
        y = H/2;
                
        text = String (TRANS("Right"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
        
    }

    void resized() override
    {
        Tuner::resized(); // Base class resized()
        
        // Sets bounds and changes the slider and labels position
        Component::setBounds (Tuner::getBounds());
        
        // Sliders
        rangeSliderLeft->setBounds  (sliderPlacement.getStart(), H/3 - 6,
                                     sliderPlacement.getLength()/2 + 6, 15);
                                     
        rangeSliderRight->setBounds (sliderPlacement.getStart()+sliderPlacement.getLength()/2 - 6, H/3 - 6,
                                     sliderPlacement.getLength()/2 + 6, 15);
        
        // Labels
        rangeLabelMinLeft->setBounds  (W*3/4+2, H/4, W/8-4, H/5);
        rangeLabelMaxLeft->setBounds  (W*7/8+2, H/4, W/8-4, H/5);
        rangeLabelMinRight->setBounds (W*3/4+2, H*3/4, W/8-4, H/5);
        rangeLabelMaxRight->setBounds (W*7/8+2, H*3/4, W/8-4, H/5);
        repaint();
    }
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override
    {
        // checks that the string is numbers only (and unit)
        if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
        {
            if      (lbl == rangeLabelMinLeft)   lbl->setText (String (int (parameterRangeLeft.getStart())) + valueUnit, dontSendNotification);
            else if (lbl == rangeLabelMaxLeft)   lbl->setText (String (int (parameterRangeLeft.getEnd())) + valueUnit, dontSendNotification);
            else if (lbl == rangeLabelMinRight)  lbl->setText (String (int (parameterRangeRight.getStart())) + valueUnit, dontSendNotification);
            else if (lbl == rangeLabelMaxRight)  lbl->setText (String (int (parameterRangeRight.getEnd())) + valueUnit, dontSendNotification);

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
        if (lbl == rangeLabelMinLeft)   // Min left
        {
            if ( val > parameterRangeLeft.getEnd()) val = parameterRangeLeft.getEnd();
            
            parameterRangeLeft.setStart (val);
            rangeSliderLeft->setMinValue (parameterRangeLeft.getStart(), dontSendNotification);
            lbl->setText (String (int (parameterRangeLeft.getStart())) + valueUnit, dontSendNotification);
        }
        else if (lbl == rangeLabelMaxLeft)  // Max left
        {
            if ( val < parameterRangeLeft.getStart()) val = parameterRangeLeft.getStart();
            else if (val > (parameterMax.getStart() + parameterMax.getEnd())/2) val = (parameterMax.getStart() + parameterMax.getEnd())/2;
            
            parameterRangeLeft.setEnd (val);
            rangeSliderLeft->setMaxValue (parameterRangeLeft.getEnd(), dontSendNotification);
            lbl->setText (String (int (parameterRangeLeft.getEnd())) + valueUnit, dontSendNotification);
        }
        else if (lbl == rangeLabelMinRight)   // Min right
        {
            if ( val > parameterRangeRight.getEnd()) val = parameterRangeRight.getEnd();
            else if (val < (parameterMax.getStart() + parameterMax.getEnd())/2) val = (parameterMax.getStart() + parameterMax.getEnd())/2;
            
            parameterRangeRight.setStart (val);
            rangeSliderRight->setMinValue (parameterRangeRight.getStart(), dontSendNotification);
            lbl->setText (String (int (parameterRangeRight.getStart())) + valueUnit, dontSendNotification);
        }
        else if (lbl == rangeLabelMaxRight)  // Max right
        {
            if ( val < parameterRangeRight.getStart()) val = parameterRangeRight.getStart();
            
            parameterRangeRight.setEnd (val);
            rangeSliderRight->setMaxValue (parameterRangeRight.getEnd(), dontSendNotification);
            lbl->setText (String (int (parameterRangeRight.getEnd())) + valueUnit, dontSendNotification);
        }
    }
    
    void sliderValueChanged (Slider* sldr) override
    {
        if (sldr == rangeSliderLeft)
        {
            // min left value changed
		    if (float (sldr->getMinValue()) != parameterRangeLeft.getStart())
		    {
			    parameterRangeLeft.setStart (float (sldr->getMinValue()));
		        rangeLabelMinLeft->setText (String (int (parameterRangeLeft.getStart())) + valueUnit, sendNotification);
		    }

		    // max left value changed
		    else if (float (sldr->getMaxValue()) != parameterRangeLeft.getEnd())
		    {
			    parameterRangeLeft.setEnd (float (sldr->getMaxValue()));
		        rangeLabelMaxLeft->setText (String (int (parameterRangeLeft.getEnd())) + valueUnit, sendNotification);
		    }
        }
        
		else if (sldr == rangeSliderRight)
        {
		    // min right value changed
		    if (float (sldr->getMinValue()) != parameterRangeRight.getStart())
		    {
			    parameterRangeRight.setStart (float (sldr->getMinValue()));
		        rangeLabelMinRight->setText (String (int (parameterRangeRight.getStart())) + valueUnit, sendNotification);
		    }

		    // max right value changed
		    else if (float (sldr->getMaxValue()) != parameterRangeRight.getEnd())
		    {
			    parameterRangeRight.setEnd (float (sldr->getMaxValue()));
		        rangeLabelMaxRight->setText (String (int (parameterRangeRight.getEnd())) + valueUnit, sendNotification);
		    }
        }
    }
    
private:
    void createSliders()
    {
        Tuner::addAndMakeVisible (rangeSliderLeft = new Slider ("Range Slider Left"));
		Tuner::addAndMakeVisible (rangeSliderRight = new Slider ("Range Slider Right"));
        
        // Slider style
        //Left
	    rangeSliderLeft->setSliderStyle(Slider::TwoValueHorizontal);
        rangeSliderLeft->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        
        rangeSliderLeft->setLookAndFeel (&leftLookAndFeel);
        
        //Right
	    rangeSliderRight->setSliderStyle(Slider::TwoValueHorizontal);
        rangeSliderRight->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        
        rightLookAndFeel.setSliderLeft (false);
        rangeSliderRight->setLookAndFeel (&rightLookAndFeel);
	    
        // Slider values
        rangeSliderLeft->setRange (double (parameterMax.getStart()),
                                   double ((parameterMax.getStart() + parameterMax.getEnd())/2), 1.0);
        rangeSliderLeft->setMinValue (double (parameterRangeLeft.getStart()), dontSendNotification);
        rangeSliderLeft->setMaxValue (double (parameterRangeLeft.getEnd()), dontSendNotification);
        
        rangeSliderRight->setRange (double ((parameterMax.getStart() + parameterMax.getEnd())/2),
                                    double (parameterMax.getEnd()), 1.0);
        rangeSliderRight->setMaxValue (double (parameterRangeRight.getEnd()), dontSendNotification);
        rangeSliderRight->setMinValue (double (parameterRangeRight.getStart()), dontSendNotification);
        
        rangeSliderLeft->addListener (this);
		rangeSliderRight->addListener(this);
    }
    
    void createLabels()
    {
        Tuner::addAndMakeVisible (rangeLabelMinLeft  = new Label ("Min Left Label", TRANS (String(int(parameterRangeLeft.getStart())) + valueUnit)));
        Tuner::addAndMakeVisible (rangeLabelMaxLeft  = new Label ("Max Left Label", TRANS (String(int(parameterRangeLeft.getEnd())) + valueUnit)));
        Tuner::addAndMakeVisible (rangeLabelMinRight = new Label ("Min Right Label", TRANS (String(int(parameterRangeRight.getStart())) + valueUnit)));
        Tuner::addAndMakeVisible (rangeLabelMaxRight = new Label ("Max Right Label", TRANS (String(int(parameterRangeRight.getEnd())) + valueUnit)));
        
        // LabelMinLeft style
        rangeLabelMinLeft->setEditable (true, false, false);
        rangeLabelMinLeft->setFont (Font (13.0f, Font::plain));
        rangeLabelMinLeft->setJustificationType (Justification::centred);
        
        // LabelMaxLeft style
        rangeLabelMaxLeft->setEditable (true, false, false);
        rangeLabelMaxLeft->setFont (Font (13.0f, Font::plain));
        rangeLabelMaxLeft->setJustificationType (Justification::centred);
        
        // LabelMinRight style
        rangeLabelMinRight->setEditable (true, false, false);
        rangeLabelMinRight->setFont (Font (13.0f, Font::plain));
        rangeLabelMinRight->setJustificationType (Justification::centred);
        
        // LabelMaxRight style
        rangeLabelMaxRight->setEditable (true, false, false);
        rangeLabelMaxRight->setFont (Font (13.0f, Font::plain));
        rangeLabelMaxRight->setJustificationType (Justification::centred);
        
        // Labels settings
        rangeLabelMinLeft->addListener (this);
        rangeLabelMaxLeft->addListener (this);
        rangeLabelMinRight->addListener (this);
        rangeLabelMaxRight->addListener (this);
    }
    
    //==============================================================================
    Range<float>& parameterRangeLeft;
    Range<float>& parameterRangeRight;
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