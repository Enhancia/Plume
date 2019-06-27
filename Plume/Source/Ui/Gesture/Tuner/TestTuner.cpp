/*
  ==============================================================================

    TestTuner.cpp
    Created: 21 Jun 2019 9:50:24am
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Tuner/TestTuner.h"


TestTuner::TestTuner(const float& val, NormalisableRange<float> gestRange,
                RangedAudioParameter& rangeL, RangedAudioParameter& rangeH, const Range<float> paramMax,
                const String unit, bool show)
    : Tuner(val, gestRange, paramMax, unit, show),
		value (val), gestureRange (gestRange), rangeLow (rangeL), rangeHigh (rangeH), parameterMax (paramMax)
{
    setLookAndFeel (&testTunerLookAndFeel);
    createSliders();
    createLabels();
}

TestTuner::~TestTuner()
{
	lowSlider = nullptr;
    highSlider = nullptr;
	rangeLabelMin = nullptr;
	rangeLabelMax = nullptr;
}
    
//==============================================================================
void TestTuner::paint (Graphics& g)
{
    drawTunerSliderBackground (g);
}

void TestTuner::resized()
{
    //Tuner::resized(); // Base class resized
        
    // Sets bounds and changes the slider and labels position
    sliderBounds = getLocalBounds().reduced (30);
    resizeSliders();

    updateLabelBounds (rangeLabelMin);
    updateLabelBounds (rangeLabelMax);

    repaint();
}

void TestTuner::resizeSliders()
{
    auto sliderRadius = jmin (sliderBounds.getWidth(), sliderBounds.getHeight()/2);

    auto adjustedBounds = sliderBounds.withWidth (sliderBounds.getWidth()*2)
                                      .expanded (10)
                                      .withCentre ({getLocalBounds().getCentreX() - sliderRadius/2,
                                                    sliderBounds.getCentreY()});

    lowSlider->setBounds (adjustedBounds);
    highSlider->setBounds (adjustedBounds);
}
    
void TestTuner::updateComponents()
{
    if (rangeLow.getValue() < rangeHigh.getValue())
    {
        // Sets slider value
        if (lowSlider->getThumbBeingDragged() == -1)
            lowSlider->setValue (double (getRangeLow()), dontSendNotification);

        if (highSlider->getThumbBeingDragged() == -1)
            highSlider->setValue (double (getRangeHigh()), dontSendNotification);
        
        // Sets label text
		if (!(rangeLabelMin->isBeingEdited()))
		{
		    rangeLabelMin->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
		}
		  
		if (!(rangeLabelMax->isBeingEdited()))
		{
		    rangeLabelMax->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);
		}
    }
}
    
//==============================================================================
void TestTuner::labelTextChanged (Label* lbl)
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
        lowSlider->setValue (getRangeLow(), dontSendNotification);
        updateLabelBounds (rangeLabelMin);
        lbl->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
    }
    
    else if (lbl == rangeLabelMax)
    {
        if ( val < getRangeLow()) val = getRangeLow();
            
        setRangeHigh (val);
        highSlider->setValue (getRangeHigh(), dontSendNotification);
        updateLabelBounds (rangeLabelMax);
        lbl->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);
    }
}
    
void TestTuner::sliderValueChanged (Slider* sldr)
{
    if (sldr == lowSlider)
    {
        // min value changed by user
		setRangeLow (float (lowSlider->getValue()));
        updateLabelBounds (rangeLabelMin);
        rangeLabelMin->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
        
		// in case the other thumb is dragged along..
		if (rangeLow.getValue() > rangeHigh.getValue())
		{
		    setRangeHigh (float (lowSlider->getValue()));
            highSlider->setValue (double (getRangeHigh()), dontSendNotification);
            updateLabelBounds (rangeLabelMax);
			rangeLabelMax->setText (String (float (sldr->getValue())) + valueUnit, dontSendNotification);
    	}
    }

	// max value changed by user
	else if (sldr == highSlider)
	{
		setRangeHigh (float (highSlider->getValue()));
        updateLabelBounds (rangeLabelMax);
		rangeLabelMax->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);
		    
		// in case the other thumb is dragged along..
		if (rangeLow.getValue() > rangeHigh.getValue())
		{
		    setRangeLow (float (highSlider->getValue()));
            lowSlider->setValue (double (getRangeLow()), dontSendNotification);
            updateLabelBounds (rangeLabelMin);
			rangeLabelMin->setText (String (float (sldr->getValue())) + valueUnit, dontSendNotification);
		}
	}
}

void TestTuner::mouseDown (const MouseEvent& e)
{
    if (getObjectToDrag (e) == lowThumb)
    {
        objectBeingDragged = lowThumb;
        //updateMouseCursor();
        lowSlider->mouseDown (e);
    }
    else if (getObjectToDrag (e) == highThumb)
    {
		objectBeingDragged = highThumb;
        //updateMouseCursor();
        highSlider->mouseDown (e);
    }
    else
    {
        objectBeingDragged = middleArea;
        //updateMouseCursor();
        lowSlider->mouseDown (e);
        highSlider->mouseDown (e);
    }
}

void TestTuner::mouseDrag (const MouseEvent& e)
{
    if (objectBeingDragged == lowThumb )
    {
        lowSlider->mouseDrag (e);
    }
    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseDrag (e);
    }
    else
    {
        lowSlider->mouseDrag (e);
        highSlider->mouseDrag (e);
    }
}

void TestTuner::mouseUp (const MouseEvent& e)
{
    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseUp (e);
    }
    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseUp (e);
    }
    else 
    {
        lowSlider->mouseUp (e);
        highSlider->mouseUp (e);
    }

    if (objectBeingDragged != none)
    {
        objectBeingDragged = none;
        //updateMouseCursor();
        repaint();
    }

}

MouseCursor TestTuner::getMouseCursor()
{
	return MouseCursor::NormalCursor;
    /*
    switch (objectBeingDragged)
    {
        case (none): return MouseCursor::NormalCursor;
        default: return MouseCursor::NoCursor;
    }*/
}

void TestTuner::createSliders()
{
    addAndMakeVisible (lowSlider = new Slider ("Range Low Slider"));
        
    // Slider style
	lowSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    lowSlider->setRotaryParameters (MathConstants<float>::pi, 0.0f, true);
    lowSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    lowSlider->setColour (Slider::thumbColourId, Colour (0xffffffff));
    lowSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
    lowSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
	    
    // Slider values
    lowSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
    lowSlider->setValue (double (getRangeLow()), dontSendNotification);
    lowSlider->addListener (this);
    lowSlider->setInterceptsMouseClicks (false, false);

    addAndMakeVisible (highSlider = new Slider ("Range High Slider"));
        
    // Slider style
    highSlider->setSliderStyle (Slider::RotaryVerticalDrag);
    highSlider->setRotaryParameters (MathConstants<float>::pi, 0.0f, true);
    highSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    highSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
    highSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
        
    // Slider values
    highSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
    highSlider->setValue (double (getRangeHigh()), dontSendNotification);
    highSlider->addListener (this);
    highSlider->setInterceptsMouseClicks (false, false);
}
    
void TestTuner::createLabels()
{
    Tuner::addAndMakeVisible (rangeLabelMin = new Label ("Min Label", TRANS (String(int(getRangeLow())) + valueUnit)));
    Tuner::addAndMakeVisible (rangeLabelMax = new Label ("Max Label", TRANS (String(int(getRangeHigh())) + valueUnit)));
        
    // LabelMin style
    rangeLabelMin->setEditable (true, false, false);
    rangeLabelMin->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMin->setJustificationType (Justification::centred);
    rangeLabelMin->setSize (30, 20);
        
    // LabelMax style
    rangeLabelMax->setEditable (true, false, false);
    rangeLabelMax->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMax->setJustificationType (Justification::centred);
    rangeLabelMax->setSize (30, 20);
        
    // Labels settings
    rangeLabelMin->addListener (this);
    rangeLabelMax->addListener (this);
}
    
void TestTuner::setRangeLow (float val)
{
    rangeLow.beginChangeGesture();
    rangeLow.setValueNotifyingHost (rangeLow.convertTo0to1 (val));
    rangeLow.endChangeGesture();
}
    
void TestTuner::setRangeHigh (float val)
{
    rangeHigh.beginChangeGesture();
    rangeHigh.setValueNotifyingHost (rangeHigh.convertTo0to1 (val));
    rangeHigh.endChangeGesture();
}
    
float TestTuner::getRangeLow()
{
    return rangeLow.convertFrom0to1 (rangeLow.getValue());
}
    
float TestTuner::getRangeHigh()
{
    return rangeHigh.convertFrom0to1 (rangeHigh.getValue());
}


double TestTuner::getAngleFromMouseEventRadians (const MouseEvent& e)
{
    return std::atan2 (e.getMouseDownX() - sliderBounds.getX(),
                        -(e.getMouseDownY() - sliderBounds.getCentreY()));
}

double TestTuner::getThumbAngleRadians (const DraggableObject thumb)
{
    if (thumb != lowThumb && thumb != highThumb) return -0.01;
    
    Slider& slider = (thumb == lowThumb) ? *lowSlider : *highSlider;

    return slider.getRotaryParameters().startAngleRadians
				+ ((slider.getValue() - slider.getMinimum())/slider.getRange().getLength())
                     *(slider.getRotaryParameters().endAngleRadians - slider.getRotaryParameters().startAngleRadians);
}

TestTuner::DraggableObject TestTuner::getObjectToDrag (const MouseEvent& e)
{
    if (e.mods.isShiftDown()) return middleArea;

    double mouseAngle = getAngleFromMouseEventRadians (e);
    auto radius = jmin (sliderBounds.getWidth(), sliderBounds.getHeight()/2);

    // Either a ~6-pixels-long arc OR the 4th of the angle between the two thumbs.
    double tolerance = jmax (std::asin (6.0/radius),
                            ((highSlider->getValue() - lowSlider->getValue())*MathConstants<double>::pi)
                                /(lowSlider->getRange().getLength()*4));

    if (mouseAngle > 0) // Mouse click in the right side
    {
        if (mouseAngle > getThumbAngleRadians (lowThumb) - tolerance)
        {
            return lowThumb;
        }
        else if (mouseAngle < getThumbAngleRadians (highThumb) + tolerance)
        {
            return highThumb;
        }
        else
        {
            return middleArea;
        }
    }
    else // Mouse click on the left side
    {
        if (mouseAngle < -MathConstants<double>::pi/2) return lowThumb;
        else return highThumb;
    }
	
    return none;
}

void TestTuner::drawTunerSliderBackground (Graphics& g)
{
    auto outline = Colour (0xff505050);
    auto fill    = objectBeingDragged == middleArea ? Colour (0xffdedeff) : Colour (0xff7c80de);
    const float rotaryStartAngle = lowSlider->getRotaryParameters().startAngleRadians;
    const float rotaryEndAngle = lowSlider->getRotaryParameters().endAngleRadians;

    auto radius = jmin (sliderBounds.toFloat().getWidth(), sliderBounds.getHeight()/2.0f);
    auto lowAngle = rotaryStartAngle
                        + (lowSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                          * (rotaryEndAngle - rotaryStartAngle);
    auto highAngle = rotaryStartAngle
                        + (highSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                           * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (sliderBounds.getCentreX() - radius/2,
                                 sliderBounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (12.0f, PathStrokeType::curved, PathStrokeType::rounded));

    if (isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (sliderBounds.getCentreX() - radius/2,
                                sliderBounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                lowAngle,
                                highAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));
    }

    if (objectBeingDragged == lowThumb || objectBeingDragged == highThumb)
    {
        auto angle = getThumbAngleRadians (objectBeingDragged);

        Point<float> thumbPoint (sliderBounds.getCentreX() - radius/2
                                     + arcRadius * std::cos (angle - MathConstants<float>::halfPi),
                                 sliderBounds.getCentreY()
                                     + arcRadius * std::sin (angle - MathConstants<float>::halfPi));

        g.setColour (fill.withAlpha (0.6f));
        g.fillEllipse (juce::Rectangle<float> (25, 25).withCentre (thumbPoint));
    }
}

void TestTuner::updateLabelBounds (Label* labelToUpdate)
{
    auto sliderRadius = jmin (sliderBounds.getWidth(), sliderBounds.getHeight()/2);

    if (labelToUpdate == rangeLabelMin)
    {
        auto radius = sliderRadius - 20;
        auto angle = getThumbAngleRadians (lowThumb);

        rangeLabelMin->setCentrePosition (sliderBounds.getCentreX() - sliderRadius/2
                                              + radius * std::cos (angle - MathConstants<float>::halfPi),
                                          sliderBounds.getCentreY()
                                              + radius * std::sin (angle - MathConstants<float>::halfPi));

    }
    else if (labelToUpdate == rangeLabelMax)
    {
        auto radius = sliderRadius + 15;
        auto angle = getThumbAngleRadians (highThumb);

        rangeLabelMax->setCentrePosition (sliderBounds.getCentreX() - sliderRadius/2
                                              + radius * std::cos (angle - MathConstants<float>::halfPi),
                                          sliderBounds.getCentreY()
                                              + radius * std::sin (angle - MathConstants<float>::halfPi));
    }
}