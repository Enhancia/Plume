/*
  ==============================================================================

    Tuner.cpp
    Created: 30 Nov 2018 4:16:50pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Tuner/TwoRangeTuner.h"

//==============================================================================
TwoRangeTuner::TwoRangeTuner(const float& val, const NormalisableRange<float> gestRange,
                RangedAudioParameter& rangeLL, RangedAudioParameter& rangeLH,
                RangedAudioParameter& rangeRL, RangedAudioParameter& rangeRH,
                const Range<float> paramMax,
                const String unit, bool show)
    :   Tuner (val, gestRange, paramMax, unit, show),
        value (val), gestureRange (gestRange),
        rangeLeftLow (rangeLL), rangeLeftHigh (rangeLH),
        rangeRightLow (rangeRL), rangeRightHigh (rangeRH),
        parameterMax (paramMax)
{
    setAngles (MathConstants<float>::pi*5/3, MathConstants<float>::pi*7/3);
    tunerColour = Colour (0xffa255f3);
    createSliders();
    createLabels();
}

TwoRangeTuner::~TwoRangeTuner()
{
    leftLowSlider = nullptr;
    leftHighSlider = nullptr;
    rightLowSlider = nullptr;
    rightHighSlider = nullptr;
	rangeLabelMinLeft = nullptr;
	rangeLabelMaxLeft = nullptr;
	rangeLabelMinRight = nullptr;
	rangeLabelMaxRight = nullptr;
}
    
//==============================================================================
void TwoRangeTuner::paint (Graphics& g)
{   
    drawTunerSliderBackground (g);
    drawValueCursor (g);
}

void TwoRangeTuner::resized()
{
    // Sets bounds and changes the slider and labels position
    sliderBounds = getLocalBounds().reduced (30);
    resizeSliders();

    updateLabelBounds (rangeLabelMinLeft);
    updateLabelBounds (rangeLabelMaxLeft);
    updateLabelBounds (rangeLabelMinRight);
    updateLabelBounds (rangeLabelMaxRight);

    repaint();
}


void TwoRangeTuner::resizeSliders()
{
    sliderRadius = jmin (sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
    sliderCentre = {getLocalBounds().getCentreX(),
                    sliderBounds.getCentreY() + (int) sliderRadius/2};

    juce::Rectangle<int> adjustedBounds = sliderBounds.withWidth (sliderRadius*2)
                                                      .withHeight (sliderRadius*2)
                                                      .expanded (10)
                                                      .withCentre (sliderCentre);

    leftLowSlider->setBounds (adjustedBounds);
    leftHighSlider->setBounds (adjustedBounds);
    rightLowSlider->setBounds (adjustedBounds);
    rightHighSlider->setBounds (adjustedBounds);
}

void TwoRangeTuner::updateComponents()
{
    if (rangeLeftLow.getValue() < rangeLeftHigh.getValue()
            && leftLowSlider->getThumbBeingDragged() == -1
            && leftHighSlider->getThumbBeingDragged() == -1)
    {
        // Sets slider value
        leftLowSlider->setValue (double (getRangeLeftLow()), dontSendNotification);
        leftHighSlider->setValue (double (getRangeLeftHigh()), dontSendNotification);
        
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
        
    if (rangeRightLow.getValue() < rangeRightHigh.getValue()
            && rightLowSlider->getThumbBeingDragged() == -1
            && rightHighSlider->getThumbBeingDragged() == -1)
    {
        // Sets slider value
        rightHighSlider->setValue (double (getRangeRightHigh()), dontSendNotification);
        rightLowSlider->setValue (double (getRangeRightLow()), dontSendNotification);
        
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

void TwoRangeTuner::updateDisplay()
{
    if (getValueAngle() != previousCursorAngle)
    {
        repaint();
    }
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
        leftLowSlider->setValue (getRangeLeftLow(), dontSendNotification);
        lbl->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxLeft)  // Max left
    {
        if ( val < getRangeLeftLow()) val = getRangeLeftLow();
        else if (val > (parameterMax.getStart() + parameterMax.getEnd())/2) val = (parameterMax.getStart() + parameterMax.getEnd())/2;
            
        setRangeLeftHigh (val);
        leftHighSlider->setValue (getRangeLeftHigh(), dontSendNotification);
        lbl->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMinRight)   // Min right
    {
        if ( val > getRangeRightHigh()) val = getRangeRightHigh();
        else if (val < (parameterMax.getStart() + parameterMax.getEnd())/2) val = (parameterMax.getStart() + parameterMax.getEnd())/2;
            
        setRangeRightLow (val);
        rightLowSlider->setValue (getRangeRightLow(), dontSendNotification);
        lbl->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxRight)  // Max right
    {
        if ( val < getRangeRightLow()) val = getRangeRightLow();
            
        setRangeRightHigh (val);
        rightHighSlider->setValue (getRangeRightHigh(), dontSendNotification);
        lbl->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
    }
}
    
void TwoRangeTuner::sliderValueChanged (Slider* sldr)
{
	// min left value changed
	if (sldr == leftLowSlider)
	{
	    setRangeLeftLow (float (sldr->getValue()));
        updateLabelBounds (rangeLabelMinLeft);
	    rangeLabelMinLeft->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
	        
	    // in case the other thumb is dragged along..
		if (rangeLeftLow.getValue() > rangeLeftHigh.getValue())
		{
			setRangeLeftHigh (float(sldr->getValue()));
            leftHighSlider->setValue (double (getRangeLeftHigh()), dontSendNotification);
            updateLabelBounds (rangeLabelMaxLeft);
			rangeLabelMaxLeft->setText(String(int(getRangeLeftHigh())) + valueUnit, dontSendNotification);
		}
				
	}

	// max left value changed
	else if (sldr == leftHighSlider)
	{
        // Illegal attempt to get left range over right range
        if (leftHighSlider->getValue() > rightLowSlider->getValue())
        {
            setRangeLeftHigh (rightLowSlider->getValue());
            sldr->setValue (double (rightLowSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMaxLeft);
            rangeLabelMaxLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
            return;
        }

        // normal case
		setRangeLeftHigh (float (sldr->getValue()));
        updateLabelBounds (rangeLabelMaxLeft);
	    rangeLabelMaxLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
	        
	    // in case the other thumb is dragged along..
		if (rangeLeftLow.getValue() > rangeLeftHigh.getValue())
		{
			setRangeLeftLow (float(sldr->getValue()));
            leftLowSlider->setValue (double (getRangeLeftLow()), dontSendNotification);
            updateLabelBounds (rangeLabelMinLeft);
			rangeLabelMinLeft->setText(String(int(getRangeLeftLow())) + valueUnit, dontSendNotification);
		}
	}

	// min right value changed
	if (sldr == rightLowSlider)
	{
        // Illegal attempt to get right range over left range
        if (leftHighSlider->getValue() > rightLowSlider->getValue())
        {
            setRangeRightLow (leftHighSlider->getValue());
            sldr->setValue (double (leftHighSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMinRight);
            rangeLabelMinRight->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
            return;
        }

        // Normal case
		setRangeRightLow (float (sldr->getValue()));
        updateLabelBounds (rangeLabelMinRight);
	    rangeLabelMinRight->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
	        
	    // in case the other thumb is dragged along..
		if (rangeRightLow.getValue() > rangeRightHigh.getValue())
		{
			setRangeRightHigh (float(sldr->getValue()));
            rightHighSlider->setValue (double (getRangeRightHigh()), dontSendNotification);
            updateLabelBounds (rangeLabelMaxRight);
			rangeLabelMaxRight->setText(String(int(getRangeRightHigh())) + valueUnit, dontSendNotification);
		}
	}

	// max right value changed
	else if (sldr == rightHighSlider)
	{
		setRangeRightHigh (float (sldr->getValue()));
        updateLabelBounds (rangeLabelMaxRight);
	    rangeLabelMaxRight->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
	        
	    // in case the other thumb is dragged along..
		if (rangeRightLow.getValue() > rangeRightHigh.getValue())
		{
			setRangeRightLow (float(sldr->getValue()));
            rightLowSlider->setValue (double (getRangeRightLow()), dontSendNotification);
            updateLabelBounds (rangeLabelMinRight);
			rangeLabelMinRight->setText(String(int(getRangeRightLow())) + valueUnit, dontSendNotification);
		}
	}
}

void TwoRangeTuner::mouseDown (const MouseEvent& e)
{
    objectBeingDragged = getObjectToDrag (e);

    if (objectBeingDragged == leftLowThumb)
    {
        leftLowSlider->mouseDown (e.getEventRelativeTo (leftLowSlider));
        rangeLabelMinLeft->setVisible (true);
    }
    else if (objectBeingDragged == leftHighThumb)
    {
        leftHighSlider->mouseDown (e.getEventRelativeTo (leftHighSlider));
        rangeLabelMaxLeft->setVisible (true);
    }
    else if (objectBeingDragged == rightLowThumb)
    {
        rightLowSlider->mouseDown (e.getEventRelativeTo (rightLowSlider));
        rangeLabelMinRight->setVisible (true);
    }
    else if (objectBeingDragged == rightHighThumb)
    {
        rightHighSlider->mouseDown (e.getEventRelativeTo (rightHighSlider));
        rangeLabelMaxRight->setVisible (true);
    }
    else if (objectBeingDragged == middleAreaLeft)
    {
        leftLowSlider->setSliderStyle (Slider::RotaryHorizontalDrag);
        leftHighSlider->setSliderStyle (Slider::RotaryHorizontalDrag);

        leftLowSlider->mouseDown (e.getEventRelativeTo (leftLowSlider));
        leftHighSlider->mouseDown (e.getEventRelativeTo (leftHighSlider));
        rangeLabelMinLeft->setVisible (true);
        rangeLabelMaxLeft->setVisible (true);
    }
    else if (objectBeingDragged == middleAreaLeft)
    {
        rightLowSlider->setSliderStyle (Slider::RotaryHorizontalDrag);
        rightHighSlider->setSliderStyle (Slider::RotaryHorizontalDrag);

        rightLowSlider->mouseDown (e.getEventRelativeTo (rightLowSlider));
        rightHighSlider->mouseDown (e.getEventRelativeTo (rightHighSlider));
        rangeLabelMinRight->setVisible (true);
        rangeLabelMaxRight->setVisible (true);
    }

    updateMouseCursor();
    repaint();
}
void TwoRangeTuner::mouseDrag (const MouseEvent& e)
{
    if (objectBeingDragged == leftLowThumb)        leftLowSlider->mouseDrag (e.getEventRelativeTo (leftLowSlider));
    else if (objectBeingDragged == leftHighThumb)  leftHighSlider->mouseDrag (e.getEventRelativeTo (leftHighSlider));
    else if (objectBeingDragged == rightLowThumb)  rightLowSlider->mouseDrag (e.getEventRelativeTo (rightLowSlider));
    else if (objectBeingDragged == rightHighThumb) rightHighSlider->mouseDrag (e.getEventRelativeTo (rightHighSlider));
    
    else if (objectBeingDragged == middleAreaLeft)
    {
        leftLowSlider->mouseDrag (e.getEventRelativeTo (leftLowSlider));
        leftHighSlider->mouseDrag (e.getEventRelativeTo (leftHighSlider));
    }
    else if (objectBeingDragged == middleAreaRight)
    {
        rightLowSlider->mouseDrag (e.getEventRelativeTo (rightLowSlider));
        rightHighSlider->mouseDrag (e.getEventRelativeTo (rightHighSlider));
    }

    repaint();
}
void TwoRangeTuner::mouseUp (const MouseEvent& e)
{
    if (objectBeingDragged == middleAreaLeft)
    {
        leftLowSlider->setSliderStyle (Slider::Rotary);
        leftHighSlider->setSliderStyle (Slider::Rotary);
    }
    else if (objectBeingDragged == middleAreaRight)
    {
        rightLowSlider->setSliderStyle (Slider::Rotary);
        rightHighSlider->setSliderStyle (Slider::Rotary);
    }

    leftLowSlider->mouseUp (e.getEventRelativeTo (leftLowSlider));
    leftHighSlider->mouseUp (e.getEventRelativeTo (leftHighSlider));
    rightLowSlider->mouseUp (e.getEventRelativeTo (rightLowSlider));
    rightHighSlider->mouseUp (e.getEventRelativeTo (rightHighSlider));

    if (objectBeingDragged != none)
    {
        rangeLabelMinLeft->setVisible (false);
        rangeLabelMaxLeft->setVisible (false);
        rangeLabelMinRight->setVisible (false);
        rangeLabelMaxRight->setVisible (false);
        objectBeingDragged = none;
        updateMouseCursor();
        repaint();
    }
}
MouseCursor TwoRangeTuner::getMouseCursor()
{
    return MouseCursor::NormalCursor;
}

void TwoRangeTuner::setAngles (float start, float end)
{
    startAngle = start;
    endAngle = end;
}

void TwoRangeTuner::createSliders()
{
    Tuner::addAndMakeVisible (leftLowSlider = new Slider ("Range Slider Left Low"));
    Tuner::addAndMakeVisible (leftHighSlider = new Slider ("Range Slider Left High"));
    Tuner::addAndMakeVisible (rightLowSlider = new Slider ("Range Slider Right Low"));
    Tuner::addAndMakeVisible (rightHighSlider = new Slider ("Range Slider Right High"));
        
    // Slider style
    //Left
    leftLowSlider->setSliderStyle (Slider::Rotary);
    leftLowSlider->setRotaryParameters (startAngle, endAngle, true);
    leftLowSlider->setSliderSnapsToMousePosition (false);
    leftLowSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    leftLowSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
    leftLowSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));

    leftHighSlider->setSliderStyle (Slider::Rotary);
    leftHighSlider->setRotaryParameters (startAngle, endAngle, true);
    leftHighSlider->setSliderSnapsToMousePosition (false);
    leftHighSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    leftHighSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
    leftHighSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
        
    //Right
    rightLowSlider->setSliderStyle (Slider::Rotary);
    rightLowSlider->setRotaryParameters (startAngle, endAngle, true);
    rightLowSlider->setSliderSnapsToMousePosition (false);
    rightLowSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    rightLowSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
    rightLowSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));

    rightHighSlider->setSliderStyle (Slider::Rotary);
    rightHighSlider->setRotaryParameters (startAngle, endAngle, true);
    rightHighSlider->setSliderSnapsToMousePosition (false);
    rightHighSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    rightHighSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
    rightHighSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
	    
    // Slider values
    leftLowSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
    leftLowSlider->setValue (double (getRangeLeftLow()), dontSendNotification);
    leftLowSlider->addListener (this);
    leftLowSlider->setInterceptsMouseClicks (false, false);

    leftHighSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
    leftHighSlider->setValue (double (getRangeLeftHigh()), dontSendNotification);
    leftHighSlider->addListener (this);
    leftHighSlider->setInterceptsMouseClicks (false, false);

    rightLowSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
    rightLowSlider->setValue (double (getRangeRightLow()), dontSendNotification);
    rightLowSlider->addListener (this);
    rightLowSlider->setInterceptsMouseClicks (false, false);

    rightHighSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
    rightHighSlider->setValue (double (getRangeRightHigh()), dontSendNotification);
    rightHighSlider->addListener (this);
    rightHighSlider->setInterceptsMouseClicks (false, false);

    // l&f
    leftLowSlider->setLookAndFeel (&slidersLookAndFeel);
    leftHighSlider->setLookAndFeel (&slidersLookAndFeel);
    rightLowSlider->setLookAndFeel (&slidersLookAndFeel);
    rightHighSlider->setLookAndFeel (&slidersLookAndFeel);

    leftLowSlider->addListener (this);
    leftHighSlider->addListener (this);
    rightLowSlider->addListener (this);
    rightHighSlider->addListener (this);
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
    rangeLabelMinLeft->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMinLeft->setJustificationType (Justification::centred);
    rangeLabelMinLeft->setColour (Label::textColourId, tunerColour);
    rangeLabelMinLeft->setSize (30, 20);
        
    // LabelMaxLeft style
    rangeLabelMaxLeft->setEditable (true, false, false);
    rangeLabelMaxLeft->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMaxLeft->setJustificationType (Justification::centred);
    rangeLabelMaxLeft->setColour (Label::textColourId, tunerColour);
    rangeLabelMaxLeft->setSize (30, 20);
        
    // LabelMinRight style
    rangeLabelMinRight->setEditable (true, false, false);
    rangeLabelMinRight->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMinRight->setJustificationType (Justification::centred);
    rangeLabelMinRight->setColour (Label::textColourId, tunerColour);
    rangeLabelMinRight->setSize (30, 20);
        
    // LabelMaxRight style
    rangeLabelMaxRight->setEditable (true, false, false);
    rangeLabelMaxRight->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMaxRight->setJustificationType (Justification::centred);
    rangeLabelMaxRight->setColour (Label::textColourId, tunerColour);
    rangeLabelMaxRight->setSize (30, 20);
        
    // Labels settings
    rangeLabelMinLeft->addListener (this);
    rangeLabelMaxLeft->addListener (this);
    rangeLabelMinRight->addListener (this);
    rangeLabelMaxRight->addListener (this);
}
    
void TwoRangeTuner::setRangeLeftLow (float val)
{
    rangeLeftLow.beginChangeGesture();
    rangeLeftLow.setValueNotifyingHost (rangeLeftLow.convertTo0to1 (val));
    rangeLeftLow.endChangeGesture();
}
    
void TwoRangeTuner::setRangeLeftHigh (float val)
{
    rangeLeftHigh.beginChangeGesture();
    rangeLeftHigh.setValueNotifyingHost (rangeLeftHigh.convertTo0to1 (val));
    rangeLeftHigh.endChangeGesture();
}
    
void TwoRangeTuner::setRangeRightLow (float val)
{
    rangeRightLow.beginChangeGesture();
    rangeRightLow.setValueNotifyingHost (rangeRightLow.convertTo0to1 (val));
    rangeRightLow.endChangeGesture();
}
    
void TwoRangeTuner::setRangeRightHigh (float val)
{
    rangeRightHigh.beginChangeGesture();
    rangeRightHigh.setValueNotifyingHost (rangeRightHigh.convertTo0to1 (val));
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

double TwoRangeTuner::getAngleFromMouseEventRadians (const MouseEvent& e)
{
    double angle =  std::atan2 (e.getMouseDownX() - sliderCentre.x,
                                -(e.getMouseDownY() - sliderCentre.y));

    if (angle < 0) angle += MathConstants<double>::twoPi;

    return angle;
}

double TwoRangeTuner::getThumbAngleRadians (const DraggableObject thumb)
{
    if (thumb == none || thumb == middleAreaLeft || thumb == middleAreaRight) return -0.01;
    
    Slider* slider;

    switch (thumb)
    {
        case leftLowThumb: slider = leftLowSlider; break;
        case leftHighThumb: slider = leftHighSlider; break;
        case rightLowThumb: slider = rightLowSlider; break;
        case rightHighThumb: slider = rightHighSlider; break;
        default: return -0.01;
    }

    return startAngle + ((slider->getValue() - slider->getMinimum())/slider->getRange().getLength())
                             *(endAngle - startAngle);
}

TwoRangeTuner::DraggableObject TwoRangeTuner::getObjectToDrag (const MouseEvent& e)
{
    if (e.mods.isShiftDown()) return middleAreaLeft;

    double mouseAngle = getAngleFromMouseEventRadians (e);
    bool inverted = startAngle > endAngle;

    // The 4th of the angle between the two thumbs.
    double tolerance = ((leftHighSlider->getValue() - leftLowSlider->getValue())*(std::abs (endAngle - startAngle)))
                                /(leftLowSlider->getRange().getLength()*5);

    // Computes "% 2*pi" if both angles are somehow greater that 2*pi
    float startAngleModulo = startAngle, endAngleModulo = endAngle;
    while (startAngleModulo > MathConstants<float>::twoPi
           && endAngleModulo > MathConstants<float>::twoPi)
    {
        startAngleModulo -= MathConstants<float>::twoPi;
        endAngleModulo -= MathConstants<float>::twoPi;
    }

    // If only one thumb is higher than 2*pi, the process needs a different logic.
    // Boolean "differentRef" notifies that this is the case.
    bool differentRef = jmax(startAngleModulo, endAngleModulo) > MathConstants<float>::twoPi
                        && jmin(startAngleModulo, endAngleModulo) < MathConstants<float>::twoPi;
    
    // If the higher thumb minus 2pi is still higher that the smaller, then we fall back to the
    // previous logic with "inverted" thumbs.
    if (differentRef && jmax(startAngleModulo, endAngleModulo) - MathConstants<float>::twoPi
                        > jmin(startAngleModulo, endAngleModulo))
    {
        differentRef = false;
        inverted ? startAngleModulo -= MathConstants<float>::twoPi
                 : endAngleModulo -= MathConstants<float>::twoPi;
    }

    DraggableObject thumb1 = inverted ? leftLowThumb : leftHighThumb;
    DraggableObject thumb2 = inverted ? leftHighThumb : leftLowThumb;

    // Click within the slider range
    if ((!differentRef && (mouseAngle > jmin (startAngleModulo, endAngleModulo)
                       && mouseAngle < jmax (startAngleModulo, endAngleModulo)))
        || (differentRef && (mouseAngle > jmin(startAngleModulo, endAngleModulo)
                                || mouseAngle + MathConstants<float>::twoPi
                                   < jmax(startAngleModulo, endAngleModulo))))
    {
        if (!differentRef)
        {
            if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
            else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
            else return middleAreaLeft;
        }
        else
        {
            if (mouseAngle < jmax(startAngleModulo, endAngleModulo) - MathConstants<float>::twoPi)
                mouseAngle += MathConstants<float>::twoPi;

            if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
            else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
            else return middleAreaLeft;
        }
    }
    
    return none; // Somehow no thumb could be chosen...
}

void TwoRangeTuner::drawTunerSliderBackground (Graphics& g)
{
    auto outline = Colour (0xff505050);
    auto fillLeft    = objectBeingDragged == middleAreaLeft ? tunerColour.interpolatedWith (Colour (0xffffffff), 0.8f)
                                                            : tunerColour;

    auto fillRight    = objectBeingDragged == middleAreaRight ? tunerColour.interpolatedWith (Colour (0xffffffff), 0.8f)
                                                              : tunerColour;


    auto leftLowAngle = startAngle
                        + (leftLowSlider->getValue() - leftLowSlider->getMinimum())
                            / leftLowSlider->getRange().getLength() * (endAngle - startAngle);
    auto leftHighAngle = startAngle
                        + (leftHighSlider->getValue() - leftLowSlider->getMinimum())
                            / leftLowSlider->getRange().getLength() * (endAngle - startAngle);

    auto rightLowAngle = startAngle
                        + (rightLowSlider->getValue() - rightLowSlider->getMinimum())
                            / rightLowSlider->getRange().getLength() * (endAngle - startAngle);
    auto rightHighAngle = startAngle
                        + (rightHighSlider->getValue() - rightLowSlider->getMinimum())
                            / rightLowSlider->getRange().getLength() * (endAngle - startAngle);

    auto lineW = 7.0f;
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
        Path valueArcLeft, valueArcRight;
        valueArcLeft.addCentredArc (sliderCentre.x,
                                    sliderCentre.y,
                                    arcRadius,
                                    arcRadius,
                                    0.0f,
                                    leftLowAngle,
                                    leftHighAngle,
                                    true);

        g.setColour (fillLeft);
        g.strokePath (valueArcLeft, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));

        valueArcRight.addCentredArc (sliderCentre.x,
                                     sliderCentre.y,
                                     arcRadius,
                                     arcRadius,
                                     0.0f,
                                     rightLowAngle,
                                     rightHighAngle,
                                     true);

        g.setColour (fillRight);
        g.strokePath (valueArcRight, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));

    }

    if (objectBeingDragged != none && objectBeingDragged == middleAreaLeft
                                   && objectBeingDragged == middleAreaRight)
    {
        auto angle = getThumbAngleRadians (objectBeingDragged);

        Point<float> thumbPoint (sliderCentre.x + arcRadius * std::cos (angle - MathConstants<float>::halfPi),
                                 sliderCentre.y + arcRadius * std::sin (angle - MathConstants<float>::halfPi));

        g.setColour (tunerColour.withAlpha (0.6f));
        g.fillEllipse (juce::Rectangle<float> (25, 25).withCentre (thumbPoint));
    }
}
void TwoRangeTuner::updateLabelBounds (Label* labelToUpdate)
{
    if (!labelToUpdate->isVisible()) return;
    int radius;
    float angle;

    if (labelToUpdate == rangeLabelMinLeft)
    {
        radius = sliderRadius - 20;
        angle = getThumbAngleRadians (leftLowThumb);
    }
    else if (labelToUpdate == rangeLabelMaxLeft)
    {
        radius = sliderRadius + 15;
        angle = getThumbAngleRadians (leftHighThumb);
    }
    else if (labelToUpdate == rangeLabelMinRight)
    {
        radius = sliderRadius - 20;
        angle = getThumbAngleRadians (rightLowThumb);
    }
    else if (labelToUpdate == rangeLabelMinRight)
    {
        radius = sliderRadius + 15;
        angle = getThumbAngleRadians (rightHighThumb);
    }
    
    labelToUpdate->setCentrePosition (sliderCentre.x + radius * std::cos (angle - MathConstants<float>::halfPi),
                                      sliderCentre.y + radius * std::sin (angle - MathConstants<float>::halfPi));
}

float TwoRangeTuner::getValueAngle()
{
    float convertedValue = gestureRange.convertFrom0to1 (value);

    float cursorAngle;

    if (gestureRange.getRange().getLength() > 0)
    {
        if (convertedValue < parameterMax.getStart())    convertedValue = parameterMax.getStart();
        else if (convertedValue > parameterMax.getEnd()) convertedValue = parameterMax.getEnd();

        cursorAngle = startAngle + (convertedValue - parameterMax.getStart()) * (endAngle - startAngle)
                                        / parameterMax.getLength();
    }
    else
    {
        cursorAngle = startAngle;
    }

    // In case the angle somehow exceeds 2*pi
    while (cursorAngle > MathConstants<float>::twoPi)
    {
        cursorAngle -= MathConstants<float>::twoPi;
    }

    return cursorAngle;
}
void TwoRangeTuner::drawValueCursor (Graphics& g)
{
    float cursorAngle = getValueAngle();
    previousCursorAngle = cursorAngle;

    auto cursorRadius = sliderRadius + 7;
    Point<float> cursorPoint (sliderCentre.x + cursorRadius * std::cos (cursorAngle - MathConstants<float>::halfPi),
                              sliderCentre.y + cursorRadius * std::sin (cursorAngle - MathConstants<float>::halfPi));

    Path cursorPath;
    cursorPath.addTriangle ({cursorPoint.x - 3.0f, cursorPoint.y - 3.0f},
                            {cursorPoint.x + 3.0f, cursorPoint.y - 3.0f},
                            {cursorPoint.x,        cursorPoint.y + 3.0f});
    
    AffineTransform transform = AffineTransform::rotation (cursorAngle,
                                                           cursorPath.getBounds().getCentreX(),
                                                           cursorPath.getBounds().getCentreY());

    g.setColour (   ((gestureRange.convertFrom0to1 (value) >= getRangeLeftLow())
                        && (gestureRange.convertFrom0to1 (value) <= getRangeLeftHigh()))
                 ||  ((gestureRange.convertFrom0to1 (value) >= getRangeRightLow())
                        && (gestureRange.convertFrom0to1 (value) <= getRangeRightHigh()))
                  ? tunerColour
                  : Colour (0x80808080));
    g.fillPath (cursorPath, transform);
}