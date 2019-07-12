/*
  ==============================================================================

    MappedParameterComponent.cpp
    Created: 9 Jul 2019 2:59:55pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "MappedParameterComponent.h"

//==============================================================================
MappedParameterComponent::MappedParameterComponent (Gesture& gest, Gesture::MappedParameter& mappedParam,
                                                    PluginWrapper& wrap, int id)
    : gesture (gest), mappedParameter (mappedParam), wrapper (wrap),
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
    reverseButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    reverseButton->setColour (TextButton::buttonOnColourId, highlightColour);
    reverseButton->setColour (TextButton::textColourOnId , Colour (0xffffffff));
    reverseButton->setColour (TextButton::textColourOffId , Colour (0xffffffff));
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

	drawCursor (g);
    drawSliderBackground (g);
}

void MappedParameterComponent::resized()
{
    auto area = getLocalBounds();
    auto topArea = area.removeFromTop (area.getHeight()/6);

    closeButton->setBounds (topArea.removeFromRight (getWidth()/6));

    paramNameLabel->setBounds (topArea);

    reverseButton->setBounds (area.removeFromBottom (area.getHeight()/6)
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
            mappedParameter.range.setStart(val);
            lbl->setText (String (mappedParameter.range.getStart(), 1), dontSendNotification);
            lowSlider->setValue (val, dontSendNotification);
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
            lowSlider->setValue (val, dontSendNotification);
        }
    }
}

void MappedParameterComponent::sliderValueChanged (Slider* sldr)
{
    if (sldr == lowSlider)
    {
        mappedParameter.range.setStart ((float) lowSlider->getValue());
        rangeLabelMin->setText (String (lowSlider->getValue()), dontSendNotification);
        setLabelBounds (*rangeLabelMin);

        // in case the other thumb is dragged along..
        if (mappedParameter.range.getStart() > mappedParameter.range.getEnd())
        {
            mappedParameter.range.setEnd (float (lowSlider->getValue()));
            highSlider->setValue (double (mappedParameter.range.getEnd()), dontSendNotification);
            setLabelBounds (*rangeLabelMax);
            rangeLabelMax->setText (String (highSlider->getValue()), dontSendNotification);
        }
    }

    else if (sldr == highSlider)
    {
        mappedParameter.range.setEnd ((float) highSlider->getValue());
		rangeLabelMax->setText (String (highSlider->getValue()), dontSendNotification);
        setLabelBounds (*rangeLabelMax);

        // in case the other thumb is dragged along..
        if (mappedParameter.range.getStart() > mappedParameter.range.getEnd())
        {
            mappedParameter.range.setStart (float (highSlider->getValue()));
            lowSlider->setValue (double (mappedParameter.range.getStart()), dontSendNotification);
            setLabelBounds (*rangeLabelMin);
            rangeLabelMin->setText (String (lowSlider->getValue()), dontSendNotification);
        }
    }
}

void MappedParameterComponent::mouseDown (const MouseEvent& e)
{
	DBG("Click");
    if (e.eventComponent == paramNameLabel)
    {
        DBG ("Label Click");
        handleLabelClick (e);
    }
    else
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

        DBG ("1st Index : " << wrapper.getWrapperProcessor().getParameters()[0]->getParameterIndex());
        DBG ("last Index : " << wrapper.getWrapperProcessor().getParameters()[wrapper.getWrapperProcessor().getParameters()
                                                                                                           .size()-1]
                                    ->getParameterIndex());

		for (auto* param : wrapper.getWrapperProcessor().getParameters())
		{
            parameterListMenu.addItem (param->getParameterIndex() + 2, param->getName (20));
		}

        parameterListMenu.showMenuAsync (PopupMenu::Options().withParentComponent (getParentComponent())
                                                             .withMinimumWidth (getWidth() - 4)
                                                             .withMaximumNumColumns (1)
                                                             .withPreferredPopupDirection
                                                                 (PopupMenu::Options
                                                                           ::PopupDirection
                                                                           ::downwards)
                                                             //.withTargetScreenArea (getBounds()),
                                                             .withTargetComponent (this),
                                         ModalCallbackFunction::forComponent (parameterMenuCallback, this));
    }
}

void MappedParameterComponent::parameterMenuCallback (int result, MappedParameterComponent* mpc)
{
    DBG ("Result : " << result);
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
                rangeLabelMin->setVisible (true);
                rangeLabelMin->showEditor();
            }

            else if (objectBeingDragged == highThumb)
            {
                rangeLabelMax->setVisible (true);
                rangeLabelMax->showEditor();
            }
        }
    }
}

void MappedParameterComponent::mouseDrag (const MouseEvent& e)
{
    if (!e.mods.isLeftButtonDown() || e.getNumberOfClicks() > 1) return;

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

//==============================================================================
void MappedParameterComponent::createLabels()
{
    //=== Value label ===
    addAndMakeVisible (paramNameLabel = new Label ("Value Label", mappedParameter.parameter.getName (20)));
    paramNameLabel->setEditable (false, false, false);
    paramNameLabel->setFont (Font (PLUME::UI::font, 11.0f, Font::plain));
    paramNameLabel->setColour (Label::textColourId, Colour (0xff000000));
    paramNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    paramNameLabel->setJustificationType (Justification::centredLeft);
    paramNameLabel->addMouseListener (this, false);
    
    //=== range Control labels ===
    addAndMakeVisible (rangeLabelMin = new Label ("Min Label", String (mappedParameter.range.getStart(), 1)));
    addAndMakeVisible (rangeLabelMax = new Label ("Max Label", String (mappedParameter.range.getEnd(), 1)));
    
    // LabelMin style
    rangeLabelMin->setEditable (true, false, false);
    rangeLabelMin->setSize (30, 20);
    rangeLabelMin->setFont (Font (PLUME::UI::font, 8.0f, Font::plain));
    rangeLabelMin->setColour (Label::textColourId, highlightColour);
    rangeLabelMin->setColour (Label::backgroundColourId, Colour (0x00000000));
    rangeLabelMin->setColour (TextEditor::textColourId, highlightColour);
    rangeLabelMin->setColour (TextEditor::highlightColourId, highlightColour.withAlpha (0.2f));
    rangeLabelMin->setColour (TextEditor::highlightedTextColourId, highlightColour);
    rangeLabelMin->setColour (CaretComponent::caretColourId, highlightColour.withAlpha (0.2f));
    rangeLabelMin->setJustificationType (Justification::centred);
    rangeLabelMin->setVisible (false);
    
    // LabelMax style
    rangeLabelMax->setEditable (true, false, false);
    rangeLabelMax->setSize (30, 20);
    rangeLabelMax->setFont (Font (PLUME::UI::font, 8.0f, Font::plain));
    rangeLabelMax->setColour (Label::textColourId, highlightColour);
    rangeLabelMax->setColour (Label::backgroundColourId, Colour (0x00000000));
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
        rangeLabelMin->setCentrePosition (lowSlider->getBounds().getCentreX() + 15,
                                          (int) getThumbY (lowThumb));
    }
    else if (&labelToResize == rangeLabelMax)
    {
        rangeLabelMax->setCentrePosition (highSlider->getBounds().getCentreX() + 15,
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
    g.setColour (Colour (0xff505050));
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