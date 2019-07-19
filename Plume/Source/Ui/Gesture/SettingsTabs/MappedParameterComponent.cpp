/*
  ==============================================================================

    MappedParameterComponent.cpp
    Created: 9 Jul 2019 2:59:55pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "MappedParameterComponent.h"

//==============================================================================
MappedParameterComponent::MappedParameterComponent (Gesture& gest,  GestureArray& gestArr, PluginWrapper& wrap,
                                                    Gesture::MappedParameter& mappedParam, int id)
    : gesture (gest), gestureArray (gestArr), mappedParameter (mappedParam), wrapper (wrap),
      paramId (id), highlightColour (gest.getHighlightColour())
{
    TRACE_IN;
    // Creates the close button
    Image close = ImageFileFormat::loadFrom (PlumeData::Close_png, PlumeData::Close_pngSize);

    addAndMakeVisible (closeButton = new ImageButton ("Close Button"));
    closeButton->setImages (false, true, true,
							close, 1.0f, Colour (0x00000000),
							close, 0.5f, Colour (0x00000000),
							close, 0.7f, Colour (0x501600f0));
    closeButton->addListener (this);
    
    // Creates the reverse button
    Image reverse = ImageFileFormat::loadFrom (PlumeData::reverse_png, PlumeData::reverse_pngSize);
    
    addAndMakeVisible (reverseButton = new TextButton ("Reverse Button"));
    reverseButton->setButtonText ("R");
    reverseButton->setColour (TextButton::buttonColourId, getPlumeColour (midiMapSliderBackground));
    reverseButton->setColour (TextButton::buttonOnColourId, highlightColour);
    reverseButton->setColour (TextButton::textColourOnId , getPlumeColour (detailPanelMainText));
    reverseButton->setColour (TextButton::textColourOffId , getPlumeColour (detailPanelMainText));
	reverseButton->setToggleState (mappedParameter.reversed, dontSendNotification);
    reverseButton->setClickingTogglesState (true);
	reverseButton->setState (Button::buttonNormal);
    reverseButton->addListener (this);
    
    createLabels();
    createSliders();
}

MappedParameterComponent::~MappedParameterComponent()
{
    TRACE_IN;
    lowSlider->setLookAndFeel (nullptr);
    lowSlider = nullptr;

    highSlider->setLookAndFeel (nullptr);
    highSlider = nullptr;

    closeButton = nullptr;
    reverseButton = nullptr;
    paramNameLabel = nullptr;
    rangeLabelMin = nullptr;
    rangeLabelMax = nullptr;
}

//==============================================================================
void MappedParameterComponent::paint (Graphics& g)
{ 
    if (!allowDisplayUpdate) return;

	if (gesture.isActive()) drawCursor (g);
    
    drawSliderBackground (g);
}

void MappedParameterComponent::resized()
{
    auto area = getLocalBounds();
    auto topArea = area.removeFromTop (area.getHeight()/6);

    //closeButton->setBounds (topArea.removeFromRight (getWidth()/6));

    paramNameLabel->setBounds (topArea);

    reverseButton->setBounds (area.removeFromBottom (jmax (20, getHeight()/6))
                                  .withSizeKeepingCentre (18, 18));
    lowSlider->setBounds (area);
    lowSlider->setMouseDragSensitivity (area.getHeight());

    highSlider->setBounds (area);
    highSlider->setMouseDragSensitivity (area.getHeight());

    setLabelBounds (*rangeLabelMin);
    setLabelBounds (*rangeLabelMax);
    
    repaint();
}


//==============================================================================
void MappedParameterComponent::buttonClicked (Button* bttn)
{
    if (bttn == closeButton)
    {
        gesture.deleteParameter (paramId);
        allowDisplayUpdate = false;
    }
    else if (bttn == reverseButton)
    {
        mappedParameter.reversed = !(mappedParameter.reversed);
    }
}

void MappedParameterComponent::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only (and dot)
    if (lbl->getText().containsOnly ("-.0123456789") == false)
    {
        if (lbl == rangeLabelMin)       lbl->setText (String (mappedParameter.range.getStart(), 1), dontSendNotification);
        else if (lbl == rangeLabelMax)  lbl->setText (String (mappedParameter.range.getEnd(), 1), dontSendNotification);

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
        if ( val > mappedParameter.range.getEnd()) val = mappedParameter.range.getEnd();
        
        // Normal case
        {
            mappedParameter.range.setStart (val);
            lbl->setText (String (mappedParameter.range.getStart(), 1), dontSendNotification);
            lowSlider->setValue (val, dontSendNotification);
            setLabelBounds (*rangeLabelMin);
            rangeLabelMin->setVisible (false);
        }
    }
    else if (lbl == rangeLabelMax)
    {
        // Max < Min
        if ( val < mappedParameter.range.getStart()) val = mappedParameter.range.getStart();
        
        // Normal case
        {
			mappedParameter.range.setEnd (val);
            lbl->setText (String (mappedParameter.range.getEnd(), 1), dontSendNotification);
            highSlider->setValue (val, dontSendNotification);
            setLabelBounds (*rangeLabelMax);
            rangeLabelMax->setVisible (false);
        }
    }
}


void MappedParameterComponent::editorHidden (Label* lbl, TextEditor&)
{
    lbl->setVisible (false);
}

void MappedParameterComponent::sliderValueChanged (Slider* sldr)
{
    if (sldr == lowSlider)
    {
        mappedParameter.range.setStart (float (lowSlider->getValue()));
        rangeLabelMin->setText (String (lowSlider->getValue(), 1), dontSendNotification);
        setLabelBounds (*rangeLabelMin);

        // in case the other thumb is dragged along..
        if (float (highSlider->getValue()) != mappedParameter.range.getEnd())
        {
            highSlider->setValue (double (mappedParameter.range.getEnd()), dontSendNotification);
            setLabelBounds (*rangeLabelMax);
            rangeLabelMax->setText (String (highSlider->getValue(), 1), dontSendNotification);
        }
    }

    else if (sldr == highSlider)
    {
        mappedParameter.range.setEnd (float (highSlider->getValue()));
		rangeLabelMax->setText (String (highSlider->getValue(), 1), dontSendNotification);
        setLabelBounds (*rangeLabelMax);

        // in case the other thumb is dragged along..
        if (float (lowSlider->getValue()) != mappedParameter.range.getStart())
        {
            lowSlider->setValue (double (mappedParameter.range.getStart()), dontSendNotification);
            setLabelBounds (*rangeLabelMin);
            rangeLabelMin->setText (String (lowSlider->getValue(), 1), dontSendNotification);
        }
    }
}
void MappedParameterComponent::mouseEnter (const MouseEvent& e)
{
    if (e.eventComponent == paramNameLabel)
    {
        paramNameLabel->setColour (Label::textColourId, getPlumeColour (detailPanelMainText).withAlpha (0.5f));
    }
}

void MappedParameterComponent::mouseExit (const MouseEvent& e)
{
    if (e.eventComponent == paramNameLabel)
    {
        paramNameLabel->setColour (Label::textColourId, getPlumeColour (detailPanelMainText));
    }
}

void MappedParameterComponent::mouseDown (const MouseEvent& e)
{
    if (e.mods.isPopupMenu())
    {
        PopupMenu rightClickMenu;

        rightClickMenu.addItem (1, "Delete");

        rightClickMenu.showMenuAsync (PopupMenu::Options().withParentComponent (getParentComponent())
                                                          .withMinimumWidth (getWidth() - 4)
                                                          .withMaximumNumColumns (1)
                                                          .withStandardItemHeight (16)
                                                          .withPreferredPopupDirection
                                                              (PopupMenu::Options
                                                                        ::PopupDirection
                                                                        ::downwards),
                                                          //.withTargetComponent (this),
                                         ModalCallbackFunction::forComponent (rightClickMenuCallback, this));
    }
    
    else if (e.eventComponent == paramNameLabel)
    {
        handleLabelClick (e);
    }
    
    else if (e.x >= getWidth()/4 && e.x <= getWidth()*3/4)
    {
        handleSliderClick (e);
    }
}

void MappedParameterComponent::handleLabelClick (const MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        PopupMenu parameterListMenu;

        parameterListMenu.addItem (1, "None");
        parameterListMenu.addSeparator();

		for (auto* param : wrapper.getWrapperProcessor().getParameters())
		{
            parameterListMenu.addItem (param->getParameterIndex() + 2,
                                       param->getName (20),
                                       !gestureArray.parameterIsMapped (param->getParameterIndex()));
		}

        parameterListMenu.showMenuAsync (PopupMenu::Options().withParentComponent (getParentComponent())
                                                             .withMinimumWidth (getWidth() - 4)
                                                             .withMaximumNumColumns (1)
                                                             .withStandardItemHeight (16)
                                                             .withPreferredPopupDirection
                                                                 (PopupMenu::Options
                                                                           ::PopupDirection
                                                                           ::downwards)
                                                             .withTargetScreenArea (juce::Rectangle<int> (getScreenX(),
                                                                                                          getScreenY()+10,
                                                                                                          getWidth(), getHeight())),
                                                             //.withTargetComponent (this),
                                         ModalCallbackFunction::forComponent (parameterMenuCallback, this));
    }
}

void MappedParameterComponent::parameterMenuCallback (int result, MappedParameterComponent* mpc)
{
	mpc->handleMenuResult (result, true);
}

void MappedParameterComponent::rightClickMenuCallback (int result, MappedParameterComponent* mpc)
{
    mpc->handleMenuResult (result, false);
}

void MappedParameterComponent::handleMenuResult (const int result, const bool isParameterMenu)
{
    if (isParameterMenu)
    {
        if (result == 0)
        {
            paramNameLabel->setColour (Label::textColourId, getPlumeColour (detailPanelMainText));
        }

        else if (result == 1)
        {
            gesture.deleteParameter (paramId);
        }

        else if (result < wrapper.getWrapperProcessor().getParameters().size() + 2)
        {
            AudioProcessorParameter& newParam = *wrapper.getWrapperProcessor().getParameters()[result-2];

            if (!gestureArray.parameterIsMapped (newParam.getParameterIndex()))
            {
                allowDisplayUpdate = false;

                gesture.replaceParameter (paramId, newParam,
                                          mappedParameter.range, mappedParameter.reversed);
            }
        }
    }

    else if (result == 1)
    {
        gesture.deleteParameter (paramId);
    }
}

void MappedParameterComponent::handleSliderClick (const MouseEvent& e)
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

void MappedParameterComponent::mouseDrag (const MouseEvent& e)
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

void MappedParameterComponent::mouseUp (const MouseEvent& e)
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

Gesture::MappedParameter& MappedParameterComponent::getMappedParameter()
{
    return mappedParameter;
}

void MappedParameterComponent::updateDisplay()
{
    if (allowDisplayUpdate)
    {
        if (gesture.getValueForMappedParameter (mappedParameter.range, mappedParameter.reversed))
        {
            repaint (lowSlider->getBounds().withX (lowSlider->getBounds().getCentreX() - 13)
                                           .withWidth (8));
        }
    }
}

void MappedParameterComponent::updateHighlightColour()
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

//==============================================================================
void MappedParameterComponent::createLabels()
{
    //=== Value label ===
    addAndMakeVisible (paramNameLabel = new Label ("Value Label", mappedParameter.parameter.getName (20)));
    paramNameLabel->setEditable (false, false, false);
    paramNameLabel->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    paramNameLabel->setColour (Label::textColourId, getPlumeColour (detailPanelMainText));
    paramNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    paramNameLabel->setJustificationType (Justification::centredLeft);
    paramNameLabel->addMouseListener (this, false);
    
    //=== range Control labels ===
    addAndMakeVisible (rangeLabelMin = new Label ("Min Label", String (mappedParameter.range.getStart(), 1)));
    addAndMakeVisible (rangeLabelMax = new Label ("Max Label", String (mappedParameter.range.getEnd(), 1)));
    
    // LabelMin style
    rangeLabelMin->setEditable (true, false, false);
    rangeLabelMin->setSize (30, 30);
    rangeLabelMin->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    rangeLabelMin->setColour (Label::textColourId, highlightColour);
    rangeLabelMin->setColour (Label::backgroundColourId, Colour (0x00000000));
    rangeLabelMin->setColour (Label::textWhenEditingColourId, highlightColour);
    rangeLabelMin->setColour (TextEditor::textColourId, highlightColour);
    rangeLabelMin->setColour (TextEditor::highlightColourId, highlightColour.withAlpha (0.2f));
    rangeLabelMin->setColour (TextEditor::highlightedTextColourId, highlightColour);
    rangeLabelMin->setColour (CaretComponent::caretColourId, highlightColour.withAlpha (0.2f));
    rangeLabelMin->setJustificationType (Justification::centred);
    rangeLabelMin->setVisible (false);
    
    // LabelMax style
    rangeLabelMax->setEditable (true, false, false);
    rangeLabelMax->setSize (30, 30);
    rangeLabelMax->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    rangeLabelMax->setColour (Label::textColourId, highlightColour);
    rangeLabelMax->setColour (Label::backgroundColourId, Colour (0x00000000));
    rangeLabelMax->setColour (Label::textWhenEditingColourId, highlightColour);
    rangeLabelMax->setColour (TextEditor::textColourId, highlightColour);
    rangeLabelMax->setColour (TextEditor::highlightColourId, highlightColour.withAlpha (0.2f));
    rangeLabelMax->setColour (TextEditor::highlightedTextColourId, highlightColour);
    rangeLabelMax->setColour (CaretComponent::caretColourId, highlightColour.withAlpha (0.2f));
    rangeLabelMax->setJustificationType (Justification::centred);
    rangeLabelMax->setVisible (false);
    
    // Labels settings
    rangeLabelMin->addListener (this);
    rangeLabelMax->addListener (this);
}

void MappedParameterComponent::setLabelBounds (Label& labelToResize)
{
    if (&labelToResize == rangeLabelMin)
    {
        rangeLabelMin->setCentrePosition (lowSlider->getBounds().getCentreX() + 16,
                                          (int) getThumbY (lowThumb));
    }
    else if (&labelToResize == rangeLabelMax)
    {
        rangeLabelMax->setCentrePosition (highSlider->getBounds().getCentreX() + 16,
                                          (int) getThumbY (highThumb));
    }
}

void MappedParameterComponent::createSliders()
{
    addAndMakeVisible (lowSlider = new Slider ("Range Low Slider"));
    addAndMakeVisible (highSlider = new Slider ("Range High Slider"));
    
    auto setSliderSettings = [this] (Slider& slider, float valueToSet)
    {
        slider.setSliderStyle (Slider::LinearVertical);
        slider.setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
        slider.setColour (Slider::backgroundColourId, Colour (0x00000000));
        slider.setColour (Slider::trackColourId, Colour (0x00000000));
        slider.setRange (0.0, 1.0, 0.01);
        slider.setValue (valueToSet);
        slider.setLookAndFeel (&sliderLookAndFeel);
        slider.setInterceptsMouseClicks (false, false);
        slider.addListener (this);
    };

    setSliderSettings (*lowSlider, (double) mappedParameter.range.getStart());
    setSliderSettings (*highSlider, (double) mappedParameter.range.getEnd());
}

//==============================================================================
float MappedParameterComponent::getThumbY (MappedParameterComponent::DraggableObject thumb)
{
    if (thumb == lowThumb)
    {
        return lowSlider->getY() + 11.5f + (lowSlider->getHeight() - 23.0f) *
                                          (1.0f - lowSlider->getValue());
    }

    if (thumb == highThumb)
    {
        return highSlider->getY() + 11.5f + (highSlider->getHeight() - 23.0f) *
                                          (1.0f - highSlider->getValue());
    }

    return -1;
}

MappedParameterComponent::DraggableObject MappedParameterComponent::getObjectToDrag (const MouseEvent& e)
{
    if (e.y < lowSlider->getY() || e.y > highSlider->getY() + highSlider->getHeight())
        return none;

    if (e.mods.isShiftDown())
        return middleArea;

    const float tolerance = (getThumbY (lowThumb) - getThumbY (highThumb))/5.0f;

    if ((float) e.y <= getThumbY (highThumb) + tolerance)
        return highThumb;
    
    if ((float) e.y < getThumbY (lowThumb) - tolerance)
        return middleArea;

    return lowThumb;
    
}

//==============================================================================
void MappedParameterComponent::drawCursor (Graphics& g)
{
    Path cursorPath;
    float cursorY = lowSlider->getY() + 11.5f +
                        (lowSlider->getHeight() - 23.0f) *
                        (1.0f - gesture.getValueForMappedParameter (mappedParameter.range,
                                                                    mappedParameter.reversed));

    Point<float> cursorPoint = {lowSlider->getBounds().getCentreX() - 9.0f,
                                cursorY};

    cursorPath.addTriangle ({cursorPoint.x - 3.0f, cursorPoint.y - 3.0f},
                            {cursorPoint.x + 3.0f, cursorPoint.y       },
                            {cursorPoint.x - 3.0f, cursorPoint.y + 3.0f});

    g.setColour (highlightColour);
    g.fillPath (cursorPath);
}


void MappedParameterComponent::drawSliderBackground (Graphics& g)
{
    g.setColour (getPlumeColour (midiMapSliderBackground));
    g.fillRoundedRectangle (lowSlider->getBounds().withSizeKeepingCentre (12, lowSlider->getHeight() - 10)
                                                  .toFloat(),
                            3.0f);

    Point<float> startPoint (lowSlider->getX() + lowSlider->getWidth() * 0.5f,
                             getThumbY (lowThumb));

    Point<float> endPoint (lowSlider->getX() + lowSlider->getWidth() * 0.5f,
                           getThumbY (highThumb));

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