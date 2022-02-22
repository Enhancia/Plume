/*
  ==============================================================================

    Tuner.cpp
    Created: 30 Nov 2018 4:16:50pm
    Author:  Alex

  ==============================================================================
*/

#include "TwoRangeTuner.h"

//==============================================================================
TwoRangeTuner::TwoRangeTuner(const std::atomic<float>& val, const NormalisableRange<float> gestRange,
                RangedAudioParameter& rangeLL, RangedAudioParameter& rangeLH,
                RangedAudioParameter& rangeRL, RangedAudioParameter& rangeRH,
                const NormalisableRange<float> paramMax, const String unit)
    :   Tuner (unit, Colour (0xff1fcaa8)),
        value (val), gestureRange (gestRange),
        rangeLeftLow (rangeLL), rangeLeftHigh (rangeLH),
        rangeRightLow (rangeRL), rangeRightHigh (rangeRH),
        parameterMax (paramMax)
{
    setAngles (MathConstants<float>::pi*5/3, MathConstants<float>::pi*7/3);
    createSliders();
    createLabels();
    createButtons();
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
    sliderBounds = getLocalBounds().reduced (PLUME::UI::MARGIN).translated (0, jmax (20, getHeight()/8));
    resizeSliders();
    resizeButtons();

    updateLabelBounds (rangeLabelMinLeft.get());
    updateLabelBounds (rangeLabelMaxLeft.get());
    updateLabelBounds (rangeLabelMinRight.get());
    updateLabelBounds (rangeLabelMaxRight.get());

    repaint();
}


void TwoRangeTuner::resizeSliders()
{
    sliderRadius = jmin (sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
    sliderCentre = {getLocalBounds().getCentreX(),
                    sliderBounds.getCentreY() + (int) sliderRadius/2};

    juce::Rectangle<int> adjustedBounds = sliderBounds.withWidth (int (sliderRadius*2))
                                                      .withHeight (int (sliderRadius*2))
                                                      .expanded (10)
                                                      .withCentre (sliderCentre);

    leftLowSlider->setBounds (adjustedBounds);
    leftHighSlider->setBounds (adjustedBounds);
    rightLowSlider->setBounds (adjustedBounds);
    rightHighSlider->setBounds (adjustedBounds);
}

void TwoRangeTuner::resizeButtons()
{
    using namespace PLUME::UI;

    auto buttonsAreaLeft = getLocalBounds().withRight (getLocalBounds().getX() + 70)
                                           .withHeight (60)
                                           .reduced (MARGIN);

    auto buttonsAreaRight = getLocalBounds().withLeft (getLocalBounds().getRight() - 70)
                                            .withHeight (60)
                                            .reduced (MARGIN);

    maxLeftAngleButton->setBounds (buttonsAreaLeft.removeFromTop (buttonsAreaLeft.getHeight()/2).withTrimmedBottom (MARGIN/2));
    minLeftAngleButton->setBounds (buttonsAreaLeft.withTrimmedTop (MARGIN/2));
    maxRightAngleButton->setBounds (buttonsAreaRight.removeFromTop (buttonsAreaRight.getHeight()/2).withTrimmedBottom (MARGIN/2));
    minRightAngleButton->setBounds (buttonsAreaRight.withTrimmedTop (MARGIN/2));
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
}

void TwoRangeTuner::updateComponents (TwoRangeTuner::DraggableObject thumbThatShouldUpdate)
{
    if (thumbThatShouldUpdate == leftLowThumb)
    {
        // Sets slider value
        if (leftLowSlider->getThumbBeingDragged() == -1)
        {
            DBG ("left low update");
            if (leftHighSlider->getThumbBeingDragged() == -1 && rangeLeftLow.getValue() > rangeLeftHigh.getValue())
            {
                setRangeLeftHigh (getRangeLeftLow(), true);

                // Allows the DAW to update the value without using slider->setValue() with a notification
                // The latter causes crashes on Ableton Live
                sliderValueChanged (leftHighSlider.get());
            }

            leftLowSlider->setValue (double (getRangeLeftLow()), dontSendNotification);
        }
    }

    else if (thumbThatShouldUpdate == leftHighThumb)
    {
        // Sets slider value
        if (leftHighSlider->getThumbBeingDragged() == -1)
        {
            DBG ("left high update");
            if (leftLowSlider->getThumbBeingDragged() == -1 && rangeLeftLow.getValue() > rangeLeftHigh.getValue())
            {
                setRangeLeftLow (getRangeLeftHigh(), true);

                // Allows the DAW to update the value without using slider->setValue() with a notification
                // The latter causes crashes on Ableton Live
                sliderValueChanged (leftLowSlider.get());
            }

            if (rangeLeftHigh.getValue() > rangeRightLow.getValue()) // Tries to overlap with right range
            {
                setRangeLeftHigh (getRangeRightLow(), true);
                return;
            }

            leftHighSlider->setValue (double (getRangeLeftHigh()), dontSendNotification);
        }
    }

    else if (thumbThatShouldUpdate == rightLowThumb)
    {
        // Sets slider value
        if (rightLowSlider->getThumbBeingDragged() == -1)
        {
            DBG ("right low update");
            if (rightHighSlider->getThumbBeingDragged() == -1 && rangeRightLow.getValue() > rangeRightHigh.getValue())
            {
                setRangeRightHigh (getRangeRightLow(), true);

                // Allows the DAW to update the value without using slider->setValue() with a notification
                // The latter causes crashes on Ableton Live
                sliderValueChanged (rightHighSlider.get());
            }

            if (rangeLeftHigh.getValue() > rangeRightLow.getValue()) // Tries to overlap with left range
            {
                setRangeRightLow (getRangeLeftHigh(), true);
                return;
            }

            rightLowSlider->setValue (double (getRangeRightLow()), dontSendNotification);
        }
    }

    else if (thumbThatShouldUpdate == rightHighThumb)
    {
        // Sets slider value
        if (rightHighSlider->getThumbBeingDragged() == -1)
        {
            DBG ("right high update");
            if (rightLowSlider->getThumbBeingDragged() == -1 && rangeRightLow.getValue() > rangeRightHigh.getValue())
            {
                setRangeRightLow (getRangeRightHigh(), true);

                // Allows the DAW to update the value without using slider->setValue() with a notification
                // The latter causes crashes on Ableton Live
                sliderValueChanged (rightLowSlider.get());
            }

            rightHighSlider->setValue (double (getRangeRightHigh()), dontSendNotification);
        }
    }

    else updateComponents();
}

void TwoRangeTuner::updateDisplay()
{
    if (getValueAngle() != previousCursorAngle)
    {
        repaint (leftLowSlider->getBounds().withTrimmedBottom (leftLowSlider->getHeight()*6/10)
                                           .translated (0, -15));
    }
}

void TwoRangeTuner::setColour (const Colour newColour)
{
    Tuner::setColour (newColour);

    auto setLabelColours = [this] (Label& label)
    {
        label.setColour (Label::textColourId, tunerColour);
        label.setColour (Label::textWhenEditingColourId, tunerColour);
        label.setColour (TextEditor::textColourId, tunerColour);
        label.setColour (TextEditor::highlightColourId, tunerColour.withAlpha (0.2f));
        label.setColour (TextEditor::highlightedTextColourId, tunerColour);
        label.setColour (CaretComponent::caretColourId, tunerColour.withAlpha (0.2f));
    };

    setLabelColours (*rangeLabelMinLeft);
    setLabelColours (*rangeLabelMaxLeft);
    setLabelColours (*rangeLabelMinRight);
    setLabelColours (*rangeLabelMaxRight);

    repaint();
}
    
//==============================================================================
void TwoRangeTuner::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only (and unit)
    if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
    {
        if      (lbl == rangeLabelMinLeft.get())   lbl->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMaxLeft.get())   lbl->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMinRight.get())  lbl->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMaxRight.get())  lbl->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);

        return;
    }
        
    float val;
        
    // Gets the float value of the text 
    if (valueUnit != "" &&
        lbl->getText().endsWith(valueUnit)) val = lbl->getText().upToFirstOccurrenceOf(valueUnit, false, false).getFloatValue();
    else                                    val = lbl->getText().getFloatValue();
        
    if (val < parameterMax.getRange().getStart())    val = parameterMax.getRange().getStart();
    else if (val > parameterMax.getRange().getEnd()) val = parameterMax.getRange().getEnd();
        
    // Sets slider and labels accordingly
    if (lbl == rangeLabelMinLeft.get())   // Min left
    {
        if (val > getRangeLeftHigh()) val = getRangeLeftHigh();
            
        setRangeLeftLow (val);
        leftLowSlider->setValue (getRangeLeftLow(), sendNotification);
        lbl->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxLeft.get())  // Max left
    {
        if (val < getRangeLeftLow()) val = getRangeLeftLow();
        else if (val > getRangeRightLow()) val = getRangeRightLow();
            
        setRangeLeftHigh (val);
        leftHighSlider->setValue (getRangeLeftHigh(), sendNotification);
        lbl->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMinRight.get())   // Min right
    {
        if ( val > getRangeRightHigh()) val = getRangeRightHigh();
        else if (val < getRangeLeftHigh()) val = getRangeLeftHigh();
            
        setRangeRightLow (val);
        rightLowSlider->setValue (getRangeRightLow(), sendNotification);
        lbl->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
    }
    else if (lbl == rangeLabelMaxRight.get())  // Max right
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
	if (sldr == leftLowSlider.get())
	{
        // Illegal attempt to get left range over right range
        if (sldr->getValue() > rightLowSlider->getValue())
        {
            setRangeLeftLow (float (rightLowSlider->getValue()));
            sldr->setValue (double (rightLowSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMinLeft.get());
            rangeLabelMinLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
            //return;
        }
        else // Normal drag
        {
            setRangeLeftLow (float (sldr->getValue()));
            updateLabelBounds (rangeLabelMinLeft.get());
            rangeLabelMinLeft->setText (String (int (getRangeLeftLow())) + valueUnit, dontSendNotification);
	    }

	    // In case the other thumb is dragged along..
		if (leftHighSlider->getThumbBeingDragged() == -1 && rangeLeftLow.getValue() > rangeLeftHigh.getValue())
		{
			setRangeLeftHigh (float(sldr->getValue()), true);
            leftHighSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMaxLeft.get());
			rangeLabelMaxLeft->setText(String(int(getRangeLeftHigh())) + valueUnit, dontSendNotification);
		}
				
	}

	// max left value changed
	else if (sldr == leftHighSlider.get())
	{
        // Illegal attempt to get left range over right range
        if (leftHighSlider->getValue() > rightLowSlider->getValue())
        {
            setRangeLeftHigh (float(rightLowSlider->getValue()));
            sldr->setValue (double (rightLowSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMaxLeft.get());
            rangeLabelMaxLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
            //return;
        }

        // normal case
		setRangeLeftHigh (float (sldr->getValue()));
        updateLabelBounds (rangeLabelMaxLeft.get());
	    rangeLabelMaxLeft->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
	        
	    // in case the other thumb is dragged along..
		if (leftLowSlider->getThumbBeingDragged() == -1 && rangeLeftLow.getValue() > rangeLeftHigh.getValue())
		{
            setRangeLeftLow (float(sldr->getValue()), true);
            leftLowSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMinLeft.get());
			rangeLabelMinLeft->setText(String(int(getRangeLeftLow())) + valueUnit, dontSendNotification);
		}
	}

	// min right value changed
	if (sldr == rightLowSlider.get())
	{
        // Illegal attempt to get right range over left range
        if (leftHighSlider->getValue() > rightLowSlider->getValue())
        {
            setRangeRightLow (float(leftHighSlider->getValue()));
            sldr->setValue (double (leftHighSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMinRight.get());
            rangeLabelMinRight->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
            //return;
        }

        // Normal case
		setRangeRightLow (float (sldr->getValue()));
        updateLabelBounds (rangeLabelMinRight.get());
	    rangeLabelMinRight->setText (String (int (getRangeRightLow())) + valueUnit, dontSendNotification);
	        
	    // in case the other thumb is dragged along..
		if (rightHighSlider->getThumbBeingDragged() == -1 && rangeRightLow.getValue() > rangeRightHigh.getValue())
		{
            setRangeRightHigh (float(sldr->getValue()), true);
            rightHighSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMaxRight.get());
			rangeLabelMaxRight->setText(String(int(getRangeRightHigh())) + valueUnit, dontSendNotification);
		}
	}

	// max right value changed
	else if (sldr == rightHighSlider.get())
	{
        // Illegal attempt to get right range over left range
        if (leftHighSlider->getValue() > sldr->getValue())
        {
            setRangeRightHigh (float(leftHighSlider->getValue()));
            sldr->setValue (double (leftHighSlider->getValue()), dontSendNotification);
            updateLabelBounds (rangeLabelMaxRight.get());
            rangeLabelMaxRight->setText (String (int (getRangeLeftHigh())) + valueUnit, dontSendNotification);
            //return;
        }
        else // Normal drag
        {
    		setRangeRightHigh (float (sldr->getValue()));
            updateLabelBounds (rangeLabelMaxRight.get());
    	    rangeLabelMaxRight->setText (String (int (getRangeRightHigh())) + valueUnit, dontSendNotification);
	    }  
	    // in case the other thumb is dragged along..
		if (rightLowSlider->getThumbBeingDragged() == -1 && rangeRightLow.getValue() > rangeRightHigh.getValue())
		{
			setRangeRightLow (float(sldr->getValue()), true);
            rightLowSlider->setValue (sldr->getValue(), dontSendNotification);
            updateLabelBounds (rangeLabelMinRight.get());
			rangeLabelMinRight->setText(String(int(getRangeRightLow())) + valueUnit, dontSendNotification);
		}
	}
}

void TwoRangeTuner::buttonClicked (Button* bttn)
{
    if (bttn == maxLeftAngleButton.get())
    {
        if (gestureRange.convertFrom0to1 (value) > getRangeRightLow())
        {
            leftLowSlider->setValue (getRangeRightLow(), sendNotification);
        }
        else
        {
            leftLowSlider->setValue (gestureRange.convertFrom0to1 (value), sendNotification);
        }
    }
    else if (bttn == minLeftAngleButton.get())
    {
        leftHighSlider->setValue (gestureRange.convertFrom0to1 (value), sendNotification);
    }
    
    else if (bttn == minRightAngleButton.get())
    {
        rightLowSlider->setValue (gestureRange.convertFrom0to1 (value), sendNotification);
    }
    else if (bttn == maxRightAngleButton.get())
    {
        if (gestureRange.convertFrom0to1 (value) < getRangeLeftHigh())
        {
            rightHighSlider->setValue (getRangeLeftHigh(), sendNotification);
        }
        else
        {
            rightHighSlider->setValue (gestureRange.convertFrom0to1 (value), sendNotification);
        }
    }
}

void TwoRangeTuner::buttonStateChanged (Button* btn) {

    if (btn->isOver ()) {

        if (btn == maxLeftAngleButton.get())
            maxLeftAngleBtnIsHovered = true;
        else if (btn == minLeftAngleButton.get())
            minLeftAngleBtnIsHovered = true;
        else if (btn == maxRightAngleButton.get())
            maxRightAngleBtnIsHovered = true;
        else if (btn == minRightAngleButton.get())
            minRightAngleBtnIsHovered = true;
        else
            return;

        repaint ();
    }
    else {
        maxLeftAngleBtnIsHovered = false;
        minLeftAngleBtnIsHovered = false;
        maxRightAngleBtnIsHovered = false;
        minRightAngleBtnIsHovered = false;
        repaint ();
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
        rangeLeftLow.beginChangeGesture();
        leftLowSlider->mouseDown (e.getEventRelativeTo (leftLowSlider.get()));
        rangeLabelMinLeft->setVisible (true);
    }
    else if (objectBeingDragged == leftHighThumb)
    {
        rangeLeftHigh.beginChangeGesture();
        leftHighSlider->mouseDown (e.getEventRelativeTo (leftHighSlider.get()));
        rangeLabelMaxLeft->setVisible (true);
    }
    else if (objectBeingDragged == rightLowThumb)
    {
        rangeRightLow.beginChangeGesture();
        rightLowSlider->mouseDown (e.getEventRelativeTo (rightLowSlider.get()));
        rangeLabelMinRight->setVisible (true);
    }
    else if (objectBeingDragged == rightHighThumb)
    {
        rangeRightHigh.beginChangeGesture();
        rightHighSlider->mouseDown (e.getEventRelativeTo (rightHighSlider.get()));
        rangeLabelMaxRight->setVisible (true);
    }
    else if (objectBeingDragged == middleAreaLeft)
    {
        rangeLeftLow.beginChangeGesture();
        rangeLeftHigh.beginChangeGesture();
        leftLowSlider->setSliderStyle (Slider::RotaryHorizontalDrag);
        leftHighSlider->setSliderStyle (Slider::RotaryHorizontalDrag);

        leftLowSlider->mouseDown (e.getEventRelativeTo (leftLowSlider.get()));
        leftHighSlider->mouseDown (e.getEventRelativeTo (leftHighSlider.get()));
        rangeLabelMinLeft->setVisible (true);
        rangeLabelMaxLeft->setVisible (true);
    }
    else if (objectBeingDragged == middleAreaRight)
    {
        rangeRightLow.beginChangeGesture();
        rangeRightHigh.beginChangeGesture();
        rightLowSlider->setSliderStyle (Slider::RotaryHorizontalDrag);
        rightHighSlider->setSliderStyle (Slider::RotaryHorizontalDrag);

        rightLowSlider->mouseDown (e.getEventRelativeTo (rightLowSlider.get()));
        rightHighSlider->mouseDown (e.getEventRelativeTo (rightHighSlider.get()));
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

    if (objectBeingDragged == leftLowThumb)        leftLowSlider->mouseDrag (e.getEventRelativeTo (leftLowSlider.get()));
    else if (objectBeingDragged == leftHighThumb)  leftHighSlider->mouseDrag (e.getEventRelativeTo (leftHighSlider.get()));
    else if (objectBeingDragged == rightLowThumb)  rightLowSlider->mouseDrag (e.getEventRelativeTo (rightLowSlider.get()));
    else if (objectBeingDragged == rightHighThumb) rightHighSlider->mouseDrag (e.getEventRelativeTo (rightHighSlider.get()));
    
    else if (objectBeingDragged == middleAreaLeft)
    {
        leftLowSlider->mouseDrag (e.getEventRelativeTo (leftLowSlider.get()));
        leftHighSlider->mouseDrag (e.getEventRelativeTo (leftHighSlider.get()));
    }
    else if (objectBeingDragged == middleAreaRight)
    {
        rightLowSlider->mouseDrag (e.getEventRelativeTo (rightLowSlider.get()));
        rightHighSlider->mouseDrag (e.getEventRelativeTo (rightHighSlider.get()));
    }

    repaint();
}
void TwoRangeTuner::mouseUp (const MouseEvent& e)
{
    if (objectBeingDragged == middleAreaLeft)
    {
        rangeLeftLow.endChangeGesture();
        rangeLeftHigh.endChangeGesture();
        leftLowSlider->mouseUp (e.getEventRelativeTo (leftLowSlider.get()));
        leftHighSlider->mouseUp (e.getEventRelativeTo (leftHighSlider.get()));
        leftLowSlider->setSliderStyle (Slider::Rotary);
        leftHighSlider->setSliderStyle (Slider::Rotary);
    }
    else if (objectBeingDragged == middleAreaRight)
    {
        rangeRightLow.endChangeGesture();
        rangeRightHigh.endChangeGesture();
        rightLowSlider->mouseUp (e.getEventRelativeTo (rightLowSlider.get()));
        rightHighSlider->mouseUp (e.getEventRelativeTo (rightHighSlider.get()));
        rightLowSlider->setSliderStyle (Slider::Rotary);
        rightHighSlider->setSliderStyle (Slider::Rotary);
    }
    else if (objectBeingDragged == leftLowThumb)
    {
        rangeLeftLow.endChangeGesture();
        leftLowSlider->mouseUp (e.getEventRelativeTo (leftLowSlider.get()));
    }
    else if (objectBeingDragged == leftHighThumb)
    {
        rangeLeftHigh.endChangeGesture();
        leftHighSlider->mouseUp (e.getEventRelativeTo (leftHighSlider.get()));
    }
    else if (objectBeingDragged == rightLowThumb)
    {
        rangeRightLow.endChangeGesture();
        rightLowSlider->mouseUp (e.getEventRelativeTo (rightLowSlider.get()));
    }
    else if (objectBeingDragged == rightHighThumb)
    {
        rangeRightHigh.endChangeGesture();
        rightHighSlider->mouseUp (e.getEventRelativeTo (rightHighSlider.get()));
    }

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
    leftLowSlider.reset (new Slider ("Range Slider Left Low"));
    Tuner::addAndMakeVisible (*leftLowSlider);

    leftHighSlider.reset (new Slider ("Range Slider Left High"));
    Tuner::addAndMakeVisible (*leftHighSlider);

    rightLowSlider.reset (new Slider ("Range Slider Right Low"));
    Tuner::addAndMakeVisible (*rightLowSlider);

    rightHighSlider.reset (new Slider ("Range Slider Right High"));
    Tuner::addAndMakeVisible (*rightHighSlider);

    auto setSliderSettings = [this] (Slider& slider, float valueToSet)
    {
        slider.setSliderStyle (Slider::Rotary);
        slider.setRotaryParameters (startAngle, endAngle, true);
        slider.setSliderSnapsToMousePosition (false);
        slider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        slider.setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
        slider.setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
        slider.setRange (double (parameterMax.getRange().getStart()), double (parameterMax.getRange().getEnd()), 1.0);
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
    rangeLabelMinLeft.reset (new Label ("Min Left Label",
                                        TRANS (String (int(getRangeLeftLow())) + valueUnit)));
    addAndMakeVisible (*rangeLabelMinLeft);
    rangeLabelMaxLeft.reset (new Label ("Max Left Label",
                                        TRANS (String(int(getRangeLeftHigh())) + valueUnit)));
    addAndMakeVisible (*rangeLabelMaxLeft);
    rangeLabelMinRight.reset (new Label ("Min Right Label",
                                        TRANS (String(int(getRangeRightLow())) + valueUnit)));
    addAndMakeVisible (*rangeLabelMinRight);
    rangeLabelMaxRight.reset (new Label ("Max Right Label",
                                        TRANS (String(int(getRangeRightHigh())) + valueUnit)));
    addAndMakeVisible (*rangeLabelMaxRight);
    
    auto setLabelSettings = [this] (Label& label)
    {
        label.setEditable (true, false, false);
        label.setFont (PLUME::font::plumeFont.withHeight (13.0f));
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

void TwoRangeTuner::createButtons()
{
    minLeftAngleButton.reset (new TextButton ("MinLeft Angle Button"));
    addAndMakeVisible (*minLeftAngleButton);
    maxLeftAngleButton.reset (new TextButton ("MaxLeft Angle Button"));
    addAndMakeVisible (*maxLeftAngleButton);
    minRightAngleButton.reset (new TextButton ("MinRight Angle Button"));
    addAndMakeVisible (*minRightAngleButton);
    maxRightAngleButton.reset (new TextButton ("MaxRight Angle Button"));
    addAndMakeVisible (*maxRightAngleButton);

    auto setButtonSettings = [this] (TextButton& button)
    {
        button.setColour (TextButton::buttonColourId , getPlumeColour (tunerButtonFill));
        button.setColour (TextButton::buttonOnColourId , tunerColour);
        button.setColour (TextButton::textColourOffId , getPlumeColour (detailPanelMainText));
        button.setColour (TextButton::textColourOnId , getPlumeColour (detailPanelMainText));
        button.setButtonText (&button == minLeftAngleButton.get() || &button == minRightAngleButton.get() ? "MIN ANGLE"
                                                                                              : "MAX ANGLE");
        button.addListener (this);
    };

    setButtonSettings (*minLeftAngleButton);
    setButtonSettings (*maxLeftAngleButton);
    setButtonSettings (*minRightAngleButton);
    setButtonSettings (*maxRightAngleButton);
}
    
void TwoRangeTuner::setRangeLeftLow (float val, const bool createChangeGesture)
{
    if (createChangeGesture) rangeLeftLow.beginChangeGesture();
    rangeLeftLow.setValueNotifyingHost (parameterMax.convertTo0to1 (val));
    if (createChangeGesture) rangeLeftLow.endChangeGesture();
}
    
void TwoRangeTuner::setRangeLeftHigh (float val, const bool createChangeGesture)
{
    if (createChangeGesture) rangeLeftHigh.beginChangeGesture();
    rangeLeftHigh.setValueNotifyingHost (parameterMax.convertTo0to1 (val));
    if (createChangeGesture) rangeLeftHigh.endChangeGesture();
}
    
void TwoRangeTuner::setRangeRightLow (float val, const bool createChangeGesture)
{
    if (createChangeGesture) rangeRightLow.beginChangeGesture();
    rangeRightLow.setValueNotifyingHost (parameterMax.convertTo0to1 (val));
    if (createChangeGesture) rangeRightLow.endChangeGesture();
}
    
void TwoRangeTuner::setRangeRightHigh (float val, const bool createChangeGesture)
{
    if (createChangeGesture) rangeRightHigh.beginChangeGesture();
    rangeRightHigh.setValueNotifyingHost (parameterMax.convertTo0to1 (val));
    if (createChangeGesture) rangeRightHigh.endChangeGesture();
}
    
float TwoRangeTuner::getRangeLeftLow()
{
    return parameterMax.convertFrom0to1 (rangeLeftLow.getValue());
}
    
float TwoRangeTuner::getRangeLeftHigh()
{
    return parameterMax.convertFrom0to1 (rangeLeftHigh.getValue());
}
    
float TwoRangeTuner::getRangeRightLow()
{
    return parameterMax.convertFrom0to1 (rangeRightLow.getValue());
}
    
float TwoRangeTuner::getRangeRightHigh()
{
    return parameterMax.convertFrom0to1 (rangeRightHigh.getValue());
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
        case leftLowThumb: slider = leftLowSlider.get(); break;
        case leftHighThumb: slider = leftHighSlider.get(); break;
        case rightLowThumb: slider = rightLowSlider.get(); break;
        case rightHighThumb: slider = rightHighSlider.get(); break;
        default: return -0.01;
    }

    return startAngle + float ((slider->getValue() - slider->getMinimum())/slider->getRange().getLength())
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

    float leftHighAngle = jmax (float (getThumbAngleRadians (leftLowThumb)), float (getThumbAngleRadians (leftHighThumb)));
    float rightLowAngle = jmin (float (getThumbAngleRadians (rightLowThumb)), float (getThumbAngleRadians (rightHighThumb)));
    
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
    Slider* lowSlider = leftNotRight ? leftLowSlider.get() : rightLowSlider.get();
    Slider* highSlider = leftNotRight ? leftHighSlider.get() : rightHighSlider.get();

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
    auto outline = getPlumeColour (tunerSliderBackground);
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
    backgroundArc.addCentredArc (float (sliderCentre.x),
                                 float (sliderCentre.y),
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
        valueArcLeft.addCentredArc (float (sliderCentre.x),
                                    float (sliderCentre.y),
                                    arcRadius,
                                    arcRadius,
                                    0.0f,
                                    float (leftLowAngle),
                                    float (leftHighAngle),
                                    true);

        g.setColour (fillLeft);
        g.strokePath (valueArcLeft, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));

        valueArcRight.addCentredArc (float (sliderCentre.x),
                                     float (sliderCentre.y),
                                     arcRadius,
                                     arcRadius,
                                     0.0f,
                                     float (rightLowAngle),
                                     float (rightHighAngle),
                                     true);

        g.setColour (fillRight);
        g.strokePath (valueArcRight, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));

    }

    if (objectBeingDragged != none && objectBeingDragged != middleAreaLeft
                                   && objectBeingDragged != middleAreaRight)
    {
        auto angle = getThumbAngleRadians (objectBeingDragged);

        juce::Point<float> thumbPoint (float (sliderCentre.x) + arcRadius * float (std::cos (angle - MathConstants<double>::halfPi)),
                                 float (sliderCentre.y) + arcRadius * float (std::sin (angle - MathConstants<double>::halfPi)));

        g.setColour (tunerColour.withAlpha (0.6f));
        g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
    }

        // Add highlight on tuner thumb when min/max button are hoverred
    if (maxLeftAngleBtnIsHovered || minLeftAngleBtnIsHovered || maxRightAngleBtnIsHovered || minRightAngleBtnIsHovered) {

        double angle;

        if (maxLeftAngleBtnIsHovered)
            angle = getThumbAngleRadians (leftLowThumb);
        else if (minLeftAngleBtnIsHovered)
            angle = getThumbAngleRadians (leftHighThumb);
        else if (maxRightAngleBtnIsHovered)
            angle = getThumbAngleRadians (rightHighThumb);
        else
            angle = getThumbAngleRadians (rightLowThumb);

        juce::Point<float> thumbPoint (sliderCentre.x + arcRadius * std::cos (angle - MathConstants<float>::halfPi),
            sliderCentre.y + arcRadius * std::sin (angle - MathConstants<float>::halfPi));

        g.setColour (tunerColour.withAlpha (0.3f));
        g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
    }
}

void TwoRangeTuner::updateLabelBounds (Label* labelToUpdate)
{
    int radius;
    float angle;

    if (labelToUpdate == rangeLabelMinLeft.get())
    {
        radius = int (sliderRadius) - 20;
        angle = float (getThumbAngleRadians (leftLowThumb));
    }
    else if (labelToUpdate == rangeLabelMaxLeft.get())
    {
        radius = int (sliderRadius) + 15;
        angle = float (getThumbAngleRadians (leftHighThumb));
    }
    else if (labelToUpdate == rangeLabelMinRight.get())
    {
        radius = int (sliderRadius) - 20;
        angle = float (getThumbAngleRadians (rightLowThumb));
    }
    else if (labelToUpdate == rangeLabelMaxRight.get())
    {
        radius = int (sliderRadius) + 15;
        angle = float (getThumbAngleRadians (rightHighThumb));
    }
    else return;
    
    labelToUpdate->setCentrePosition (sliderCentre.x + int (radius * std::cos (angle - MathConstants<float>::halfPi)),
                                      sliderCentre.y + int (radius * std::sin (angle - MathConstants<float>::halfPi)));
}

float TwoRangeTuner::getValueAngle()
{
    float convertedValue = gestureRange.convertFrom0to1 (value);
    float cursorAngle;

    if (gestureRange.getRange().getLength() > 0)
    {
        // Cursor stays at same angle if value out of range.
        if (convertedValue <= parameterMax.getRange().getStart() || convertedValue >= parameterMax.getRange().getEnd())
        {
            float startAngleModulo = startAngle;
            float endAngleModulo = endAngle;

            while (startAngleModulo > MathConstants<float>::twoPi) startAngleModulo -= MathConstants<float>::twoPi;
            while (endAngleModulo > MathConstants<float>::twoPi)   endAngleModulo   -= MathConstants<float>::twoPi;

            if (previousCursorAngle == startAngleModulo || previousCursorAngle == endAngleModulo)
            {
                return previousCursorAngle;
            }
        }
        
        if (convertedValue <= parameterMax.getRange().getStart())    cursorAngle = startAngle;
        else if (convertedValue >= parameterMax.getRange().getEnd()) cursorAngle = endAngle;
        else
        {
            cursorAngle = startAngle + (convertedValue - parameterMax.getRange().getStart()) * (endAngle - startAngle)
                                        / parameterMax.getRange().getLength();
        }
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
    juce::Point<float> cursorPoint (sliderCentre.x + cursorRadius * std::cos (cursorAngle - MathConstants<float>::halfPi),
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
    juce::Point<float> point (sliderCentre.x + sliderRadius * std::cos (angleRadian - MathConstants<float>::halfPi),
                        sliderCentre.y + sliderRadius * std::sin (angleRadian - MathConstants<float>::halfPi));

    g.drawLine (Line<float> (sliderCentre.toFloat(), point), 1.0f);
}

void TwoRangeTuner::drawThumbsAndToleranceLines (Graphics& g)
{
    double leftTolerance = ((leftHighSlider->getValue() - leftLowSlider->getValue())*double (std::abs (endAngle - startAngle)))
                         / (leftLowSlider->getRange().getLength() * 5);

    double rightTolerance = ((rightHighSlider->getValue() - rightLowSlider->getValue())*double (std::abs (endAngle - startAngle)))
                         / (rightLowSlider->getRange().getLength() * 5);

    g.setColour (tunerColour.withAlpha (0.5f));
    drawLineFromSliderCentre (g, float (getThumbAngleRadians (leftLowThumb)));
    drawLineFromSliderCentre (g, float (getThumbAngleRadians (leftHighThumb)));
    drawLineFromSliderCentre (g, float (getThumbAngleRadians (rightLowThumb)));
    drawLineFromSliderCentre (g, float (getThumbAngleRadians (rightHighThumb)));

    g.setColour (Colour (0xff903030));
    bool invertTolerance = startAngle > endAngle;
    drawLineFromSliderCentre (g, float (getThumbAngleRadians (leftLowThumb) + (invertTolerance ? -leftTolerance
                                                                                    : leftTolerance)));
    drawLineFromSliderCentre (g, float (getThumbAngleRadians (leftHighThumb) + (invertTolerance ? leftTolerance
                                                                                     : -leftTolerance)));
    drawLineFromSliderCentre (g, float (getThumbAngleRadians (rightLowThumb) + (invertTolerance ? -rightTolerance
                                                                                    : rightTolerance)));
    drawLineFromSliderCentre (g, float (getThumbAngleRadians (rightHighThumb) + (invertTolerance ? rightTolerance
                                                                                     : -rightTolerance)));
}
