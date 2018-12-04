/*
  ==============================================================================

    Tuner.cpp
    Created: 30 Nov 2018 4:16:50pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Tuner/TwoRangeTuner.h"

//==============================================================================
TwoRangeTuner::TwoRangeTuner(const float& val, const NormalisableRange<float>& gestureRange,
                RangedAudioParameter& rangeLL, RangedAudioParameter& rangeLH,
                RangedAudioParameter& rangeRL, RangedAudioParameter& rangeRH,
                const Range<float> paramMax,
                const String unit, bool show)
    :   Tuner (val, gestureRange, paramMax, unit, show),
        rangeLeftLow (rangeLL), rangeLeftHigh (rangeLH),
        rangeRightLow (rangeRL), rangeRightHigh (rangeRH),
        parameterMax (paramMax)
{
    Component::setBounds (Tuner::getBounds());
    createSliders();
    createLabels();
}

TwoRangeTuner::~TwoRangeTuner()
{
	rangeSliderLeft = nullptr;
	rangeSliderRight = nullptr;
	rangeLabelMinLeft = nullptr;
	rangeLabelMaxLeft = nullptr;
	rangeLabelMinRight = nullptr;
	rangeLabelMaxRight = nullptr;
}
    
//==============================================================================
void TwoRangeTuner::paint (Graphics& g)
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

void TwoRangeTuner::resized()
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
    
void TwoRangeTuner::updateComponents()
{
    if (rangeLeftLow.getValue() < rangeLeftHigh.getValue() && rangeSliderLeft->getThumbBeingDragged() == -1)
    {
        // Sets slider value
        rangeSliderLeft->setMinValue (double (getRangeLeftLow()), dontSendNotification);
        rangeSliderLeft->setMaxValue (double (getRangeLeftHigh()), dontSendNotification);
        
        // Sets label text
        if (!(rangeLabelMinLeft->isBeingEdited()))
		{
		    rangeLabelMinLeft->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
		}
		if (!(rangeLabelMaxLeft->isBeingEdited()))
		{
            rangeLabelMaxLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
		}
    }
        
    if (rangeRightLow.getValue() < rangeRightHigh.getValue() && rangeSliderRight->getThumbBeingDragged() == -1)
    {
        // Sets slider value
        rangeSliderRight->setMaxValue (double (getRangeRightHigh()), dontSendNotification);
        rangeSliderRight->setMinValue (double (getRangeRightLow()), dontSendNotification);
        
        // Sets label text
        if (!(rangeLabelMinRight->isBeingEdited()))
		{
            rangeLabelMinRight->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
		}
        if (!(rangeLabelMaxRight->isBeingEdited()))
		{
            rangeLabelMaxRight->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
		}
    }
        
    //repaint();
}
    
//==============================================================================
void TwoRangeTuner::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only (and unit)
    if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
    {
        if      (lbl == rangeLabelMinLeft)   lbl->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMaxLeft)   lbl->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMinRight)  lbl->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMaxRight)  lbl->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);

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
        if ( val > getRangeLeftHigh()) val = getRangeLeftHigh();
            
        setRangeLeftLow (val);
        rangeSliderLeft->setMinValue (getRangeLeftLow(), dontSendNotification);
        lbl->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxLeft)  // Max left
    {
        if ( val < getRangeLeftLow()) val = getRangeLeftLow();
        else if (val > (parameterMax.getStart() + parameterMax.getEnd())/2) val = (parameterMax.getStart() + parameterMax.getEnd())/2;
            
        setRangeLeftHigh (val);
        rangeSliderLeft->setMaxValue (getRangeLeftHigh(), dontSendNotification);
        lbl->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMinRight)   // Min right
    {
        if ( val > getRangeRightHigh()) val = getRangeRightHigh();
        else if (val < (parameterMax.getStart() + parameterMax.getEnd())/2) val = (parameterMax.getStart() + parameterMax.getEnd())/2;
            
        setRangeRightLow (val);
        rangeSliderRight->setMinValue (getRangeRightLow(), dontSendNotification);
        lbl->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxRight)  // Max right
    {
        if ( val < getRangeRightLow()) val = getRangeRightLow();
            
        setRangeRightHigh (val);
        rangeSliderRight->setMaxValue (getRangeRightHigh(), dontSendNotification);
        lbl->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
    }
}
    
void TwoRangeTuner::sliderValueChanged (Slider* sldr)
{
    if (sldr == rangeSliderLeft)
    {
		// min left value changed
		if (sldr->getThumbBeingDragged() == 1)
		{
		    setRangeLeftLow (float (sldr->getMinValue()));
		    rangeLabelMinLeft->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
		        
		    // in case the other thumb is dragged along..
			if (rangeLeftLow.getValue() > rangeLeftHigh.getValue())
			{
				setRangeLeftHigh (float(sldr->getMaxValue()));
				rangeLabelMaxLeft->setText(String(int(getRangeLeftHigh())) + valueUnit, dontSendNotification);
			}
					
		}

		// max left value changed
		else if (sldr->getThumbBeingDragged() == 2)
		{
			setRangeLeftHigh (float (sldr->getMaxValue()));
		    rangeLabelMaxLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
		        
		    // in case the other thumb is dragged along..
			if (rangeLeftLow.getValue() > rangeLeftHigh.getValue())
			{
				setRangeLeftLow (float(sldr->getMinValue()));
				rangeLabelMinLeft->setText(String(int(getRangeLeftLow())) + valueUnit, dontSendNotification);
			}
		}
    }
        
	else if (sldr == rangeSliderRight)
    {
		// min right value changed
		if (sldr->getThumbBeingDragged() == 1)
		{
			setRangeRightLow (float (sldr->getMinValue()));
		    rangeLabelMinRight->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
		        
		    // in case the other thumb is dragged along..
			if (rangeRightLow.getValue() > rangeRightHigh.getValue())
			{
				setRangeRightHigh (float(sldr->getMaxValue()));
				rangeLabelMaxRight->setText(String(int(getRangeRightHigh())) + valueUnit, dontSendNotification);
			}
		}

		// max right value changed
		else if (sldr->getThumbBeingDragged() == 2)
		{
			setRangeRightHigh (float (sldr->getMaxValue()));
		    rangeLabelMaxRight->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
		        
		    // in case the other thumb is dragged along..
			if (rangeRightLow.getValue() > rangeRightHigh.getValue())
			{
				setRangeRightLow (float(sldr->getMinValue()));
				rangeLabelMinRight->setText(String(int(getRangeRightLow())) + valueUnit, dontSendNotification);
			}
		}
    }
}
    
void TwoRangeTuner::createSliders()
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
    rangeSliderLeft->setMinValue (double (getRangeLeftLow()), dontSendNotification);
    rangeSliderLeft->setMaxValue (double (getRangeLeftHigh()), dontSendNotification);
        
    rangeSliderRight->setRange (double ((parameterMax.getStart() + parameterMax.getEnd())/2),
                                double (parameterMax.getEnd()), 1.0);
    rangeSliderRight->setMaxValue (double (getRangeRightHigh()), dontSendNotification);
    rangeSliderRight->setMinValue (double (getRangeRightLow()), dontSendNotification);
        
    rangeSliderLeft->addListener (this);
	rangeSliderRight->addListener(this);
}
    
void TwoRangeTuner::createLabels()
{
    Tuner::addAndMakeVisible (rangeLabelMinLeft  = new Label ("Min Left Label",
                                                                TRANS (String (int(getRangeLeftLow())) + valueUnit)));
    Tuner::addAndMakeVisible (rangeLabelMaxLeft  = new Label ("Max Left Label",
                                                                TRANS (String(int(getRangeLeftHigh())) + valueUnit)));
    Tuner::addAndMakeVisible (rangeLabelMinRight = new Label ("Min Right Label",
                                                                TRANS (String(int(getRangeRightLow())) + valueUnit)));
    Tuner::addAndMakeVisible (rangeLabelMaxRight = new Label ("Max Right Label",
                                                                TRANS (String(int(getRangeRightHigh())) + valueUnit)));
        
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
    
void TwoRangeTuner::setRangeLeftLow (float value)
{
    rangeLeftLow.beginChangeGesture();
    rangeLeftLow.setValueNotifyingHost (rangeLeftLow.convertTo0to1 (value));
    rangeLeftLow.endChangeGesture();
}
    
void TwoRangeTuner::setRangeLeftHigh (float value)
{
    rangeLeftHigh.beginChangeGesture();
    rangeLeftHigh.setValueNotifyingHost (rangeLeftHigh.convertTo0to1 (value));
    rangeLeftHigh.endChangeGesture();
}
    
void TwoRangeTuner::setRangeRightLow (float value)
{
    rangeRightLow.beginChangeGesture();
    rangeRightLow.setValueNotifyingHost (rangeRightLow.convertTo0to1 (value));
    rangeRightLow.endChangeGesture();
}
    
void TwoRangeTuner::setRangeRightHigh (float value)
{
    rangeRightHigh.beginChangeGesture();
    rangeRightHigh.setValueNotifyingHost (rangeRightHigh.convertTo0to1 (value));
    rangeRightHigh.endChangeGesture();
}
    
float TwoRangeTuner::getRangeLeftLow()
{
    return rangeLeftLow.convertFrom0to1 (rangeLeftLow.getValue());
}
    
float TwoRangeTuner::getRangeLeftHigh()
{
    return rangeLeftHigh.convertFrom0to1 (rangeLeftHigh.getValue());
}
    
float TwoRangeTuner::getRangeRightLow()
{
    return rangeRightLow.convertFrom0to1 (rangeRightLow.getValue());
}
    
float TwoRangeTuner::getRangeRightHigh()
{
    return rangeRightHigh.convertFrom0to1 (rangeRightHigh.getValue());
}