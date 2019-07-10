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
                                                    const int id, Colour paramCompColour)
    : gesture (gest), mappedParameter (mappedParam), paramId (id), highlightColour (paramCompColour)
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
    
    addAndMakeVisible (reverseButton = new ImageButton ("Reverse Button"));
    reverseButton->setImages (false, true, true,
							  reverse, 1.0f, Colour (0x00000000),
							  reverse, 0.5f, Colour (0xff323232),
							  reverse, 1.0f, Colour (0xff4d94d9));
	reverseButton->setToggleState (mappedParameter.reversed, dontSendNotification);
    reverseButton->setClickingTogglesState (true);
	reverseButton->setState (Button::buttonNormal);
    reverseButton->addListener (this);
    
    createLabels();
    createSlider();
}

MappedParameterComponent::~MappedParameterComponent()
{
    TRACE_IN;
    rangeSlider->setLookAndFeel (nullptr);
    rangeSlider = nullptr;

    closeButton = nullptr;
    reverseButton = nullptr;
    valueLabel = nullptr;
    rangeLabelMin = nullptr;
    rangeLabelMax = nullptr;
}

//==============================================================================
void MappedParameterComponent::paint (Graphics& g)
{ 
    if (!allowDisplayUpdate) return;
    
    g.setColour (Colours::black);
    g.setFont (PLUME::font::plumeFont.withHeight (11.0f));

    g.drawFittedText (mappedParameter.parameter.getName (20),
                      getLocalBounds().withHeight (closeButton->getHeight())
                                      .withSizeKeepingCentre (getWidth()*3/4, closeButton->getHeight()),
                      Justification::centred, 1);

    g.setColour (Colour (0xff505050));
    g.fillRoundedRectangle (rangeSlider->getBounds().withSizeKeepingCentre (12, rangeSlider->getHeight() - 10)
                                                    .toFloat(),
                            3.0f);
	drawCursor (g);
}

void MappedParameterComponent::resized()
{
    auto area = getLocalBounds();
    auto topArea = area.removeFromTop (area.getHeight()/6);

    closeButton->setBounds (topArea.removeFromRight (getWidth()/6));

    //valueLabel->setBounds (area.removeFromTop (getHeight()/4). reduced (getWidth()/8, 0));

    reverseButton->setBounds (area.removeFromBottom (area.getHeight()/6)
                                  .withSizeKeepingCentre (15, 15));
    rangeSlider->setBounds (area);

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
            rangeSlider->setMinValue (val, dontSendNotification);
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
            rangeSlider->setMaxValue (val, dontSendNotification);
        }
    }
}

void MappedParameterComponent::sliderValueChanged (Slider* sldr)
{
    if (sldr == rangeSlider)
    {
        if (rangeSlider->getThumbBeingDragged() == 1)
        {
            mappedParameter.range.setStart ((float) rangeSlider->getMinValue());
            rangeLabelMin->setText (String (rangeSlider->getMinValue()), dontSendNotification);
            setLabelBounds (*rangeLabelMin);
        }
        else if (rangeSlider->getThumbBeingDragged() == 2)
        {
            mappedParameter.range.setEnd ((float) rangeSlider->getMaxValue());
			rangeLabelMax->setText (String (rangeSlider->getMaxValue()), dontSendNotification);
            setLabelBounds (*rangeLabelMax);
        }
    }
}

void MappedParameterComponent::mouseDown (const MouseEvent& e)
{
    if (e.eventComponent == rangeSlider)
    {
        if (rangeSlider->getThumbBeingDragged() == 1)
        {
            rangeLabelMin->setVisible (true);
        }
        else if (rangeSlider->getThumbBeingDragged() == 2)
        {
            rangeLabelMax->setVisible (true);
        }

        thumbBeingDragged = rangeSlider->getThumbBeingDragged();
    }
}

void MappedParameterComponent::mouseUp (const MouseEvent& e)
{
    if (e.eventComponent == rangeSlider)
    {
        if (thumbBeingDragged == 1)
        {
            rangeLabelMin->setVisible (false);
        }
        else if (thumbBeingDragged == 2)
        {
            rangeLabelMax->setVisible (false);
        }

        thumbBeingDragged = -1;
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
        valueLabel->setText (String (mappedParameter.parameter.getValue(), 2), dontSendNotification);

        if (gesture.getValueForMappedParameter (mappedParameter.range, mappedParameter.reversed))
        {
            repaint (rangeSlider->getBounds().withX (rangeSlider->getBounds().getCentreX() - 13)
                                             .withWidth (8));
        }
    }
}

//==============================================================================
void MappedParameterComponent::createLabels()
{
    //=== Value label ===
    addAndMakeVisible (valueLabel = new Label ("Value Label", String (mappedParameter.parameter.getValue(), 2)));
    valueLabel->setEditable (false, false, false);
    valueLabel->setFont (Font (PLUME::UI::font, 11.0f, Font::plain));
    valueLabel->setColour (Label::textColourId, Colour (0xff000000));
    valueLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    valueLabel->setJustificationType (Justification::centredBottom);
    
    //=== range Control labels ===
    addAndMakeVisible (rangeLabelMin = new Label ("Min Label", String (mappedParameter.range.getStart(), 1)));
    addAndMakeVisible (rangeLabelMax = new Label ("Max Label", String (mappedParameter.range.getEnd(), 1)));
    
    // LabelMin style
    rangeLabelMin->setEditable (true, false, false);
    rangeLabelMin->setSize (30, 20);
    rangeLabelMin->setFont (Font (PLUME::UI::font, 8.0f, Font::plain));
    rangeLabelMin->setColour (Label::textColourId, highlightColour);
    rangeLabelMin->setColour (Label::backgroundColourId, Colour (0x00000000));
    rangeLabelMin->setJustificationType (Justification::centred);
    rangeLabelMin->setVisible (false);
    
    // LabelMax style
    rangeLabelMax->setEditable (true, false, false);
    rangeLabelMax->setSize (30, 20);
    rangeLabelMax->setFont (Font (PLUME::UI::font, 8.0f, Font::plain));
    rangeLabelMax->setColour (Label::textColourId, highlightColour);
    rangeLabelMax->setColour (Label::backgroundColourId, Colour (0x00000000));
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
        rangeLabelMin->setCentrePosition (rangeSlider->getBounds().getCentreX() + 15,
                                          rangeSlider->getY() + 10
                                            + (rangeSlider->getHeight() - 20) *
                                              (1 - rangeSlider->getMinValue()));
    }
    else if (&labelToResize == rangeLabelMax)
    {
        rangeLabelMax->setCentrePosition (rangeSlider->getBounds().getCentreX() + 15,
                                          rangeSlider->getY() + 10 
                                            + (rangeSlider->getHeight() - 20) *
                                              (1 - rangeSlider->getMaxValue()));
    }
}

void MappedParameterComponent::createSlider()
{
    addAndMakeVisible (rangeSlider = new Slider ("Range Slider"));
    rangeSlider->setSliderStyle (Slider::TwoValueVertical);
    rangeSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    rangeSlider->setColour (Slider::backgroundColourId, Colour (0x00000000));
    rangeSlider->setColour (Slider::trackColourId, highlightColour);
    rangeSlider->setRange (0.0, 1.0, 0.01);
    rangeSlider->setMinValue (0.0);
    rangeSlider->setMaxValue (1.0);
    rangeSlider->setLookAndFeel (&sliderLookAndFeel);
    rangeSlider->addListener (this);
    rangeSlider->addMouseListener (this, false);
}

//==============================================================================
void MappedParameterComponent::drawCursor (Graphics& g)
{
    Path cursorPath;
    int cursorY = rangeSlider->getY() + 10 
                                      + (rangeSlider->getHeight() - 20) *
                                            (1 - gesture.getValueForMappedParameter (mappedParameter.range,
                                                                                     mappedParameter.reversed));

    cursorPath.startNewSubPath (rangeSlider->getBounds().getCentreX() - 6, cursorY);
	cursorPath.lineTo(rangeSlider->getBounds().getCentreX() - 12, cursorY);

    g.setColour (highlightColour);
    g.strokePath (cursorPath, PathStrokeType (1.0f));
}