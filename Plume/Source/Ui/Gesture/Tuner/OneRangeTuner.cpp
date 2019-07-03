/*
  ==============================================================================

    Tuner.cpp
    Created: 30 Nov 2018 4:16:50pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Tuner/OneRangeTuner.h"


OneRangeTuner::OneRangeTuner(const float& val, NormalisableRange<float> gestRange,
                RangedAudioParameter& rangeL, RangedAudioParameter& rangeH, const Range<float> paramMax,
                const String unit, bool show, TunerStyle style)
    : Tuner(val, gestRange, paramMax, unit, show),
        value (val), gestureRange (gestRange),
        rangeLow (rangeL), rangeHigh (rangeH), parameterMax (paramMax),
        tunerStyle (style)
{
    setLookAndFeel (&OneRangeTunerLookAndFeel);
    setStyle (style);
    createSliders();
    createLabels();
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
    sliderBounds = getLocalBounds().reduced (30);
    resizeSliders();

    updateLabelBounds (rangeLabelMin);
    updateLabelBounds (rangeLabelMax);

    repaint();
}

void OneRangeTuner::resizeSliders()
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
            sliderCentre = {getLocalBounds().getCentreX(),
                            sliderBounds.getCentreY() + (int) sliderRadius/2};
            break;
    }

    juce::Rectangle<int> adjustedBounds = sliderBounds.withWidth (sliderRadius*2)
                                                      .withHeight (sliderRadius*2)
                                                      .expanded (10)
                                                      .withCentre (sliderCentre);

    lowSlider->setBounds (adjustedBounds);
    highSlider->setBounds (adjustedBounds);
}
    
void OneRangeTuner::updateComponents()
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

void OneRangeTuner::updateDisplay()
{
    if (getValueAngle() != previousCursorAngle)
    {
        repaint();
    }
}
    
void OneRangeTuner::setStyle (TunerStyle newStyle)
{
    tunerStyle = newStyle;

    switch (newStyle)
    {
        case wave:
            setAngles (0.0f, MathConstants<float>::pi);
            tunerColour = Colour (0xff7c80de);
            break;

        case tilt:
            setAngles (0.0f, MathConstants<float>::pi*4/10);
            tunerColour = Colour (0xfff3a536);
            break;

        case roll:
            setAngles (MathConstants<float>::pi*5/3, MathConstants<float>::pi*7/3);
            tunerColour = Colour (0xffa255f3);
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
        if (lbl == rangeLabelMin)       lbl->setText (String (int (getRangeLow())) + valueUnit, dontSendNotification);
        else if (lbl == rangeLabelMax)  lbl->setText (String (int (getRangeHigh())) + valueUnit, dontSendNotification);

        return;
    }
        
    float val;
        
    // Gets the float value of the text 
    if (valueUnit != "" &&
        lbl->getText().endsWith(valueUnit)) val = lbl->getText().upToFirstOccurrenceOf(valueUnit, false, false)
                                                                .getFloatValue();
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
    
void OneRangeTuner::sliderValueChanged (Slider* sldr)
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

void OneRangeTuner::mouseDown (const MouseEvent& e)
{
    objectBeingDragged = getObjectToDrag (e);

    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseDown (e.getEventRelativeTo (lowSlider));
        rangeLabelMin->setVisible (true);
    }
    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseDown (e.getEventRelativeTo (highSlider));
        rangeLabelMax->setVisible (true);
    }
    else if (objectBeingDragged == middleArea)
    {
        lowSlider->setSliderStyle (tunerStyle == wave ? Slider::RotaryVerticalDrag
                                                      : tunerStyle == tilt ? Slider::RotaryHorizontalVerticalDrag
                                                                           : Slider::RotaryHorizontalDrag);
        highSlider->setSliderStyle (tunerStyle == wave ? Slider::RotaryVerticalDrag
                                                       : tunerStyle == tilt ? Slider::RotaryHorizontalVerticalDrag
                                                                            : Slider::RotaryHorizontalDrag);

        lowSlider->mouseDown (e.getEventRelativeTo (lowSlider));
        highSlider->mouseDown (e.getEventRelativeTo (highSlider));
        rangeLabelMin->setVisible (true);
        rangeLabelMax->setVisible (true);
    }

    updateMouseCursor();
    repaint();
}

void OneRangeTuner::mouseDrag (const MouseEvent& e)
{
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
        // Inverts the drag for the y axis
        auto invertedYEvent = e.withNewPosition(Point<int> (e.x,
                                                            e.getMouseDownY() - e.getDistanceFromDragStartY()));

        lowSlider->mouseDrag (invertedYEvent.getEventRelativeTo (lowSlider));
        highSlider->mouseDrag (invertedYEvent.getEventRelativeTo (highSlider));
    }

    repaint();
}

void OneRangeTuner::mouseUp (const MouseEvent& e)
{
    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseUp (e.getEventRelativeTo (lowSlider));
    }
    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseUp (e.getEventRelativeTo (highSlider));
    }
    else if (objectBeingDragged == middleArea)
    {
        lowSlider->setSliderStyle (Slider::Rotary);
        highSlider->setSliderStyle (Slider::Rotary);

        lowSlider->mouseUp (e.getEventRelativeTo (lowSlider));
        highSlider->mouseUp (e.getEventRelativeTo (highSlider));
    }

    if (objectBeingDragged != none)
    {
        rangeLabelMin->setVisible (false);
        rangeLabelMax->setVisible (false);
        objectBeingDragged = none;
        updateMouseCursor();
        repaint();
    }

}

MouseCursor OneRangeTuner::getMouseCursor()
{
    return MouseCursor::NormalCursor;

    switch (objectBeingDragged)
    {
        case (none): return MouseCursor::NormalCursor; break;
        default: return MouseCursor::NoCursor;
    }
}

void OneRangeTuner::createSliders()
{
    addAndMakeVisible (lowSlider = new Slider ("Range Low Slider"));
        
    // Slider style
    lowSlider->setSliderStyle (Slider::Rotary);
    lowSlider->setRotaryParameters (startAngle, endAngle, true);
    lowSlider->setSliderSnapsToMousePosition (false);
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
    highSlider->setSliderStyle (Slider::Rotary);
    highSlider->setRotaryParameters (startAngle, endAngle, true);
    highSlider->setSliderSnapsToMousePosition (false);
    highSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    highSlider->setColour (Slider::rotarySliderFillColourId, Colour (0x00000000));
    highSlider->setColour (Slider::rotarySliderOutlineColourId, Colour (0x00000000));
        
    // Slider values
    highSlider->setRange (double (parameterMax.getStart()), double (parameterMax.getEnd()), 1.0);
    highSlider->setValue (double (getRangeHigh()), dontSendNotification);
    highSlider->addListener (this);
    highSlider->setInterceptsMouseClicks (false, false);
}
    
void OneRangeTuner::createLabels()
{
    Tuner::addAndMakeVisible (rangeLabelMin = new Label ("Min Label", TRANS (String(int(getRangeLow())) + valueUnit)));
    Tuner::addAndMakeVisible (rangeLabelMax = new Label ("Max Label", TRANS (String(int(getRangeHigh())) + valueUnit)));

    // LabelMin style
    rangeLabelMin->setEditable (true, false, false);
    rangeLabelMin->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMin->setColour (Label::textColourId, tunerColour);
    rangeLabelMin->setJustificationType (Justification::centred);
    rangeLabelMin->setSize (30, 20);
        
    // LabelMax style
    rangeLabelMax->setEditable (true, false, false);
    rangeLabelMax->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
    rangeLabelMax->setColour (Label::textColourId, tunerColour);
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
    
void OneRangeTuner::setRangeLow (float val)
{
    rangeLow.beginChangeGesture();
    rangeLow.setValueNotifyingHost (rangeLow.convertTo0to1 (val));
    rangeLow.endChangeGesture();
}
    
void OneRangeTuner::setRangeHigh (float val)
{
    rangeHigh.beginChangeGesture();
    rangeHigh.setValueNotifyingHost (rangeHigh.convertTo0to1 (val));
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


double OneRangeTuner::getAngleFromMouseEventRadians (const MouseEvent& e)
{
    double angle =  std::atan2 (e.getMouseDownX() - sliderCentre.x,
                                -(e.getMouseDownY() - sliderCentre.y));

    if (angle < 0) angle += MathConstants<double>::twoPi;

    return angle;
}

double OneRangeTuner::getThumbAngleRadians (const DraggableObject thumb)
{
    if (thumb != lowThumb && thumb != highThumb) return -0.01;
    
    Slider& slider = (thumb == lowThumb) ? *lowSlider : *highSlider;

    return startAngle + ((slider.getValue() - slider.getMinimum())/slider.getRange().getLength())
                             *(endAngle - startAngle);
}

OneRangeTuner::DraggableObject OneRangeTuner::getObjectToDrag (const MouseEvent& e)
{
    if (e.mods.isShiftDown()) return middleArea;

    double mouseAngle = getAngleFromMouseEventRadians (e);
    bool inverted = startAngle > endAngle;

    // The 4th of the angle between the two thumbs.
    double tolerance = ((highSlider->getValue() - lowSlider->getValue())*(std::abs (endAngle - startAngle)))
                                /(lowSlider->getRange().getLength()*5);

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

    DraggableObject thumb1 = inverted ? lowThumb : highThumb;
    DraggableObject thumb2 = inverted ? highThumb : lowThumb;

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
    
    return none; // Somehow no thumb could be chosen...
}

void OneRangeTuner::drawTunerSliderBackground (Graphics& g)
{
    auto outline = Colour (0xff505050);
    auto fill    = objectBeingDragged == middleArea ? tunerColour.interpolatedWith (Colour (0xffffffff), 0.8f)
                                                    : tunerColour;

    auto lowAngle = startAngle
                        + (lowSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                          * (endAngle - startAngle);
    auto highAngle = startAngle
                        + (highSlider->getValue() - lowSlider->getMinimum()) / lowSlider->getRange().getLength()
                                                                           * (endAngle - startAngle);
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
}

void OneRangeTuner::updateLabelBounds (Label* labelToUpdate)
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

float OneRangeTuner::getValueAngle()
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

void OneRangeTuner::drawValueCursor (Graphics& g)
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

    g.setColour ((gestureRange.convertFrom0to1 (value) >= getRangeLow())
                 && (gestureRange.convertFrom0to1 (value) <= getRangeHigh()) ? tunerColour
                                                                             : Colour (0x80808080));
    g.fillPath (cursorPath, transform);
}

void OneRangeTuner::drawLineFromSliderCentre (Graphics& g, float angleRadian)
{
    Point<float> point (sliderCentre.x + sliderRadius * std::cos (angleRadian - MathConstants<float>::halfPi),
                        sliderCentre.y + sliderRadius * std::sin (angleRadian - MathConstants<float>::halfPi));

    g.drawLine (Line<float> (sliderCentre.toFloat(), point), 1.0f);
}

void OneRangeTuner::drawThumbsAndToleranceLines (Graphics& g)
{
    double tolerance = ((highSlider->getValue() - lowSlider->getValue())*(std::abs (endAngle - startAngle)))
                         / (lowSlider->getRange().getLength() * 5);

    g.setColour (Colour (0xffdedeff));
    drawLineFromSliderCentre (g, getThumbAngleRadians (lowThumb));
    drawLineFromSliderCentre (g, getThumbAngleRadians (highThumb));

    g.setColour (Colour (0xff903030));
    bool invertTolerance = startAngle > endAngle;
    drawLineFromSliderCentre (g, getThumbAngleRadians (lowThumb) + (invertTolerance ? -tolerance
                                                                                    : tolerance));
    drawLineFromSliderCentre (g, getThumbAngleRadians (highThumb) + (invertTolerance ? tolerance
                                                                                     : -tolerance));
}