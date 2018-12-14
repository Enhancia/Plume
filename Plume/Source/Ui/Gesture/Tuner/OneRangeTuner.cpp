/*
  ==============================================================================

    Tuner.cpp
    Created: 30 Nov 2018 4:16:50pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Tuner/OneRangeTuner.h"


OneRangeTuner::OneRangeTuner(const float& val, NormalisableRange<float> gestureRange,
                RangedAudioParameter& rangeL, RangedAudioParameter& rangeH, const Range<float> paramMax,
                const String unit, bool show)
    : Tuner(val, gestureRange, paramMax, unit, show),
		rangeLow (rangeL), rangeHigh (rangeH), parameterMax (paramMax)
{
    createSlider();
    createLabels();
}

OneRangeTuner::~OneRangeTuner()
{
	rangeSlider = nullptr;
	rangeLabelMin = nullptr;
	rangeLabelMax = nullptr;
}
    
//==============================================================================
void OneRangeTuner::paint (Graphics& g)
{
	Tuner::paint(g); // Base class paint method
		
	// Sets the text style
    Colour fillColour = Colour (0xffffffff);
    g.setColour (fillColour);
    g.setFont (Font (PLUME::UI::font, 12.0f, Font::plain).withTypefaceStyle ("Regular"));
        
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

void OneRangeTuner::resized()
{
    Tuner::resized(); // Base class resized
        
    // Sets bounds and changes the slider and labels position
    setBounds (Tuner::getBounds());
    rangeSlider->setBounds (Tuner::sliderPlacement.getStart(), H/3 - 6, Tuner::sliderPlacement.getLength(), 15);
    rangeLabelMin->setBounds (W*3/4+W/16, H/4, W/8, H/5);
    rangeLabelMax->setBounds (W*3/4+W/16, H*3/4, W/8, H/5);
    repaint();
}
    
void OneRangeTuner::updateComponents()
{
    if (rangeLow.getValue() < rangeHigh.getValue() && rangeSlider->getThumbBeingDragged() == -1)
    {
        // Sets slider value
        rangeSlider->setMinValue (double (getRangeLow()), dontSendNotification);
        rangeSlider->setMaxValue (double (getRangeHigh()), dontSendNotification);
        
        // Sets label text
		if (!(rangeLabelMin->isBeingEdited()))
		{
		    rangeLabelMin->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
		}
		    
		if (!(rangeLabelMax->isBeingEdited()))
		{
		    rangeLabelMax->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);
		}
		
		//repaint();
    }
}
    
//==============================================================================
void OneRangeTuner::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only (and unit)
    if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
    {
        if (lbl == rangeLabelMin)       lbl->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMax)  lbl->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);

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
        if ( val > getRangeHigh()) val = getRangeHigh();
            
        setRangeLow (val);
        rangeSlider->setMinValue (getRangeLow(), dontSendNotification);
        lbl->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMax)
    {
        if ( val < getRangeLow()) val = getRangeLow();
            
        setRangeHigh (val);
        rangeSlider->setMaxValue (getRangeHigh(), dontSendNotification);
        lbl->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);
    }
}
    
void OneRangeTuner::sliderValueChanged (Slider* sldr)
{
    // min value changed by user
	if (sldr->getThumbBeingDragged() == 1)
	{
		setRangeLow (float (sldr->getMinValue()));
		rangeLabelMin->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
		    
		// in case the other thumb is dragged along..
		if (rangeLow.getValue() > rangeHigh.getValue())
		{
		    setRangeHigh (float (sldr->getMaxValue()));
			rangeLabelMax->setText (String (float (sldr->getMaxValue())) + valueUnit, dontSendNotification);
		}
	}

	// max value changed by user
	else if (sldr->getThumbBeingDragged() == 2)
	{
		setRangeHigh (float (sldr->getMaxValue()));
		rangeLabelMax->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);
		    
		// in case the other thumb is dragged along..
		if (rangeLow.getValue() > rangeHigh.getValue())
		{
		    setRangeLow (float (sldr->getMinValue()));
			rangeLabelMin->setText (String (float (sldr->getMinValue())) + valueUnit, dontSendNotification);
		}
	}
}
    

void OneRangeTuner::createSlider()
{
    Tuner::addAndMakeVisible (rangeSlider = new Slider ("Range Slider"));
        
    // Slider style
	rangeSlider->setSliderStyle(Slider::TwoValueHorizontal);
    rangeSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    rangeSlider->setLookAndFeel (&oneRangeTunerLookAndFeel);
	    
    // Slider values
    rangeSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
    rangeSlider->setMinValue (double (getRangeLow()), dontSendNotification);
    rangeSlider->setMaxValue (double (getRangeHigh()), dontSendNotification);
        
    rangeSlider->setBounds (Tuner::sliderPlacement.getStart(), H/3 - 7, Tuner::sliderPlacement.getLength(), 15);
    rangeSlider->addListener (this);
}
    
void OneRangeTuner::createLabels()
{
    Tuner::addAndMakeVisible (rangeLabelMin = new Label ("Min Label", TRANS (String(int(getRangeLow())) + valueUnit)));
    Tuner::addAndMakeVisible (rangeLabelMax = new Label ("Max Label", TRANS (String(int(getRangeHigh())) + valueUnit)));
        
    // LabelMin style
    rangeLabelMin->setEditable (true, false, false);
    rangeLabelMin->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMin->setJustificationType (Justification::centred);
        
    // LabelMax style
    rangeLabelMax->setEditable (true, false, false);
    rangeLabelMax->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMax->setJustificationType (Justification::centred);
        
    // Labels settings
    rangeLabelMin->setBounds (W*(3/4+1/16), H/4, W/8, H/4);
    rangeLabelMin->addListener (this);
        
    rangeLabelMax->setBounds (W*(3/4+1/16), H*3/4, W/8, H/4);
    rangeLabelMax->addListener (this);
}
    
void OneRangeTuner::setRangeLow (float value)
{
    rangeLow.beginChangeGesture();
    rangeLow.setValueNotifyingHost (rangeLow.convertTo0to1 (value));
    rangeLow.endChangeGesture();
}
    
void OneRangeTuner::setRangeHigh (float value)
{
    rangeHigh.beginChangeGesture();
    rangeHigh.setValueNotifyingHost (rangeHigh.convertTo0to1 (value));
    rangeHigh.endChangeGesture();
}
    
float OneRangeTuner::getRangeLow()
{
    return rangeLow.convertFrom0to1 (rangeLow.getValue());
}
    
float OneRangeTuner::getRangeHigh()
{
    return rangeHigh.convertFrom0to1 (rangeHigh.getValue());
}