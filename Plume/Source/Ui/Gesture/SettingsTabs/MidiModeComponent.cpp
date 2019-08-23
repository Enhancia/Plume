/*
  ==============================================================================

    MidiModeComponent.cpp
    Created: 14 Jun 2019 2:33:50pm
    Author:  Alex

  ==============================================================================
*/

#include "MidiModeComponent.h"

#ifndef W 
#define W Component::getWidth()
#endif

#ifndef H 
#define H Component::getHeight()
#endif

MidiModeComponent::MidiModeComponent (Gesture& gest, GestureArray& gestArray)
    : gesture (gest), gestureArray (gestArray)
{
    createComboBox();
    createLabels();

    addAndMakeVisible (midiRangeTuner = new MidiRangeTuner (gesture));
}

MidiModeComponent::~MidiModeComponent()
{
    midiTypeBox->removeListener (this);
    ccLabel->removeListener (this);
    
    midiTypeBox = nullptr;
    ccLabel = nullptr;
    midiRangeTuner = nullptr;
}

//==============================================================================
void MidiModeComponent::paint (Graphics& g)
{
    auto area = getLocalBounds().reduced (PLUME::UI::MARGIN)
                                .withTrimmedLeft (PLUME::UI::MARGIN);
    area.removeFromRight (getWidth()*2/3);

    g.setColour (getPlumeColour (detailPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight (13.0f));

    g.drawText ("MIDI Type:",
                area.removeFromTop (area.getHeight()/2),
                Justification::centredLeft, false);

    g.drawText ("MIDI Range:",
                area,
                Justification::centredLeft, false);
}

void MidiModeComponent::resized()
{
    using namespace PLUME::UI;

	auto area = getLocalBounds().reduced (MARGIN);
	area.removeFromLeft (area.getWidth()/3);

    auto typeArea = area.removeFromTop (getHeight()/2);
    midiTypeBox->setBounds (typeArea.removeFromLeft (typeArea.getWidth()/2)
    								.withSizeKeepingCentre (area.getWidth()*4/10, 20));
    ccLabel->setBounds (typeArea.withSizeKeepingCentre (area.getWidth()/4, 20));

    auto rangeArea = area;

    midiRangeTuner->setBounds (rangeArea.withSizeKeepingCentre (area.getWidth()*3/4 - 2*MARGIN, area.getHeight()));
}

//==============================================================================
void MidiModeComponent::labelTextChanged (Label* lbl)
{
    if (lbl == ccLabel)
    {
        // checks that the string is numbers only
        if (lbl->getText().containsOnly ("0123456789") == false)
        {
            lbl->setText (String (gesture.getCc()), dontSendNotification);
            return;
        }

        int val = lbl->getText().getIntValue();
    
        if (val < 0) val = 0;
        else if (val > 127) val = 127;
    
        gesture.setCc(val);

        lbl->setText (String(val), dontSendNotification);

        if (auto* parentComp = getParentComponent())
        	parentComp->repaint();
    }
    
    /*
    else if (lbl == rangeLabelMin || lbl == rangeLabelMax)
    {
        // checks that the string is numbers only (and dot)
        if (lbl->getText().containsOnly ("-.0123456789") == false)
        {
            if (lbl == rangeLabelMin)       lbl->setText (String (gesture.midiLow.getValue(), 2), dontSendNotification);
            else if (lbl == rangeLabelMax)  lbl->setText (String (gesture.midiHigh.getValue(), 2), dontSendNotification);

            return;
        }
    
        // Gets the float value of the text 
        float val = lbl->getText().getFloatValue();
    
        if (val < 0.0f)      val = 0.0f;
        else if (val > 1.0f) val = 1.0f;
    
        // Sets slider and labels accordingly
        if (lbl == rangeLabelMin)
        {
            // Min > Max
            if ( val > gesture.midiHigh.getValue()) val = gesture.midiHigh.getValue();
        
            // Normal case
            gesture.setMidiLow (val);
            lbl->setText (String (gesture.midiLow.getValue(), 2), dontSendNotification);
        }
        else if (lbl == rangeLabelMax)
        {
            // Max < Min
            if ( val < gesture.midiLow.getValue()) val = gesture.midiLow.getValue();
        
            // Normal case
            gesture.setMidiHigh  (val);
            lbl->setText (String (gesture.midiHigh.getValue(), 2), dontSendNotification);
        }
    }*/
}

void MidiModeComponent::comboBoxChanged (ComboBox* box)
{
    if (box == midiTypeBox)
    {
        bool isCC = (midiTypeBox->getSelectedId() == Gesture::controlChange);
        
        // cc Label is visible & editable only if "CC" is selected
        ccLabel->setEditable (isCC, false, false);
        ccLabel->setVisible (isCC);
        
        // Affects the gesture's midiType variable
        gesture.midiType = midiTypeBox->getSelectedId();
        gestureArray.checkPitchMerging();

        if (auto* parentComp = getParentComponent())
        	parentComp->repaint();
    }
}

void MidiModeComponent::updateComponents()
{
    ccLabel->setText (String (gesture.getCc()), dontSendNotification);
    midiRangeTuner->updateHighlightColour();
}

void MidiModeComponent::updateDisplay()
{
    midiRangeTuner->updateDisplay();
}

//==============================================================================
void MidiModeComponent::createComboBox()
{
    addAndMakeVisible (midiTypeBox = new ComboBox ("midiTypeBox"));
    midiTypeBox->addItem ("CC", Gesture::controlChange);
    midiTypeBox->addItem ("Pitch", Gesture::pitch);
    //midiTypeBox->addItem ("AfterTouch", Gesture::afterTouch);
    midiTypeBox->setSelectedId (gesture.midiType, dontSendNotification);
    
    // ComboBox look
    midiTypeBox->setJustificationType (Justification::centred);
    midiTypeBox->setColour (ComboBox::outlineColourId, getPlumeColour (detailPanelSubText));
    midiTypeBox->setColour (ComboBox::textColourId, getPlumeColour (detailPanelMainText));
    midiTypeBox->setColour (ComboBox::arrowColourId, getPlumeColour (detailPanelSubText));

    if (gesture.type == Gesture::vibrato || gesture.type == Gesture::pitchBend)
    {
    	midiTypeBox->setAlpha (0.3f);
    	midiTypeBox->setInterceptsMouseClicks (false, false);
    }
    
    midiTypeBox->addListener (this);
}

void MidiModeComponent::createLabels()
{
    //=== Midi Type label ===
    
    // CC label
    addAndMakeVisible (ccLabel = new Label ("CC Label", TRANS (String(gesture.getCc()))));
    ccLabel->setEditable ((midiTypeBox->getSelectedId() == Gesture::controlChange), false, false);
    ccLabel->setFont (PLUME::font::plumeFont.withHeight (13.0f));
    ccLabel->setJustificationType (Justification::centred);

    ccLabel->setColour (Label::backgroundColourId , Colour (0));
    ccLabel->setColour (Label::textColourId, getPlumeColour (detailPanelMainText));
    ccLabel->setColour (Label::outlineColourId, getPlumeColour (detailPanelSubText));
    
    // cc Label is visible & editable only if "CC" is selected
    ccLabel->setEditable (midiTypeBox->getSelectedId() == Gesture::controlChange, false, false);
    ccLabel->setVisible (midiTypeBox->getSelectedId() == Gesture::controlChange);
    
    ccLabel->addListener (this);
    
    //=== range Control labels ===
/*
    addAndMakeVisible (rangeLabelMin = new Label ("Min Label", TRANS (String (gesture.midiLow.getValue(), 2))));
    addAndMakeVisible (rangeLabelMax = new Label ("Max Label", TRANS (String (gesture.midiHigh.getValue(), 2))));
    
    // LabelMin style
    rangeLabelMin->setEditable (true, false, false);
    rangeLabelMin->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    rangeLabelMin->setJustificationType (Justification::centred);
    rangeLabelMin->setColour (Label::backgroundColourId , Colour (0));
    rangeLabelMin->setColour (Label::textColourId, getPlumeColour (detailPanelMainText));
    rangeLabelMin->setColour (Label::outlineColourId, getPlumeColour (detailPanelSubText));
    
    // LabelMax style
    rangeLabelMax->setEditable (true, false, false);
    rangeLabelMax->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    rangeLabelMax->setJustificationType (Justification::centred);
    rangeLabelMax->setColour (Label::backgroundColourId , Colour (0));
    rangeLabelMax->setColour (Label::textColourId, getPlumeColour (detailPanelMainText));
    rangeLabelMax->setColour (Label::outlineColourId, getPlumeColour (detailPanelSubText));
    
    
    rangeLabelMin->addListener (this);
    rangeLabelMax->addListener (this);*/
}

//==============================================================================
// Midi Banner

MidiBanner::MidiBanner (Gesture& gest)	: gesture (gest)
{
}
MidiBanner::~MidiBanner()
{
}
void MidiBanner::paint (Graphics& g)
{
    String midiString = (gesture.midiType == Gesture::controlChange ?
    				    	"CC " + String (gesture.getCc()) :
    						(gesture.midiType == Gesture::pitch ? "Pitch" : "Unknown" ));

    g.setColour (getPlumeColour (detailPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight(15));
	g.drawText ("MIDI", getLocalBounds().withWidth (getWidth()/4),
                            Justification::centred);

    g.setColour (getPlumeColour (detailPanelMainText));
    g.setFont (PLUME::font::plumeFontBold.withHeight(13));
    g.drawText (midiString, getLocalBounds().withSizeKeepingCentre (getWidth()/2, getHeight()),
                            Justification::centred);
}
void MidiBanner::resized()
{
}


//==============================================================================
// Midi Range Tuner

MidiRangeTuner::MidiRangeTuner (Gesture& gest) : gesture (gest)
{
    highlightColour = gesture.getHighlightColour();
    createLabels();
    createSliders();
}

MidiRangeTuner::~MidiRangeTuner()
{
    rangeLabelMin = nullptr;
    rangeLabelMax = nullptr;
    lowSlider = nullptr;
    highSlider = nullptr;
}

void MidiRangeTuner::paint (Graphics& g)
{
    drawSliderBackground (g);

    if (gesture.isActive()) drawCursor (g);
}

void MidiRangeTuner::resized()
{
    lowSlider->setBounds (getLocalBounds());
    highSlider->setBounds (getLocalBounds());

    lowSlider->setMouseDragSensitivity (getWidth());
    highSlider->setMouseDragSensitivity (getWidth());
}

void MidiRangeTuner::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only (and dot)
    if (lbl->getText().containsOnly ("-.0123456789") == false)
    {
        if (lbl == rangeLabelMin)       lbl->setText (String (gesture.midiLow.getValue(), 2), dontSendNotification);
        else if (lbl == rangeLabelMax)  lbl->setText (String (gesture.midiHigh.getValue(), 2), dontSendNotification);

        return;
    }
    
    // Gets the float value of the text 
    float val = lbl->getText().getFloatValue();
    
    if (val < 0.0f)      val = 0.0f;
    else if (val > 1.0f) val = 1.0f;
    
    // Sets slider and labels accordingly
    if (lbl == rangeLabelMin)
    {
        // Min > Max
        if ( val > gesture.midiHigh.getValue()) val = gesture.midiHigh.getValue();
        
        // Normal case
        {
            gesture.setMidiLow (val);
            lbl->setText (String (gesture.midiLow.getValue(), 2), dontSendNotification);
            lowSlider->setValue (val, dontSendNotification);
            setLabelBounds (*rangeLabelMin);
            rangeLabelMin->setVisible (false);
        }
    }
    else if (lbl == rangeLabelMax)
    {
        // Max < Min
        if ( val < gesture.midiLow.getValue()) val = gesture.midiLow.getValue();
        
        // Normal case
        {
            gesture.setMidiHigh (val);
            lbl->setText (String (gesture.midiHigh.getValue(), 2), dontSendNotification);
            highSlider->setValue (val, dontSendNotification);
            setLabelBounds (*rangeLabelMax);
            rangeLabelMax->setVisible (false);
        }
    }
}

void MidiRangeTuner::editorHidden (Label* lbl, TextEditor&)
{
    lbl->setVisible (false);
}

void MidiRangeTuner::sliderValueChanged (Slider* sldr)
{
    if (sldr == lowSlider)
    {
        gesture.setMidiLow (float (lowSlider->getValue()), false);
        rangeLabelMin->setText (String (lowSlider->getValue(), 2), dontSendNotification);
        setLabelBounds (*rangeLabelMin);

        // in case the other thumb is dragged along..
        if (gesture.midiHigh.getValue() < gesture.midiLow.getValue())
        {
            gesture.setMidiHigh (gesture.midiLow.getValue(), false);
            highSlider->setValue (double (gesture.midiHigh.getValue()), dontSendNotification);
            setLabelBounds (*rangeLabelMax);
            rangeLabelMax->setText (String (highSlider->getValue(), 2), dontSendNotification);
        }
    }

    else if (sldr == highSlider)
    {
        gesture.setMidiHigh (float (highSlider->getValue()), false);
        rangeLabelMax->setText (String (highSlider->getValue(), 2), dontSendNotification);
        setLabelBounds (*rangeLabelMax);

        // in case the other thumb is dragged along..
        if (gesture.midiHigh.getValue() < gesture.midiLow.getValue())
        {
            gesture.setMidiLow (gesture.midiHigh.getValue(), false);
            lowSlider->setValue (double (gesture.midiLow.getValue()), dontSendNotification);
            setLabelBounds (*rangeLabelMin);
            rangeLabelMin->setText (String (lowSlider->getValue(), 2), dontSendNotification);
        }
    }
}

void MidiRangeTuner::mouseDown (const MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        objectBeingDragged = getObjectToDrag (e);

        if (e.getNumberOfClicks() == 1)
        {
            if (objectBeingDragged == lowThumb)
            {
                rangeLabelMin->setVisible (true);
                lowSlider->mouseDown (e.getEventRelativeTo (lowSlider));
            }

            else if (objectBeingDragged == highThumb)
            {
                rangeLabelMax->setVisible (true);
                highSlider->mouseDown (e.getEventRelativeTo (highSlider));
            }

            else if (objectBeingDragged == middleArea)
            {
                rangeLabelMin->setVisible (true);
                rangeLabelMax->setVisible (true);
                
                lowSlider->setSliderSnapsToMousePosition (false);
                highSlider->setSliderSnapsToMousePosition (false);
                
                lowSlider->mouseDown (e.getEventRelativeTo (lowSlider));
                highSlider->mouseDown (e.getEventRelativeTo (highSlider));
            }

            repaint();
        }
        else // double click
        {
            if (objectBeingDragged == lowThumb)
            {
                objectBeingDragged = none;
                rangeLabelMin->setVisible (true);
                rangeLabelMin->showEditor();
                repaint();
            }

            else if (objectBeingDragged == highThumb)
            {
                objectBeingDragged = none;
                rangeLabelMax->setVisible (true);
                rangeLabelMax->showEditor();
                repaint();
            }
        }
    }
}

void MidiRangeTuner::mouseDrag (const MouseEvent& e)
{
    if (!e.mods.isLeftButtonDown() || e.getNumberOfClicks() > 1)
    {
        objectBeingDragged = none;
        return;
    }

    if (objectBeingDragged == lowThumb)
    {
        lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider));
    }

    else if (objectBeingDragged == highThumb)
    {
        highSlider->mouseDrag (e.getEventRelativeTo (highSlider));
    }

    else if (objectBeingDragged == middleArea)
    {
        lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider));
        highSlider->mouseDrag (e.getEventRelativeTo (highSlider));
    }
}

void MidiRangeTuner::mouseUp (const MouseEvent& e)
{
    if (e.mods.isLeftButtonDown() && e.getNumberOfClicks() == 1)
    {
        if (objectBeingDragged != none)
        {
            if (objectBeingDragged == lowThumb)
            {
                lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider));
                rangeLabelMin->setVisible (false);
            }

            else if (objectBeingDragged == highThumb)
            {
                highSlider->mouseDrag (e.getEventRelativeTo (highSlider));
                rangeLabelMax->setVisible (false);
            }

            else if (objectBeingDragged == middleArea)
            {
                lowSlider->mouseDrag (e.getEventRelativeTo (lowSlider));
                highSlider->mouseDrag (e.getEventRelativeTo (highSlider));
                lowSlider->setSliderSnapsToMousePosition (true);
                highSlider->setSliderSnapsToMousePosition (true);

                rangeLabelMin->setVisible (false);
                rangeLabelMax->setVisible (false);
            }

            objectBeingDragged = none;
            repaint();
        }
    }
}

void MidiRangeTuner::updateDisplay()
{
    if (gesture.getRescaledMidiValue() != lastValue)
    {
        lastValue = gesture.getRescaledMidiValue();
        
        repaint (lowSlider->getBounds().withY (lowSlider->getBounds().getCentreY() - 13)
                                       .withHeight (8));
    }
}

void MidiRangeTuner::updateHighlightColour()
{
    highlightColour = gesture.getHighlightColour();

    auto updateLabelColours = [this] (Label& label)
    {
        label.setColour (Label::textColourId, highlightColour);
        label.setColour (Label::textWhenEditingColourId, highlightColour);
        label.setColour (TextEditor::textColourId, highlightColour);
        label.setColour (TextEditor::highlightColourId, highlightColour.withAlpha (0.2f));
        label.setColour (TextEditor::highlightedTextColourId, highlightColour);
        label.setColour (CaretComponent::caretColourId, highlightColour.withAlpha (0.2f));
    };

    updateLabelColours (*rangeLabelMin);
    updateLabelColours (*rangeLabelMax);

    repaint();
}

void MidiRangeTuner::createLabels()
{
    addAndMakeVisible (rangeLabelMin = new Label ("Min Label", String (gesture.midiLow.getValue(), 2)));
    addAndMakeVisible (rangeLabelMax = new Label ("Max Label", String (gesture.midiHigh.getValue(), 2)));
    
    // Label style

    auto setLabelAttributes = [this] (Label& label)
    {
        label.setEditable (true, false, false);
        label.setSize (30, 30);
        label.setFont (PLUME::font::plumeFont.withHeight (11.0f));
        label.setColour (Label::textColourId, highlightColour);
        label.setColour (Label::backgroundColourId, Colour (0x00000000));
        label.setColour (Label::textWhenEditingColourId, highlightColour);
        label.setColour (TextEditor::textColourId, highlightColour);
        label.setColour (TextEditor::highlightColourId, highlightColour.withAlpha (0.2f));
        label.setColour (TextEditor::highlightedTextColourId, highlightColour);
        label.setColour (CaretComponent::caretColourId, highlightColour.withAlpha (0.2f));
        label.setJustificationType (Justification::centred);
        label.setVisible (false);
        label.addListener (this);
    };

    setLabelAttributes (*rangeLabelMin);
    setLabelAttributes (*rangeLabelMax);
}

void MidiRangeTuner::setLabelBounds (Label& labelToResize)
{
    if (&labelToResize == rangeLabelMin)
    {
        rangeLabelMin->setCentrePosition (jmin (jmax ((int) getThumbX (lowThumb), rangeLabelMin->getWidth()/2),
                                                getWidth() - rangeLabelMin->getWidth()/2),
                                          lowSlider->getBounds().getCentreY() + 16);
    }
    else if (&labelToResize == rangeLabelMax)
    {
        rangeLabelMax->setCentrePosition (jmin (jmax ((int) getThumbX (highThumb), rangeLabelMax->getWidth()/2),
                                                getWidth() - rangeLabelMax->getWidth()/2),
                                          highSlider->getBounds().getCentreY() + 16);
    }
}

void MidiRangeTuner::createSliders()
{
    addAndMakeVisible (lowSlider = new Slider ("Range Low Slider"));
    addAndMakeVisible (highSlider = new Slider ("Range High Slider"));
    
    auto setSliderSettings = [this] (Slider& slider, float valueToSet)
    {
        slider.setSliderStyle (Slider::LinearHorizontal);
        slider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        slider.setColour (Slider::backgroundColourId, Colour (0x00000000));
        slider.setColour (Slider::trackColourId, Colour (0x00000000));
        slider.setRange (0.0, 1.0, 0.01);
        slider.setValue (valueToSet);
        slider.setLookAndFeel (&sliderLookAndFeel);
        slider.setInterceptsMouseClicks (false, false);
        slider.addListener (this);
    };

    setSliderSettings (*lowSlider, (double) gesture.midiLow.getValue());
    setSliderSettings (*highSlider, (double) gesture.midiHigh.getValue());
}

float MidiRangeTuner::getThumbX (DraggableObject thumb)
{
    if (thumb == lowThumb)
    {
        return lowSlider->getX() + 11.5f + (lowSlider->getWidth() - 23.0f) * lowSlider->getValue();
    }

    if (thumb == highThumb)
    {
        return highSlider->getX() + 11.5f + (highSlider->getWidth() - 23.0f) * highSlider->getValue();
    }

    return -1.0f;
}


void MidiRangeTuner::handleSliderClick (const MouseEvent& e)
{

}

MidiRangeTuner::DraggableObject MidiRangeTuner::getObjectToDrag (const MouseEvent& e)
{
    if (e.x < 0 || e.x > highSlider->getWidth())
        return none;

    if (e.mods.isShiftDown())
        return middleArea;

    const float tolerance = (getThumbX (highThumb) - getThumbX (lowThumb))/5.0f;

    if ((float) e.x <= getThumbX (lowThumb) + tolerance)
        return lowThumb;
    
    if ((float) e.x < getThumbX (highThumb) - tolerance)
        return middleArea;

    return highThumb;
}

void MidiRangeTuner::drawCursor (Graphics& g)
{
    if (lastValue == -1) return;

    float maxMidiFloat = (gesture.midiType == Gesture::pitch) ? 16383.0f : 127.0f;

    Path cursorPath;
    float cursorX = 11.5f + (lowSlider->getWidth() - 23.0f) * (lastValue / maxMidiFloat);

    Point<float> cursorPoint = {cursorX, lowSlider->getBounds().getCentreY() - 9.0f};

    cursorPath.addTriangle ({cursorPoint.x - 3.0f, cursorPoint.y - 3.0f},
                            {cursorPoint.x + 3.0f, cursorPoint.y - 3.0f},
                            {cursorPoint.x       , cursorPoint.y + 3.0f});

    g.setColour (highlightColour);
    g.fillPath (cursorPath);
}

void MidiRangeTuner::drawSliderBackground (Graphics& g)
{
    g.setColour (getPlumeColour (midiMapSliderBackground));
    g.fillRoundedRectangle (lowSlider->getBounds().withSizeKeepingCentre (lowSlider->getWidth() - 10, 12)
                                                  .toFloat(),
                            3.0f);

    Point<float> startPoint (getThumbX (lowThumb), lowSlider->getY() + lowSlider->getHeight() * 0.5f);

    Point<float> endPoint (getThumbX (highThumb), highSlider->getY() + highSlider->getHeight() * 0.5f);

    Path valueTrack;
    valueTrack.startNewSubPath (startPoint);
    valueTrack.lineTo (endPoint);

    g.setColour (objectBeingDragged == middleArea ? highlightColour.interpolatedWith (Colour (0xffffffff),
                                                                                      0.5f)
                                                  : highlightColour);
    g.strokePath(valueTrack, { 6.0f //Old: trackWidth
                               , PathStrokeType::curved, PathStrokeType::rounded });

    if (objectBeingDragged == lowThumb || objectBeingDragged == highThumb)
    {
        g.setColour (highlightColour.withAlpha (0.6f));
        g.fillEllipse(juce::Rectangle<float> (25.0f, 25.0f)
                          .withCentre (objectBeingDragged == lowThumb ? startPoint
                                                                      : endPoint));
    }
}
