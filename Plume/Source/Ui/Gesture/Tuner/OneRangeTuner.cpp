/*
  ==============================================================================

    Tuner.cpp
    Created: 30 Nov 2018 4:16:50pm
    Author:  Alex

  ==============================================================================
*/

#include "OneRangeTuner.h"


OneRangeTuner::OneRangeTuner(const std::atomic<float>& val, NormalisableRange<float> gestRange,
                             float& rangeL, float& rangeH, const NormalisableRange<float> paramMax,
                             const String unit, TunerStyle style)
    : Tuner (unit),
      value (val), gestureRange (gestRange),
      rangeLow (rangeL), rangeHigh (rangeH), parameterMax (paramMax),
      tunerStyle (style)
{
    setLookAndFeel (&oneRangeTunerLookAndFeel);
    setStyle (style);
    createSliders();
    createLabels();
    createButtons();
}

OneRangeTuner::~OneRangeTuner()
{
    setLookAndFeel (nullptr);

    lowSlider = nullptr;
    highSlider = nullptr;
    rangeLabelMin = nullptr;
    rangeLabelMax = nullptr;
}
    
//==============================================================================
void OneRangeTuner::paint (Graphics& g)
{
    drawTunerSliderBackground (g);
    drawValueCursor (g);
}

void OneRangeTuner::resized()
{
    // Sets bounds and changes the slider and labels position
    sliderBounds = getLocalBounds().reduced (PLUME::UI::MARGIN).translated (0, jmax (20, getHeight()/8));
    resizeSliders();
    resizeButtons();

    updateLabelBounds (rangeLabelMin.get());
    updateLabelBounds (rangeLabelMax.get());

    repaint();
}

void OneRangeTuner::resizeSliders()
{
    switch (tunerStyle)
    {
        case TunerStyle::wave:
            sliderRadius = jmin (sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
            sliderCentre = {getLocalBounds().getCentreX() - (int) sliderRadius/2,
                            sliderBounds.getCentreY()};
            break;

        case TunerStyle::tilt:
            sliderRadius = jmin (sliderBounds.getWidth()*2/3.0f, sliderBounds.getHeight()*2/3.0f);
            sliderCentre = {getLocalBounds().getCentreX() + (int) sliderRadius/2,
                            sliderBounds.getCentreY() + (int) sliderRadius/2};
            break;

        case TunerStyle::roll:
            sliderRadius = jmin (sliderBounds.getWidth()/2.0f, sliderBounds.getHeight()/2.0f);
            sliderCentre = {getLocalBounds().getCentreX(),
                            sliderBounds.getCentreY() + (int) sliderRadius/2};
            break;
    }

    juce::Rectangle<int> adjustedBounds = sliderBounds.withWidth (static_cast<int> (sliderRadius)*2)
                                                      .withHeight (static_cast<int> (sliderRadius)*2)
                                                      .expanded (10)
                                                      .withCentre (sliderCentre);

    lowSlider->setBounds (adjustedBounds);
    highSlider->setBounds (adjustedBounds);
}

void OneRangeTuner::resizeButtons()
{
    using namespace PLUME::UI;

    auto buttonsArea = getLocalBounds().withLeft (getLocalBounds().getRight() - 70)
                                       .withHeight (60)
                                       .reduced (MARGIN);

    maxAngleButton->setBounds (buttonsArea.removeFromTop (buttonsArea.getHeight()/2).withTrimmedBottom (MARGIN/2));
    minAngleButton->setBounds (buttonsArea.withTrimmedTop (MARGIN/2));
}
    
void OneRangeTuner::updateComponents()
{
    if (rangeLow < rangeHigh)
    {
        // Sets slider value
        if (lowSlider->getThumbBeingDragged() == -1)
        {
            lowSlider->setValue (double (getRangeLow()), dontSendNotification);
        }

        if (highSlider->getThumbBeingDragged() == -1)
        {
            highSlider->setValue (double (getRangeHigh()), dontSendNotification);
        }
        
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

void OneRangeTuner::updateComponents (OneRangeTuner::DraggableObject thumbThatShouldUpdate)
{
    if (thumbThatShouldUpdate == DraggableObject::lowThumb)
    {
        // Sets slider value
        if (lowSlider->getThumbBeingDragged() == -1)
        {
            if (highSlider->getThumbBeingDragged() == -1 && rangeLow > rangeHigh)
            {
                setRangeHigh (getRangeLow(), true);

                // Allows the DAW to update the value without using slider->setValue() with a notification
                // The latter causes crashes on Ableton Live
                sliderValueChanged (highSlider.get());
            }

            lowSlider->setValue (double (getRangeLow()), dontSendNotification);
        }
    }
    else if (thumbThatShouldUpdate == DraggableObject::highThumb)
    {
        // Sets slider value
        if (highSlider->getThumbBeingDragged() == -1)
        {
            if (lowSlider->getThumbBeingDragged() == -1 && rangeLow > rangeHigh)
            {
                setRangeLow (getRangeHigh(), true);

                // Allows the DAW to update the value without using slider->setValue() with a notification
                // The latter causes crashes on Ableton Live
                sliderValueChanged (lowSlider.get());
            }

            highSlider->setValue (double (getRangeHigh()), dontSendNotification);
        }
    }
    else
    {
        updateComponents();
    }
}

void OneRangeTuner::updateDisplay()
{
    if (getValueAngle() != previousCursorAngle)
    {
        switch (tunerStyle)
        {
            case TunerStyle::tilt:
                repaint (lowSlider->getBounds().withTrimmedBottom (lowSlider->getHeight()*6/10)
                                               .withTrimmedRight (lowSlider->getWidth()*4/10)
                                               .translated (0, -10));
                break;

            case TunerStyle::roll:
                repaint (lowSlider->getBounds().withTrimmedBottom (lowSlider->getHeight()*6/10)
                                               .translated (0, -15));
                break;
        }
    }
}

void OneRangeTuner::setColour (const Colour newColour)
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

    setLabelColours (*rangeLabelMin);
    setLabelColours (*rangeLabelMax);

    repaint();
}
    
void OneRangeTuner::setStyle (TunerStyle newStyle)
{
    tunerStyle = newStyle;

    switch (newStyle)
    {
        case TunerStyle::wave:
            setAngles (0.0f, MathConstants<float>::pi);
            tunerColour = getPlumeColour (waveHighlight);
            break;

        case TunerStyle::tilt:
            setAngles (MathConstants<float>::pi*16/10, MathConstants<float>::twoPi);
            tunerColour = getPlumeColour (tiltHighlight);
            break;

        case TunerStyle::roll:
            setAngles (MathConstants<float>::pi*5/3, MathConstants<float>::pi*7/3);
            tunerColour = getPlumeColour (rollHighlight);
            break;
    }
}
void OneRangeTuner::setAngles (float start, float end)
{
    startAngle = start;
    endAngle = end;
}

//==============================================================================
void OneRangeTuner::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only (and unit)
    if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
    {
        if (lbl == rangeLabelMin.get())       lbl->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMax.get())  lbl->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);

        return;
    }
        
    float val;
        
    // Gets the float value of the text 
    if (valueUnit != "" &&
        lbl->getText().endsWith(valueUnit)) val = lbl->getText().upToFirstOccurrenceOf(valueUnit, false, false)
                                                                .getFloatValue();
    else                                    val = lbl->getText().getFloatValue();
        
    if (val < parameterMax.getRange().getStart())    val = parameterMax.getRange().getStart();
    else if (val > parameterMax.getRange().getEnd()) val = parameterMax.getRange().getEnd();
        
    // Sets slider and labels accordingly
    if (lbl == rangeLabelMin.get())
    {
        if ( val > getRangeHigh()) val = getRangeHigh();
            
        setRangeLow (val);
        lowSlider->setValue (getRangeLow(), dontSendNotification);
        updateLabelBounds (rangeLabelMin.get());
        lbl->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
    }
    
    else if (lbl == rangeLabelMax.get())
    {
        if ( val < getRangeLow()) val = getRangeLow();
            
        setRangeHigh (val);
        highSlider->setValue (getRangeHigh(), dontSendNotification);
        updateLabelBounds (rangeLabelMax.get());
        lbl->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);
    }
}

void OneRangeTuner::editorHidden (Label* lbl, TextEditor&)
{
    lbl->setVisible (false);
}
    
void OneRangeTuner::sliderValueChanged (Slider* sldr)
{   
    if (sldr == lowSlider.get())
    {
        // min value changed by user
        setRangeLow (float (lowSlider->getValue()));
        updateLabelBounds (rangeLabelMin.get());
        rangeLabelMin->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
        
        // in case the other thumb is dragged along..
        if (highSlider->getThumbBeingDragged() == -1 && rangeLow > rangeHigh)
        {
            setRangeHigh (float (lowSlider->getValue()), true);
            highSlider->setValue (double (getRangeLow()), dontSendNotification);
            updateLabelBounds (rangeLabelMax.get());
            rangeLabelMax->setText (String (float (sldr->getValue())) + valueUnit, dontSendNotification);
        }
    }

    // max value changed by user
    else if (sldr == highSlider.get())
    {
        setRangeHigh (float (highSlider->getValue()));
        updateLabelBounds (rangeLabelMax.get());
        rangeLabelMax->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);
        
        // in case the other thumb is dragged along..
        if (lowSlider->getThumbBeingDragged() == -1 && rangeLow > rangeHigh)
        {
            setRangeLow (float (highSlider->getValue()), true);
            lowSlider->setValue (double (getRangeLow()), dontSendNotification);
            updateLabelBounds (rangeLabelMin.get());
            rangeLabelMin->setText (String (float (sldr->getValue())) + valueUnit, dontSendNotification);
        }
    }
}

void OneRangeTuner::buttonClicked (Button* bttn)
{
    if (bttn == minAngleButton.get())
    {
        lowSlider->setValue (gestureRange.convertFrom0to1 (value), sendNotification);
    }

    else if (bttn == maxAngleButton.get())
    {
        highSlider->setValue (gestureRange.convertFrom0to1 (value), sendNotification);
    }
}

void OneRangeTuner::buttonStateChanged (Button* btn) {

    if (btn->isOver ()) {

        if (btn == maxAngleButton.get())
            maxAngleBtnIsHovered = true;
        else if (btn == minAngleButton.get())
            minAngleBtnIsHovered = true;
        else
            return;

        repaint ();
    }
    else {
        maxAngleBtnIsHovered = false;
        minAngleBtnIsHovered = false;
        repaint ();
    }
}

void OneRangeTuner::mouseDown (const MouseEvent& e)
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

void OneRangeTuner::handleSingleClick (const MouseEvent& e)
{
    objectBeingDragged = getObjectToDrag (e);
    
    if (objectBeingDragged == DraggableObject::lowThumb)
    {
        lowSlider->mouseDown (e.getEventRelativeTo (lowSlider.get()));
        rangeLabelMin->setVisible (true);
    }
    else if (objectBeingDragged == DraggableObject::highThumb)
    {
        highSlider->mouseDown (e.getEventRelativeTo (highSlider.get()));
        rangeLabelMax->setVisible (true);
    }
    else if (objectBeingDragged == DraggableObject::middleArea)
    {
        lowSlider->setSliderStyle (tunerStyle == TunerStyle::wave ? Slider::RotaryVerticalDrag
                                                      : tunerStyle == TunerStyle::tilt ? Slider::RotaryHorizontalVerticalDrag
                                                                           : Slider::RotaryHorizontalDrag);
        highSlider->setSliderStyle (tunerStyle == TunerStyle::wave ? Slider::RotaryVerticalDrag
                                                       : tunerStyle == TunerStyle::tilt ? Slider::RotaryHorizontalVerticalDrag
                                                                            : Slider::RotaryHorizontalDrag);

        lowSlider->mouseDown (e.getEventRelativeTo (lowSlider.get()));
        highSlider->mouseDown (e.getEventRelativeTo (highSlider.get()));
        rangeLabelMin->setVisible (true);
        rangeLabelMax->setVisible (true);
    }

    updateMouseCursor();
    repaint();
}

void OneRangeTuner::handleDoubleClick (const MouseEvent& e)
{
    if (getObjectToDrag (e) == DraggableObject::lowThumb)
    {
        rangeLabelMin->setVisible (true);
        rangeLabelMin->showEditor();
    }

    else if (getObjectToDrag (e) == DraggableObject::highThumb)
    {
        rangeLabelMax->setVisible (true);
        rangeLabelMax->showEditor();
    }
}

void OneRangeTuner::mouseDrag (const MouseEvent& e)
{

    if (!e.mods.isLeftButtonDown() || e.getNumberOfClicks() > 1) return;

    if (objectBeingDragged == DraggableObject::lowThumb)
    {
        lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider.get()));
    }
    else if (objectBeingDragged == DraggableObject::highThumb)
    {
        highSlider->mouseDrag (e.getEventRelativeTo (highSlider.get()));
    }
    else
    {

        if (tunerStyle == TunerStyle::tilt)
        {
            lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider.get()));
            highSlider->mouseDrag (e.getEventRelativeTo (highSlider.get()));
        }
        else
        {
            // Inverts the drag for the y axis
            auto invertedYEvent = e.withNewPosition(juce::Point<int> (e.x, e.getMouseDownY() - e.getDistanceFromDragStartY()));

            lowSlider->mouseDrag (invertedYEvent.getEventRelativeTo (lowSlider.get()));
            highSlider->mouseDrag (invertedYEvent.getEventRelativeTo (highSlider.get()));
        }

    }

    repaint();
}

void OneRangeTuner::mouseUp (const MouseEvent& e)
{
    if (objectBeingDragged == DraggableObject::lowThumb)
    {
        lowSlider->mouseUp (e.getEventRelativeTo (lowSlider.get()));
    }
    else if (objectBeingDragged == DraggableObject::highThumb)
    {
        highSlider->mouseUp (e.getEventRelativeTo (highSlider.get()));
    }
    else if (objectBeingDragged == DraggableObject::middleArea)
    {
        lowSlider->setSliderStyle (Slider::Rotary);
        highSlider->setSliderStyle (Slider::Rotary);

        lowSlider->mouseUp (e.getEventRelativeTo (lowSlider.get()));
        highSlider->mouseUp (e.getEventRelativeTo (highSlider.get()));
    }

    if (objectBeingDragged != DraggableObject::none)
    {
        rangeLabelMin->setVisible (false);
        rangeLabelMax->setVisible (false);
        objectBeingDragged = DraggableObject::none;
        updateMouseCursor();
        repaint();
    }
}

MouseCursor OneRangeTuner::getMouseCursor()
{
    return MouseCursor::NormalCursor;
}

void OneRangeTuner::createSliders()
{
    addAndMakeVisible (*(lowSlider = std::make_unique<Slider> ("Range Low Slider")));
    addAndMakeVisible (*(highSlider = std::make_unique<Slider> ("Range High Slider")));
    
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
	};

    setSliderSettings (*lowSlider, getRangeLow());
    setSliderSettings (*highSlider, getRangeHigh());
}
    
void OneRangeTuner::createLabels()
{
    addAndMakeVisible (*(rangeLabelMin = std::make_unique<Label> ("Min Label", TRANS (String(int(getRangeLow())) + valueUnit))));
    addAndMakeVisible (*(rangeLabelMax = std::make_unique<Label> ("Max Label", TRANS (String(int(getRangeHigh())) + valueUnit))));

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
    };

    setLabelSettings (*rangeLabelMin);
    setLabelSettings (*rangeLabelMax);
}

void OneRangeTuner::createButtons()
{
    addAndMakeVisible (*(minAngleButton = std::make_unique<TextButton> ("Min Angle Button")));
    addAndMakeVisible (*(maxAngleButton = std::make_unique<TextButton> ("Max Angle Button")));

    auto setButtonSettings = [this] (TextButton& button)
    {
        button.setColour (TextButton::buttonColourId , getPlumeColour (tunerButtonFill));
        button.setColour (TextButton::buttonOnColourId , tunerColour);
        button.setColour (TextButton::textColourOffId , getPlumeColour (detailPanelMainText));
        button.setColour (TextButton::textColourOnId , getPlumeColour (detailPanelMainText));
        button.setButtonText (&button == minAngleButton.get() ? "MIN ANGLE" : "MAX ANGLE");
        button.addListener (this);
    };

    setButtonSettings (*minAngleButton);
    setButtonSettings (*maxAngleButton);
}
    
void OneRangeTuner::setRangeLow (float val, const bool createChangeGesture)
{    
    rangeLow = parameterMax.convertTo0to1 (val);
}
    
void OneRangeTuner::setRangeHigh (float val, const bool createChangeGesture)
{        
    rangeHigh = parameterMax.convertTo0to1 (val);
}
    
float OneRangeTuner::getRangeLow()
{
    return parameterMax.convertFrom0to1 (rangeLow);
}
    
float OneRangeTuner::getRangeHigh()
{
    return parameterMax.convertFrom0to1 (rangeHigh);
}


double OneRangeTuner::getAngleFromMouseEventRadians (const MouseEvent& e)
{
    double angle =  std::atan2 (e.getMouseDownX() - sliderCentre.x,
                                -(e.getMouseDownY() - sliderCentre.y));

    if (angle < 0) angle += MathConstants<double>::twoPi;

    return angle;
}

double OneRangeTuner::getThumbAngleRadians (const DraggableObject thumb)
{
    if (thumb != DraggableObject::lowThumb && thumb != DraggableObject::highThumb) return -0.01;
    
    Slider& slider = (thumb == DraggableObject::lowThumb) ? *lowSlider : *highSlider;

    return startAngle + ((slider.getValue() - slider.getMinimum())/slider.getRange().getLength())
                             *(endAngle - startAngle);
}

OneRangeTuner::DraggableObject OneRangeTuner::getObjectToDrag (const MouseEvent& e)
{
    //================ Figures out the scenario surrounding the click ==============

    if (e.mods.isShiftDown()) return DraggableObject::middleArea;

    double mouseAngle = getAngleFromMouseEventRadians (e);
    bool inverted = startAngle > endAngle;


    // Computes "% 2*pi" if both angles are somehow greater that 2*pi
    float startAngleModulo = startAngle, endAngleModulo = endAngle;
    while (startAngleModulo > MathConstants<float>::twoPi
           && endAngleModulo > MathConstants<float>::twoPi)
    {
        startAngleModulo -= MathConstants<float>::twoPi;
        endAngleModulo -= MathConstants<float>::twoPi;
    }

    // If only one extreme angle is higher than 2*pi, the process needs a different logic.
    // Boolean "differentRef" notifies that this is the case.
    bool differentRef = jmax(startAngleModulo, endAngleModulo) > MathConstants<float>::twoPi
                        && jmin(startAngleModulo, endAngleModulo) < MathConstants<float>::twoPi;
    
    // If the higher angle minus 2pi is still higher that the smaller, then we fall back to the
    // previous logic with "inverted" angles.
    if (differentRef && jmax(startAngleModulo, endAngleModulo) - MathConstants<float>::twoPi
                        > jmin(startAngleModulo, endAngleModulo))
    {
        differentRef = false;
        inverted ? startAngleModulo -= MathConstants<float>::twoPi
                 : endAngleModulo -= MathConstants<float>::twoPi;
    }

    //================ Finds the object to drag ==============

    DraggableObject thumb1 = inverted ? DraggableObject::lowThumb : DraggableObject::highThumb;
    DraggableObject thumb2 = inverted ? DraggableObject::highThumb : DraggableObject::lowThumb;

    // The 4th of the angle between the two thumbs.
    double tolerance = ((highSlider->getValue() - lowSlider->getValue())*(std::abs (endAngle - startAngle)))
                                /(lowSlider->getRange().getLength()*5);

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
            else return DraggableObject::middleArea;
        }
        else
        {
            if (mouseAngle < jmax(startAngleModulo, endAngleModulo) - MathConstants<float>::twoPi)
                mouseAngle += MathConstants<float>::twoPi;

            if (mouseAngle > getThumbAngleRadians(thumb1) - tolerance)      return thumb1;
            else if (mouseAngle < getThumbAngleRadians(thumb2) + tolerance) return thumb2;
            else return DraggableObject::middleArea;
        }
    }
    
    return DraggableObject::none; // Somehow no thumb could be chosen...
}

void OneRangeTuner::drawTunerSliderBackground (Graphics& g)
{
    auto outline = getPlumeColour (tunerSliderBackground);
    auto fill    = objectBeingDragged == DraggableObject::middleArea ? tunerColour.interpolatedWith (Colour (0xffffffff), 0.8f)
                                                    : tunerColour;

    auto lowAngle = startAngle
                        + (lowSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                          * (endAngle - startAngle);
    auto highAngle = startAngle
                        + (highSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                           * (endAngle - startAngle);
    auto lineW = jmin (8.0f, sliderRadius * 0.5f);
    auto arcRadius = sliderRadius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (sliderCentre.toFloat().x,
                                 sliderCentre.toFloat().y,
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
        valueArc.addCentredArc (sliderCentre.toFloat().x,
                                sliderCentre.toFloat().y,
                                arcRadius,
                                arcRadius,
                                0.0f,
                                static_cast<float> (lowAngle),
                                static_cast<float> (highAngle),
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (6.0f, PathStrokeType::curved, PathStrokeType::rounded));
    }

    if (objectBeingDragged == DraggableObject::lowThumb || objectBeingDragged == DraggableObject::highThumb)
    {
        auto angle = getThumbAngleRadians (objectBeingDragged);

        juce::Point<float> thumbPoint (sliderCentre.toFloat().x + arcRadius * static_cast<float> (std::cos (angle - MathConstants<double>::halfPi)),
                                 sliderCentre.toFloat().y + arcRadius * static_cast<float> (std::sin (angle - MathConstants<double>::halfPi)));

        g.setColour (fill.withAlpha (0.6f));
        g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
    }

    // Add highlight on tuner thumb when min/max button are hoverred
    if (maxAngleBtnIsHovered || minAngleBtnIsHovered) {

        double angle;

        if (maxAngleBtnIsHovered)
            angle = getThumbAngleRadians (DraggableObject::highThumb);
        else
            angle = getThumbAngleRadians (DraggableObject::lowThumb);

        juce::Point<float> thumbPoint (sliderCentre.toFloat().x + arcRadius * static_cast<float> (std::cos(angle - MathConstants<double>::halfPi)),
            sliderCentre.toFloat().y + arcRadius * static_cast<float> (std::sin(angle - MathConstants<double>::halfPi)));

        g.setColour (fill.withAlpha (0.3f));
        g.fillEllipse (juce::Rectangle<float> (25.0f, 25.0f).withCentre (thumbPoint));
    }
}

void OneRangeTuner::updateLabelBounds (Label* labelToUpdate)
{
    if (labelToUpdate == nullptr) return;

    if (labelToUpdate == rangeLabelMin.get())
    {
        auto radius = sliderRadius + 15.0f;
        auto angle = getThumbAngleRadians (DraggableObject::lowThumb);

        rangeLabelMin->setCentrePosition (sliderCentre.x + static_cast<int> (radius * static_cast<float> (std::cos (angle - MathConstants<double>::halfPi))),
                                          sliderCentre.y + static_cast<int> (radius * static_cast<float> (std::sin (angle - MathConstants<double>::halfPi))));

    }
    else if (labelToUpdate == rangeLabelMax.get())
    {
        auto radius = sliderRadius + 15.0f;
        auto angle = getThumbAngleRadians (DraggableObject::highThumb);

        rangeLabelMax->setCentrePosition (sliderCentre.x + static_cast<int> (radius * static_cast<float> (std::cos (angle - MathConstants<double>::halfPi))),
                                          sliderCentre.y + static_cast<int> (radius * static_cast<float> (std::sin (angle - MathConstants<double>::halfPi))));
    }
}

float OneRangeTuner::getValueAngle()
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

void OneRangeTuner::drawValueCursor (Graphics& g)
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

    g.setColour ((gestureRange.convertFrom0to1 (value) >= getRangeLow())
                 && (gestureRange.convertFrom0to1 (value) <= getRangeHigh()) ? tunerColour
                                                                             : Colour (0x80808080));
    g.fillPath (cursorPath, transform);
}

void OneRangeTuner::drawLineFromSliderCentre (Graphics& g, float angleRadian)
{
    juce::Point<float> point (sliderCentre.x + sliderRadius * std::cos (angleRadian - MathConstants<float>::halfPi),
                        sliderCentre.y + sliderRadius * std::sin (angleRadian - MathConstants<float>::halfPi));

    g.drawLine (Line<float> (sliderCentre.toFloat(), point), 1.0f);
}

void OneRangeTuner::drawThumbsAndToleranceLines (Graphics& g)
{
    double tolerance = ((highSlider->getValue() - lowSlider->getValue())*(std::abs (endAngle - startAngle)))
                         / (lowSlider->getRange().getLength() * 5);

    g.setColour (Colour (0xffdedeff));
    drawLineFromSliderCentre (g, static_cast<float> (getThumbAngleRadians (DraggableObject::lowThumb)));
    drawLineFromSliderCentre (g, static_cast<float> (getThumbAngleRadians (DraggableObject::highThumb)));

    g.setColour (Colour (0xff903030));
    bool invertTolerance = startAngle > endAngle;
    drawLineFromSliderCentre (g, static_cast<float> (getThumbAngleRadians (DraggableObject::lowThumb) + (invertTolerance ? -tolerance
                                                                                    : tolerance)));
    drawLineFromSliderCentre (g, static_cast<float> (getThumbAngleRadians (DraggableObject::highThumb) + (invertTolerance ? tolerance
                                                                                     : -tolerance)));
}
