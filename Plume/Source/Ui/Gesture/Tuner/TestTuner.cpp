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
                const String unit, bool show, TunerStyle style)
    : Tuner(val, gestRange, paramMax, unit, show),
		value (val), gestureRange (gestRange), rangeLow (rangeL), rangeHigh (rangeH), parameterMax (paramMax),
        tunerStyle (style)
{
    setLookAndFeel (&testTunerLookAndFeel);
    setStyle (style);
    createSliders();
    createLabels();
}

TestTuner::~TestTuner()
{
    setLookAndFeel (nullptr);

	lowSlider = nullptr;
    highSlider = nullptr;
	rangeLabelMin = nullptr;
	rangeLabelMax = nullptr;
}
    
//==============================================================================
void TestTuner::paint (Graphics& g)
{
    drawTunerSliderBackground (g);
    drawMousePosition (g);
	g.setColour(Colour (0x80006000));
    drawLineFromSliderCenter (g, lastAngle);
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
    switch (tunerStyle)
    {
        case wave:
            sliderRadius = jmin (sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
            sliderCentre = {getLocalBounds().getCentreX() - (int) sliderRadius/2,
                            sliderBounds.getCentreY()};
            break;

        case tilt:
            sliderRadius = jmin (sliderBounds.getWidth()*2/3.0f, sliderBounds.getHeight()*2/3.0f);
            sliderCentre = {getLocalBounds().getCentreX() - (int) sliderRadius/2,
                            sliderBounds.getCentreY() + (int) sliderRadius/2};
            break;

        case roll:
            sliderRadius = jmin (sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
            sliderCentre = {getLocalBounds().getCentreX() - (int) sliderRadius/2,
                            sliderBounds.getCentreY()};
            break;
    }

    

    auto adjustedBounds = sliderBounds.withWidth (sliderBounds.getWidth()*2)
                                      .expanded (10)
                                      .withCentre (sliderCentre);

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
    
void TestTuner::setStyle (TunerStyle newStyle)
{
    tunerStyle = newStyle;

    switch (newStyle)
    {
        case wave:
            //setAngles (MathConstants<float>::pi, 0.0f);
            //setAngles (0.0f, MathConstants<float>::pi);

            //setAngles (MathConstants<float>::halfPi*3, MathConstants<float>::halfPi*5);
            //setAngles (MathConstants<float>::halfPi*5, MathConstants<float>::halfPi*3);

            //setAngles (MathConstants<float>::halfPi, 0.0f);
            //setAngles (0.0f, MathConstants<float>::halfPi);
            setAngles (MathConstants<float>::halfPi*4/5, 0.0f);
            break;

        case tilt:
            setAngles (MathConstants<float>::halfPi, 0.0f);
            break;

        case roll:
            setAngles (MathConstants<float>::halfPi*3, MathConstants<float>::halfPi*5);
            break;
    }
}
void TestTuner::setAngles (float start, float end)
{
    startAngle = start;
    endAngle = end;
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
    mousePosition = e.getMouseDownPosition();

    if (getObjectToDrag (e) == lowThumb)
    {
        objectBeingDragged = lowThumb;
        //updateMouseCursor();
        lowSlider->mouseDown (e.getEventRelativeTo (lowSlider));
        rangeLabelMin->setVisible (true);
    }
    else if (getObjectToDrag (e) == highThumb)
    {
		objectBeingDragged = highThumb;
        //updateMouseCursor();
        highSlider->mouseDown (e.getEventRelativeTo (highSlider));
        rangeLabelMax->setVisible (true);
    }
    
    else
    {
        objectBeingDragged = middleArea;

        //updateMouseCursor();
        lowSlider->mouseDown (e.getEventRelativeTo (lowSlider));
        highSlider->mouseDown (e.getEventRelativeTo (highSlider));
        rangeLabelMin->setVisible (true);
        rangeLabelMax->setVisible (true);
    }

    repaint();
}

void TestTuner::mouseDrag (const MouseEvent& e)
{
    mousePosition = e.getPosition();

    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider));
    }
    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseDrag (e.getEventRelativeTo (highSlider));
    }
    else
    {
        lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider));
        highSlider->mouseDrag (e.getEventRelativeTo (highSlider));
    }

    repaint();
}

void TestTuner::mouseUp (const MouseEvent& e)
{
    mousePosition = {-1, -1};

    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseUp (e.getEventRelativeTo (lowSlider));
    }
    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseUp (e.getEventRelativeTo (highSlider));
    }
    else 
    {
        lowSlider->mouseUp (e.getEventRelativeTo (lowSlider));
        highSlider->mouseUp (e.getEventRelativeTo (highSlider));
    }

    if (objectBeingDragged != none)
    {
        rangeLabelMin->setVisible (false);
        rangeLabelMax->setVisible (false);
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
	lowSlider->setSliderStyle (tunerStyle == roll ? Slider::RotaryHorizontalDrag
                                                  : Slider::RotaryVerticalDrag);
    lowSlider->setRotaryParameters (startAngle, endAngle, true);
    lowSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    lowSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
    lowSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
	    
    // Slider values
    lowSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
    lowSlider->setValue (double (getRangeLow()), dontSendNotification);
    lowSlider->addListener (this);
    lowSlider->setInterceptsMouseClicks (false, false);

    addAndMakeVisible (highSlider = new Slider ("Range High Slider"));
        
    // Slider style
    highSlider->setSliderStyle (tunerStyle == roll ? Slider::RotaryHorizontalDrag
                                                   : Slider::RotaryVerticalDrag);
    highSlider->setRotaryParameters (startAngle, endAngle, true);
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
    updateLabelBounds (rangeLabelMin);
    updateLabelBounds (rangeLabelMax);

    rangeLabelMin->setVisible (false);
    rangeLabelMax->setVisible (false);

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
    double angle =  std::atan2 (e.getMouseDownX() - sliderCentre.x,
                                -(e.getMouseDownY() - sliderCentre.y));

	if (angle < 0) angle += MathConstants<double>::twoPi;

	return angle;
}

double TestTuner::getThumbAngleRadians (const DraggableObject thumb)
{
    if (thumb != lowThumb && thumb != highThumb) return -0.01;
    
    Slider& slider = (thumb == lowThumb) ? *lowSlider : *highSlider;

    return startAngle + ((slider.getValue() - slider.getMinimum())/slider.getRange().getLength())
                             *(endAngle - startAngle);
}

TestTuner::DraggableObject TestTuner::getObjectToDrag (const MouseEvent& e)
{
    if (e.mods.isShiftDown()) return middleArea;

    double mouseAngle = getAngleFromMouseEventRadians (e);
    lastAngle = mouseAngle;
	bool inverted = startAngle > endAngle;

    // Either a ~6-pixels-long arc OR the 4th of the angle between the two thumbs.
    double tolerance = ((highSlider->getValue() - lowSlider->getValue())*(endAngle - startAngle))
                                /(lowSlider->getRange().getLength()*5);
    if (inverted) tolerance *= -1.0;

    // Mouse click inside the range
	float startAngleModulo = startAngle, endAngleModulo = endAngle;

	while (startAngleModulo > MathConstants<float>::twoPi
		   && endAngleModulo > MathConstants<float>::twoPi)
	{
		startAngleModulo -= MathConstants<float>::twoPi;
		endAngleModulo -= MathConstants<float>::twoPi;
	}
	bool differentRef = jmax(startAngleModulo, endAngleModulo) > MathConstants<float>::twoPi
		                && jmin(startAngleModulo, endAngleModulo) < MathConstants<float>::twoPi;
		
	if (differentRef && jmax(startAngleModulo, endAngleModulo) - MathConstants<float>::twoPi
						> jmin(startAngleModulo, endAngleModulo))
	{
		differentRef = false;
		inverted ? startAngleModulo -= MathConstants<float>::twoPi
			     : endAngleModulo -= MathConstants<float>::twoPi;
	}

	DBG("\n\nMouseAngle : " << mouseAngle * 180/ MathConstants<float>::pi
		<< "\nlowThumbAngle : " << getThumbAngleRadians(lowThumb) * 180 / MathConstants<float>::pi
		<< " | highThumbAngle : " << getThumbAngleRadians(highThumb) * 180 / MathConstants<float>::pi
		<< "\nstartAngle : " << startAngle * 180 / MathConstants<float>::pi
		<< " | endAngle : " << endAngle * 180 / MathConstants<float>::pi
		<< "\nstartAngleModulo : " << startAngleModulo * 180 / MathConstants<float>::pi
		<< " | endAngleModulo : " << endAngleModulo * 180 / MathConstants<float>::pi
		<< "\n Different Ref : " << (differentRef ? "true" : "false")
		<< " | Inverted : " << (inverted ? "true" : "false")
	);

	// Click whithin the slider
    if ((!differentRef && (mouseAngle > jmin (startAngleModulo, endAngleModulo)
                       && mouseAngle < jmax (startAngleModulo, endAngleModulo)))
		|| (differentRef && (mouseAngle > jmin(startAngleModulo, endAngleModulo)
			             || mouseAngle + MathConstants<float>::twoPi
                                < jmax(startAngleModulo, endAngleModulo))))
    {
		DraggableObject thumb1 = inverted ? lowThumb : highThumb;
		DraggableObject thumb2 = inverted ? highThumb : lowThumb;


		if (!differentRef)
		{
			if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
			else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
			else return middleArea;
		}
		else
		{
			if (mouseAngle < jmax(startAngleModulo, endAngleModulo) - MathConstants<float>::twoPi)
				mouseAngle += MathConstants<float>::twoPi;


			if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
			else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
			else return middleArea;
		}
    }
    else // Click outside slider range
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

    auto lowAngle = startAngle
                        + (lowSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                          * (endAngle - startAngle);
    auto highAngle = startAngle
                        + (highSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                           * (endAngle - startAngle);
    auto lineW = jmin (8.0f, sliderRadius * 0.5f);
    auto arcRadius = sliderRadius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (sliderCentre.x,
                                 sliderCentre.y,
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 startAngle,
                                 endAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (12.0f, PathStrokeType::curved, PathStrokeType::rounded));

    if (isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (sliderCentre.x,
                                sliderCentre.y,
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

        Point<float> thumbPoint (sliderCentre.x + arcRadius * std::cos (angle - MathConstants<float>::halfPi),
                                 sliderCentre.y + arcRadius * std::sin (angle - MathConstants<float>::halfPi));

        g.setColour (fill.withAlpha (0.6f));
        g.fillEllipse (juce::Rectangle<float> (25, 25).withCentre (thumbPoint));
    }

    {
        // TO DELETE: draws thumb angles and tolerances
		double tolerance = ((highSlider->getValue() - lowSlider->getValue())*MathConstants<double>::pi)
			                 / (lowSlider->getRange().getLength() * 5);


        g.setColour (Colour (0xff323232));
        drawLineFromSliderCenter (g, 0.0f);
        drawLineFromSliderCenter (g, MathConstants<float>::pi);

        g.setColour (Colour (0xffdedeff));
        drawLineFromSliderCenter (g, getThumbAngleRadians (lowThumb));
        drawLineFromSliderCenter (g, getThumbAngleRadians (highThumb));

        g.setColour (Colour (0xff903030));
        bool invertTolerance = startAngle > endAngle;
        drawLineFromSliderCenter (g, getThumbAngleRadians (lowThumb) + (invertTolerance ? -tolerance
                                                                                        : tolerance));
        drawLineFromSliderCenter (g, getThumbAngleRadians (highThumb) + (invertTolerance ? tolerance
                                                                                         : -tolerance));
    }
}

void TestTuner::updateLabelBounds (Label* labelToUpdate)
{
    if (!labelToUpdate->isVisible()) return;

    if (labelToUpdate == rangeLabelMin)
    {
        auto radius = sliderRadius - 20;
        auto angle = getThumbAngleRadians (lowThumb);

        rangeLabelMin->setCentrePosition (sliderCentre.x + radius * std::cos (angle - MathConstants<float>::halfPi),
                                          sliderCentre.y + radius * std::sin (angle - MathConstants<float>::halfPi));

    }
    else if (labelToUpdate == rangeLabelMax)
    {
        auto radius = sliderRadius + 15;
        auto angle = getThumbAngleRadians (highThumb);

        rangeLabelMax->setCentrePosition (sliderCentre.x + radius * std::cos (angle - MathConstants<float>::halfPi),
                                          sliderCentre.y + radius * std::sin (angle - MathConstants<float>::halfPi));
    }
}

void TestTuner::drawMousePosition (Graphics& g)
{
    if (mousePosition.x != -1 && mousePosition.y != -1)
    {
        g.setColour (Colour (0xf000ff00));
        g.drawLine (Line<int> (mousePosition, sliderCentre).toFloat(), 1.0f);
    }
}

void TestTuner::drawLineFromSliderCenter (Graphics& g, float angleRadian)
{
    Point<float> point (sliderCentre.x + sliderRadius * std::cos (angleRadian - MathConstants<float>::halfPi),
                        sliderCentre.y + sliderRadius * std::sin (angleRadian - MathConstants<float>::halfPi));

    g.drawLine (Line<float> (sliderCentre.toFloat(), point), 1.0f);
}