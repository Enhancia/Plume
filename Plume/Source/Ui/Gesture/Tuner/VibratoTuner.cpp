/*
  ==============================================================================

    VibratoTuner.cpp
    Created: 4 Jul 2019 1:44:31pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "VibratoTuner.h"

VibratoTuner::VibratoTuner  (const std::atomic<float>& val, NormalisableRange<float> gestRange,
				  			 const std::atomic<float>& vibratoIntensity, NormalisableRange<float> intRange,
    			  			 float& vibGain, const NormalisableRange<float> gainMax,
    			  			 float& thresh, const NormalisableRange<float> threshMax)
    : Tuner ("", getPlumeColour (vibratoHighlight)),
      value (val), gestureRange (gestRange),
      intensity (vibratoIntensity), intensityRange (intRange),
      gain (vibGain), parameterMaxGain (gainMax),
      threshold (thresh), parameterMaxThreshold (threshMax)
{
	setLookAndFeel (&vibratoTunerLookAndFeel);

    createSliders();
    createLabels();
}

VibratoTuner::VibratoTuner (Vibrato& vib)
    : VibratoTuner (vib.getValueReference(), vib.getRangeReference(),
    				vib.getIntensityReference(), vib.intensityRange,
    	            vib.gain, Range<float> (0.0f, PLUME::UI::VIBRATO_DISPLAY_MAX),
    	            vib.threshold, Range<float> (0.0f, PLUME::UI::VIBRATO_THRESH_DISPLAY_MAX))
{
}

VibratoTuner::~VibratoTuner()
{
	setLookAndFeel (nullptr);
	gainSlider = nullptr;
	thresholdSlider = nullptr;
	gainLabel = nullptr;
	thresholdLabel = nullptr;
}
    
//==============================================================================
void VibratoTuner::paint (Graphics& g)
{
	drawValueCursor (g);
	drawIntensityCursor (g);

	g.setColour (getPlumeColour (tunerSliderBackground));
	g.setFont (PLUME::font::plumeFont.withHeight (14.0f));
	g.drawText ("THRESHOLD", thresholdSlider->getBounds().withSizeKeepingCentre (100, 50)
														 .withY (thresholdSlider->getBounds().getBottom()),
							 Justification::centredTop);

	g.drawText ("GAIN", gainSlider->getBounds().withSizeKeepingCentre (100, 50)
											   .withY (thresholdSlider->getBounds().getBottom()),
						Justification::centredTop);
}

void VibratoTuner::resized()
{
	tunerArea = getLocalBounds().reduced (getLocalBounds().getWidth()/5,
										  getLocalBounds().getHeight()/5);

	auto area = tunerArea;

	gainSlider->setBounds (area.removeFromRight (area.getWidth()*2 / 3));
	thresholdSlider->setBounds (area.withSizeKeepingCentre (area.getWidth(), (gainSlider->getHeight() / 2) + 10));

	updateLabelBounds(gainLabel.get());
	updateLabelBounds(thresholdLabel.get());
}
    
void VibratoTuner::updateComponents()
{
	if (gainSlider->getThumbBeingDragged() == -1)
    {
        // Sets slider value
        gainSlider->setValue (static_cast<double>(getGain()), dontSendNotification);
        
        // Sets label text
        if (!(gainLabel->isBeingEdited()))
		{
		    gainLabel->setText (String (static_cast<int>(getGain())) + valueUnit, dontSendNotification);
		}
    }

	if (thresholdSlider->getThumbBeingDragged() == -1)
    {
        // Sets slider value
        thresholdSlider->setValue (static_cast<double>(getThreshold()), dontSendNotification);
        //setThresholdSliderColour();
        
        // Sets label text
        if (!(thresholdLabel->isBeingEdited()))
		{
		    thresholdLabel->setText (String (static_cast<int>(getThreshold())) + valueUnit, dontSendNotification);
		}
    }
}

void VibratoTuner::updateDisplay()
{
	computeSmoothIntensity (1.5f);

	if (smoothIntensity > getIntensity() || getIntensity() != lastIntensity || value != lastValue)
	{
		// intensity value repaint
		repaint (thresholdSlider->getBounds().withSizeKeepingCentre (10, thresholdSlider->getHeight())
											 .translated (-10, 0));
		
		// vibrato value repaint
		repaint (gainSlider->getBounds().withSizeKeepingCentre (gainSlider->getWidth(), 10));
	}
}

//==============================================================================
void VibratoTuner::labelTextChanged (Label* lbl)
{
	if (lbl == gainLabel.get())
	{ 
		// checks that the string is numbers only
	    if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
	    {
	        lbl->setText (String (static_cast<int>(getGain())), dontSendNotification);
	        return;
	    }

	    // Gets the float value of the text 
	    float val = lbl->getText().getFloatValue();

	    if (val < 0.0f) val = 0.0f;
	    else if (val > parameterMaxGain.getRange().getEnd()) val = parameterMaxGain.getRange().getEnd();
	    
	    setGain (val);
	    lbl->setText (String (val) + valueUnit, dontSendNotification);
		gainSlider->setValue (val, dontSendNotification);
	}

	else if (lbl == thresholdLabel.get())
	{ 
		// checks that the string is numbers only
	    if (lbl->getText().containsOnly ("-0123456789"+valueUnit) == false)
	    {
	        lbl->setText (String (static_cast<int>(getThreshold())), dontSendNotification);
	        return;
	    }

	    // Gets the float value of the text 
	    float val = lbl->getText().getFloatValue();

	    if (val < 0.0f) val = 0.0f;
	    else if (val > parameterMaxThreshold.getRange().getEnd()) val = parameterMaxThreshold.getRange().getEnd();
	    
	    setThreshold (val);
	    lbl->setText (String (val) + valueUnit, dontSendNotification);
		thresholdSlider->setValue (val, dontSendNotification);
		//setThresholdSliderColour();
	}

	updateLabelBounds (lbl);
}


void VibratoTuner::editorHidden (Label* lbl, TextEditor&)
{
    lbl->setVisible (false);
}

void VibratoTuner::sliderValueChanged (Slider* sldr)
{
	if (sldr == gainSlider.get())
	{
		setGain (static_cast<float> (sldr->getValue()));
		updateLabelBounds (gainLabel.get());
		gainLabel->setText (String (static_cast<int> (getGain())), dontSendNotification);
	}
	else if (sldr == thresholdSlider.get())
	{    
		setThreshold (static_cast<float> (sldr->getValue()));
		updateLabelBounds (thresholdLabel.get());
		thresholdLabel->setText (String (static_cast<int> (getThreshold())), dontSendNotification);
		//setThresholdSliderColour();
	}
}

void VibratoTuner::mouseDown (const MouseEvent& e)
{
	if (e.mods.isLeftButtonDown())
	{
		if (e.getNumberOfClicks() == 1)
		{
			if (e.eventComponent == gainSlider.get())
			{
				gainLabel->setVisible (true);
			}
			else if (e.eventComponent == thresholdSlider.get())
			{
				thresholdLabel->setVisible (true);
			}
		}
		else // Double Click
		{
			if (e.eventComponent == gainSlider.get())
			{
				gainLabel->setVisible (true);
				gainLabel->showEditor();
			}
			else if (e.eventComponent == thresholdSlider.get())
			{
				thresholdLabel->setVisible (true);
				thresholdLabel->showEditor();
			}
		}
	}
}

void VibratoTuner::mouseUp (const MouseEvent& e)
{
	if (e.mods.isLeftButtonDown() && e.getNumberOfClicks() == 1)
	{
		if (e.eventComponent == gainSlider.get())
		{
			gainLabel->setVisible (false);
		}
		else if (e.eventComponent == thresholdSlider.get())
		{
			thresholdLabel->setVisible (false);
		}
	}
}

void VibratoTuner::setColour (const Colour newColour)
{
	Tuner::setColour (newColour);

    gainSlider->setColour (Slider::rotarySliderFillColourId, tunerColour);
    thresholdSlider->setColour (Slider::trackColourId, tunerColour);

    auto setLabelColours = [this] (Label& label)
    {
        label.setColour (Label::textColourId, tunerColour);
        label.setColour (Label::textWhenEditingColourId, tunerColour);
        label.setColour (TextEditor::textColourId, tunerColour);
        label.setColour (TextEditor::highlightColourId, tunerColour.withAlpha (0.2f));
        label.setColour (TextEditor::highlightedTextColourId, tunerColour);
        label.setColour (CaretComponent::caretColourId, tunerColour.withAlpha (0.2f));
    };

    setLabelColours (*gainLabel);
    setLabelColours (*thresholdLabel);
}

void VibratoTuner::createSliders()
{
    addAndMakeVisible (*(gainSlider = std::make_unique<Slider> ("Gain Slider")));
    addAndMakeVisible (*(thresholdSlider = std::make_unique<Slider> ("Threshold Slider")));

    // Gain Slider parameters
	gainSlider->setSliderStyle(Slider::Rotary);
    //gainSlider->setRotaryParameters (MathConstants<float>::pi*5/3, MathConstants<float>::pi*7/3, true);
    gainSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    gainSlider->setColour (Slider::rotarySliderFillColourId, tunerColour);
    gainSlider->setColour (Slider::rotarySliderOutlineColourId, getPlumeColour (tunerSliderBackground));
    gainSlider->setRange (static_cast<double>(parameterMaxGain.getRange().getStart()), static_cast<double>(parameterMaxGain.getRange().getEnd()), 1.0);
    gainSlider->setValue (static_cast<double>(getGain()));
    gainSlider->addListener (this);
    gainSlider->addMouseListener (this, false);

    // ThreshSliderParameters
	thresholdSlider->setSliderStyle(Slider::LinearVertical);
    thresholdSlider->setTextBoxStyle (Slider::NoTextBox, false, 0, 0);
    thresholdSlider->setColour (Slider::backgroundColourId, getPlumeColour (tunerSliderBackground));
    thresholdSlider->setColour (Slider::trackColourId, tunerColour);
    //setThresholdSliderColour();
    thresholdSlider->setRange (static_cast<double>(parameterMaxThreshold.getRange().getStart()), static_cast<double>(parameterMaxThreshold.getRange().getEnd()), 1.0);
    thresholdSlider->setValue (static_cast<double>(getThreshold()));
    thresholdSlider->addListener (this);
    thresholdSlider->addMouseListener (this, false);
}
    
void VibratoTuner::createLabels()
{
    addAndMakeVisible (*(gainLabel = std::make_unique<Label> ("Gain Label",
    	 									  TRANS (String(static_cast<int>(getGain())) + valueUnit))));
    addAndMakeVisible (*(thresholdLabel = std::make_unique<Label> ("Threshold Label",
    									           TRANS (String(static_cast<int>(getThreshold())) + valueUnit))));

    auto setLabelSettings = [this] (Label& label)
    {
        label.setEditable (false, false, false);
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

    setLabelSettings (*gainLabel);
    setLabelSettings (*thresholdLabel);
}

void VibratoTuner::updateLabelBounds (Label* labelToUpdate)
{
	if (labelToUpdate == gainLabel.get())
	{
		float angle = gainSlider->getRotaryParameters().startAngleRadians
		                  + static_cast<float> (((gainSlider->getValue() - gainSlider->getMinimum())
							     / gainSlider->getRange().getLength()))
                             *std::abs (gainSlider->getRotaryParameters().endAngleRadians
                             	- gainSlider->getRotaryParameters().startAngleRadians);

        // Slider radius with a 10-pixels offset
        float radius = (jmin (gainSlider->getWidth(), gainSlider->getHeight()) - 10.0f) / 2.0f
        				   + 10.0f;

		labelToUpdate->setCentrePosition (gainSlider->getBounds().getCentreX()
											 + static_cast<int> (radius * std::cos (angle - MathConstants<float>::halfPi)),
                                          gainSlider->getBounds().getCentreY()
                                          	 + static_cast<int> (radius * std::sin (angle - MathConstants<float>::halfPi)));
	}
	else if (labelToUpdate == thresholdLabel.get())
	{
		labelToUpdate->setCentrePosition (thresholdSlider->getBounds().getCentreX() - 25,
										  thresholdSlider->getBottom() - 10
			                                  - static_cast<int> (thresholdSlider->valueToProportionOfLength(thresholdSlider->getValue())
                                              * (thresholdSlider->getHeight() - 20)));
	}
}

void VibratoTuner::setGain (float newGain)
{
    gain = parameterMaxGain.convertTo0to1 (newGain);
}

void VibratoTuner::setThreshold (float newTresh)
{
    threshold = parameterMaxThreshold.convertTo0to1 (newTresh);
}

float VibratoTuner::getGain()
{
    return parameterMaxGain.convertFrom0to1 (gain);
}

float VibratoTuner::getThreshold()
{
    return parameterMaxThreshold.convertFrom0to1 (threshold);
}

float VibratoTuner::getIntensity()
{
    return intensityRange.convertFrom0to1 (intensity);
}

void VibratoTuner::drawValueCursor (Graphics& g)
{
	lastValue = value;

	int offset = (getIntensity() < getThreshold()) ? 0
	                                          : static_cast<int> ((value - 0.5f) * (static_cast<float> (gainSlider->getWidth()) - 30.0f))
	                                                           * static_cast<int>(getGain())/85;

	juce::Point<int> cursorPoint = {gainSlider->getBounds().getCentreX() + offset,
							  gainSlider->getBounds().getCentreY()};

    g.setColour ((getIntensity() < getThreshold()) ? getPlumeColour (tunerSliderBackground) : tunerColour);
    g.fillEllipse (juce::Rectangle<float> (5.0f, 5.0f).withCentre (cursorPoint.toFloat()));
}

void VibratoTuner::drawIntensityCursor (Graphics& g)
{
	lastIntensity = getIntensity();

    juce::Point<float> cursorPoint (static_cast<float> (thresholdSlider->getBounds().getCentreX() - 10),
                              jmax (static_cast<float> (thresholdSlider->getBottom()) - 10.0f - (static_cast<float> (thresholdSlider->getHeight()) - 20.0f)
                              											    * smoothIntensity/100.0f,
                              		static_cast<float>(thresholdSlider->getY() + 10)));

	Path cursorPath;
    cursorPath.addTriangle ({cursorPoint.x - 3.0f, cursorPoint.y - 3.0f},
                            {cursorPoint.x + 3.0f, cursorPoint.y       },
                            {cursorPoint.x - 3.0f, cursorPoint.y + 3.0f});

    g.setColour ((getIntensity() < getThreshold()) ? getPlumeColour (tunerSliderBackground) : tunerColour);
    g.fillPath (cursorPath);
}

void VibratoTuner::setThresholdSliderColour()
{
	thresholdSlider->setColour (Slider::trackColourId,
							    (getIntensity() < getThreshold()) ? getPlumeColour (tunerSliderBackground)
			                       							 : tunerColour);
}

void VibratoTuner::computeSmoothIntensity (float smoothnessRamp)
{
	float decrement = std::pow (smoothnessRamp, incrementalSmoothFactor);

	if (getIntensity() > smoothIntensity - decrement)
	{
		smoothIntensity = getIntensity();
		incrementalSmoothFactor = 1.0f;
	}
	else
	{
		smoothIntensity -= decrement;
		incrementalSmoothFactor += 0.5f;
	}
}