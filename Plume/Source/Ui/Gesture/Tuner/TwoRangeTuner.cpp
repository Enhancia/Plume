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
                const Range<float> paramMax, const String unit)
    :   Tuner (unit, Colour (0xff1fcaa8)),
        value (val), gestureRange (gestRange),
        rangeLeftLow (rangeLL), rangeLeftHigh (rangeLH),
        rangeRightLow (rangeRL), rangeRightHigh (rangeRH),
        parameterMax (paramMax)
{
    setAngles (MathConstants<float>::pi*5/3, MathConstants<float>::pi*7/3);
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
        if (val > getRangeLeftHigh()) val = getRangeLeftHigh();
            
        setRangeLeftLow (val);
        leftLowSlider->setValue (getRangeLeftLow(), sendNotification);
        lbl->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxLeft)  // Max left
    {
        if (val < getRangeLeftLow()) val = getRangeLeftLow();
        else if (val > getRangeRightLow()) val = getRangeRightLow();
            
        setRangeLeftHigh (val);
        leftHighSlider->setValue (getRangeLeftHigh(), sendNotification);
        lbl->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMinRight)   // Min right
    {
        if ( val > getRangeRightHigh()) val = getRangeRightHigh();
        else if (val < getRangeLeftHigh()) val = getRangeLeftHigh();
            
        setRangeRightLow (val);
        rightLowSlider->setValue (getRangeRightLow(), sendNotification);
        lbl->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxRight)  // Max right
    {
        if ( val < getRangeRightLow()) val = getRangeRightLow();
            
        setRangeRightHigh (val);
        rightHighSlider->setValue (getRangeRightHigh(), sendNotification);
        lbl->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
    }
}

void TwoRangeTuner::editorHidden (Label* lbl, TextEditor&)
{
    lbl->setVisible (false);
}

void TwoRangeTuner::sliderValueChanged (Slider* sldr)
{
	// min left value changed
	if (sldr == leftLowSlider)
	{
        // Illegal attempt to get left range over right range
        if (sldr->getValue() > rightLowSlider->getValue())
        {
            setRangeLeftLow (rightLowSlider->getValue());
            sldr->setValue (double (rightLowSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMinLeft);
            rangeLabelMinLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
            return;
        }
        else // Normal drag
        {
            setRangeLeftLow (float (sldr->getValue()));
            updateLabelBounds (rangeLabelMinLeft);
            rangeLabelMinLeft->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
	    }

	    // In case the other thumb is dragged along..
		if (rangeLeftLow.getValue() > rangeLeftHigh.getValue())
		{
			setRangeLeftHigh (float(sldr->getValue()));
            leftHighSlider->setValue (double (getRangeLeftHigh()), sendNotification);
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
            rangeLabelMinRight->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
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
        // Illegal attempt to get right range over left range
        if (leftHighSlider->getValue() > sldr->getValue())
        {
            setRangeRightHigh (leftHighSlider->getValue());
            sldr->setValue (double (leftHighSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMaxRight);
            rangeLabelMaxRight->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
        }
        else // Normal drag
        {
    		setRangeRightHigh (float (sldr->getValue()));
            updateLabelBounds (rangeLabelMaxRight);
    	    rangeLabelMaxRight->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
	    }  
	    // in case the other thumb is dragged along..
		if (rangeRightLow.getValue() > rangeRightHigh.getValue())
		{
			setRangeRightLow (float(sldr->getValue()));
            rightLowSlider->setValue (double (getRangeRightLow()), sendNotification);
            updateLabelBounds (rangeLabelMinRight);
			rangeLabelMinRight->setText(String(int(getRangeRightLow())) + valueUnit, dontSendNotification);
		}
	}
}

void TwoRangeTuner::mouseDown (const MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        if (e.getNumberOfClicks() == 1)
        {
            handleSingleClick (e);
        }
        else
        {
            handleDoubleClick (e);
        }
    }
}

void TwoRangeTuner::handleSingleClick (const MouseEvent& e)
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
    else if (objectBeingDragged == middleAreaRight)
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

void TwoRangeTuner::handleDoubleClick (const MouseEvent& e)
{
    if (getObjectToDrag (e) == leftLowThumb)
    {
		rangeLabelMinLeft->setVisible (true);
        rangeLabelMinLeft->showEditor();
    }
    else if (getObjectToDrag (e) == leftHighThumb)
    {
		rangeLabelMaxLeft->setVisible (true);
        rangeLabelMaxLeft->showEditor();
    }
    else if (getObjectToDrag (e) == rightLowThumb)
    {
        rangeLabelMinRight->setVisible (true);
		rangeLabelMinRight->showEditor();
    }
    else if (getObjectToDrag (e) == rightHighThumb)
    {
		rangeLabelMaxRight->setVisible (true);
		rangeLabelMaxRight->showEditor();
    }
}

void TwoRangeTuner::mouseDrag (const MouseEvent& e)
{
    if (!e.mods.isLeftButtonDown() || e.getNumberOfClicks() > 1) return;

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
        
    auto setSliderSettings = [this] (Slider& slider, float valueToSet)
    {
        slider.setSliderStyle (Slider::Rotary);
        slider.setRotaryParameters (startAngle, endAngle, true);
        slider.setSliderSnapsToMousePosition (false);
        slider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        slider.setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
        slider.setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
        slider.setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
        slider.setValue (double (valueToSet));
        slider.addListener (this);
        slider.setInterceptsMouseClicks (false, false);
        slider.setLookAndFeel (&slidersLookAndFeel);
	};

    setSliderSettings (*leftLowSlider, getRangeLeftLow());
    setSliderSettings (*leftHighSlider, getRangeLeftHigh());
    setSliderSettings (*rightLowSlider, getRangeRightLow());
    setSliderSettings (*rightHighSlider, getRangeRightHigh());
}
    
void TwoRangeTuner::createLabels()
{
    addAndMakeVisible (rangeLabelMinLeft  = new Label ("Min Left Label",
                                                       TRANS (String (int(getRangeLeftLow())) + valueUnit)));
    addAndMakeVisible (rangeLabelMaxLeft  = new Label ("Max Left Label",
                                                       TRANS (String(int(getRangeLeftHigh())) + valueUnit)));
    addAndMakeVisible (rangeLabelMinRight = new Label ("Min Right Label",
                                                       TRANS (String(int(getRangeRightLow())) + valueUnit)));
    addAndMakeVisible (rangeLabelMaxRight = new Label ("Max Right Label",
                                                       TRANS (String(int(getRangeRightHigh())) + valueUnit)));
    
    auto setLabelSettings = [this] (Label& label)
    {
        label.setEditable (true, false, false);
        label.setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
        label.setJustificationType (Justification::centred);
        label.setColour (Label::textColourId, tunerColour);
        label.setColour (Label::textWhenEditingColourId, tunerColour);
        label.setColour (TextEditor::textColourId, tunerColour);
        label.setColour (TextEditor::highlightColourId, tunerColour.withAlpha (0.2f));
        label.setColour (TextEditor::highlightedTextColourId, tunerColour);
        label.setColour (CaretComponent::caretColourId, tunerColour.withAlpha (0.2f));
        label.setSize (30, 20);
        label.addListener (this);
        label.setVisible (false);
        label.setLookAndFeel (&slidersLookAndFeel);
    };

    setLabelSettings (*rangeLabelMinLeft);
    setLabelSettings (*rangeLabelMaxLeft);
    setLabelSettings (*rangeLabelMinRight);
    setLabelSettings (*rangeLabelMaxRight);
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
    double mouseAngle = getAngleFromMouseEventRadians (e);
    float twoPi =  MathConstants<float>::twoPi;

    //================ Figures out the scenario surrounding the click ==============
    bool inverted = startAngle > endAngle;

    // Computes "% 2*pi" if both angles are somehow greater that 2*pi
    float startAngleModulo = startAngle, endAngleModulo = endAngle;
    while (startAngleModulo > twoPi
           && endAngleModulo > twoPi)
    {
        startAngleModulo -= twoPi;
        endAngleModulo -= twoPi;
    }

    // If only one extreme angle is higher than 2*pi, the process needs a different logic.
    // Boolean "differentRef" notifies that this is the case.
    bool differentRef = jmax(startAngleModulo, endAngleModulo) > twoPi
                        && jmin(startAngleModulo, endAngleModulo) < twoPi;
    
    // If the higher angle minus 2pi is still higher that the smaller, then we fall back to the
    // previous logic with "inverted" angles.
    if (differentRef && jmax(startAngleModulo, endAngleModulo) - twoPi
                        > jmin(startAngleModulo, endAngleModulo))
    {
        differentRef = false;
        inverted ? startAngleModulo -= twoPi
                 : endAngleModulo -= twoPi;
    }

    //================ Primary check: should we consider the left or right range? ==============
    bool leftNotRight = true;

    float leftHighAngle = jmax (getThumbAngleRadians (leftLowThumb), getThumbAngleRadians (leftHighThumb));
    float rightLowAngle = jmin (getThumbAngleRadians (rightLowThumb), getThumbAngleRadians (rightHighThumb));
    
    auto adjustedMouseAngle = !differentRef ? mouseAngle
                                            : mouseAngle < startAngle ? mouseAngle + twoPi
                                                                      : mouseAngle;

    // case1: both middle thumbs in the same side (< 2pi)
    if (!differentRef || (leftHighAngle < twoPi && rightLowAngle < twoPi))
    {
        if (adjustedMouseAngle < (leftHighAngle + rightLowAngle)/2.0f) leftNotRight = true;
        else                                                           leftNotRight = false;
    }
    // case2: both middle thumbs in the same side (> 2pi)
    else if (leftHighAngle > twoPi && rightLowAngle > twoPi)
    {
        if (adjustedMouseAngle < (leftHighAngle + rightLowAngle)/2.0f) leftNotRight = true;
        else                                                           leftNotRight = false;
    }
    // case3: One midde thumb < 2pi, the other >2pi
    else if (leftHighAngle < twoPi && rightLowAngle > twoPi)
    {
        if (adjustedMouseAngle < (leftHighAngle + rightLowAngle)/2.0f)
        {
            leftNotRight = true;
        }
        else leftNotRight = false;
    }

    DraggableObject lowThumb = leftNotRight ? leftLowThumb : rightLowThumb;
    DraggableObject highThumb = leftNotRight ? leftHighThumb : rightHighThumb;
    DraggableObject middleArea = leftNotRight ? middleAreaLeft : middleAreaRight;
    Slider* lowSlider = leftNotRight ? leftLowSlider : rightLowSlider;
    Slider* highSlider = leftNotRight ? leftHighSlider : rightHighSlider;

    //================ Finds the object to drag ==============
    if (e.mods.isShiftDown()) return middleArea;

    DraggableObject thumb1 = inverted ? lowThumb : highThumb;
    DraggableObject thumb2 = inverted ? highThumb : lowThumb;

    // The 4th of the angle between the two thumbs.
    double tolerance = ((highSlider->getValue() - lowSlider->getValue())*(std::abs (endAngle - startAngle)))
                                /(lowSlider->getRange().getLength()*5);

    // Click within the slider range
    if ((!differentRef && (mouseAngle > jmin (startAngleModulo, endAngleModulo)
                       && mouseAngle < jmax (startAngleModulo, endAngleModulo)))
        || (differentRef && (mouseAngle > jmin(startAngleModulo, endAngleModulo)
                                || mouseAngle + twoPi
                                   < jmax(startAngleModulo, endAngleModulo))))
    {
        if (!differentRef)
        {
            if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
            else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
            else return middleArea;
        }
        else
        {
            if (mouseAngle < jmax(startAngleModulo, endAngleModulo) - twoPi)
                mouseAngle += twoPi;

            if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
            else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
            else return middleArea;
        }
    }
    
    return none;
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

    if (objectBeingDragged != none && objectBeingDragged != middleAreaLeft
                                   && objectBeingDragged != middleAreaRight)
    {
        auto angle = getThumbAngleRadians (objectBeingDragged);

        Point<float> thumbPoint (sliderCentre.x + arcRadius * std::cos (angle - MathConstants<float>::halfPi),
                                 sliderCentre.y + arcRadius * std::sin (angle - MathConstants<float>::halfPi));

        g.setColour (tunerColour.withAlpha (0.6f));
        g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
    }
}
void TwoRangeTuner::updateLabelBounds (Label* labelToUpdate)
{
    if (labelToUpdate == nullptr) return;

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
    else if (labelToUpdate == rangeLabelMaxRight)
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

void TwoRangeTuner::drawLineFromSliderCentre (Graphics& g, float angleRadian)
{
    Point<float> point (sliderCentre.x + sliderRadius * std::cos (angleRadian - MathConstants<float>::halfPi),
                        sliderCentre.y + sliderRadius * std::sin (angleRadian - MathConstants<float>::halfPi));

    g.drawLine (Line<float> (sliderCentre.toFloat(), point), 1.0f);
}

void TwoRangeTuner::drawThumbsAndToleranceLines (Graphics& g)
{
    double leftTolerance = ((leftHighSlider->getValue() - leftLowSlider->getValue())*(std::abs (endAngle - startAngle)))
                         / (leftLowSlider->getRange().getLength() * 5);

    double rightTolerance = ((rightHighSlider->getValue() - rightLowSlider->getValue())*(std::abs (endAngle - startAngle)))
                         / (rightLowSlider->getRange().getLength() * 5);

    g.setColour (tunerColour.withAlpha (0.5f));
    drawLineFromSliderCentre (g, getThumbAngleRadians (leftLowThumb));
    drawLineFromSliderCentre (g, getThumbAngleRadians (leftHighThumb));
    drawLineFromSliderCentre (g, getThumbAngleRadians (rightLowThumb));
    drawLineFromSliderCentre (g, getThumbAngleRadians (rightHighThumb));

    g.setColour (Colour (0xff903030));
    bool invertTolerance = startAngle > endAngle;
    drawLineFromSliderCentre (g, getThumbAngleRadians (leftLowThumb) + (invertTolerance ? -leftTolerance
                                                                                    : leftTolerance));
    drawLineFromSliderCentre (g, getThumbAngleRadians (leftHighThumb) + (invertTolerance ? leftTolerance
                                                                                     : -leftTolerance));
    drawLineFromSliderCentre (g, getThumbAngleRadians (rightLowThumb) + (invertTolerance ? -rightTolerance
                                                                                    : rightTolerance));
    drawLineFromSliderCentre (g, getThumbAngleRadians (rightHighThumb) + (invertTolerance ? rightTolerance
                                                                                     : -rightTolerance));
}