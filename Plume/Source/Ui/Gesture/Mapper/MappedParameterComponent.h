/*
  ==============================================================================

    MappedParameterComponent.h
    Created: 24 Jul 2018 8:43:58pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Gesture/Gesture.h"

#define W getWidth()
#define H getHeight()

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))

//==============================================================================
/*
*/
class MappedParameterComponent    : public Component,
                                    private Label::Listener,
									private Button::Listener
{
public:
    //==============================================================================
    MappedParameterComponent(Gesture& gest, Gesture::MappedParameter& mappedParam, const int id)
        : gesture (gest), mappedParameter (mappedParam), paramId (id)
    {
        TRACE_IN;
        // Creates the close button
        String PlumeDir = File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getFullPathName();
        Image close = ImageFileFormat::loadFrom (File (PlumeDir + "/Resources/Images/Gestures/Close.png"));
    
        addAndMakeVisible (closeButton = new ImageButton ("Close Button"));
        closeButton->setImages (false, true, true,
								close, 1.0f, Colour (0x00000000),
								close, 0.5f, Colour (0x00000000),
								close, 0.7f, Colour (0x501600f0));
        closeButton->addListener (this);
        
        // Creates the labels
        createLabels();
    }
    
    ~MappedParameterComponent()
    {
        TRACE_IN;
        closeButton == nullptr;
        valueLabel == nullptr;
        rangeLabelMin == nullptr;
        rangeLabelMax == nullptr;
    }

    //==============================================================================
    void paint (Graphics& g) override
    {   
        // Parameter Name and number text
        {
			int x = 0,
				y = 0,
				width = W*2/3,
				height = H/3;
				
            g.setColour (Colours::black);
                            
            //name
            String text (TRANS(mappedParameter.parameter.getName(20)));
            g.setFont (Font (10.0f, Font::plain).withTypefaceStyle ("Regular"));
            g.drawText (text, x, y, width, height,
                        Justification::centred, true);
            //number
            /*
            g.setFont (Font (10.0f, Font::italic).withTypefaceStyle ("Regular"));
            g.drawText (String (paramId), x, y, width, height,
                        Justification::topLeft, true);
            */
        }
        
        // Param full range line
        {
			int x = 2,
				y = H/3 - 3,
				width = W*2/3 - 4,
				height = 2;
				
            g.setColour (Colours::black);    
            g.fillRect(x, y, width, height);
        }
        
        drawCursors(g);
    }
    
	void resized() override
	{
	    closeButton->setBounds (W*2/3, H/3, 10, 10);
        valueLabel->setBounds (0, H/3, W*2/3, H/3 - 2);
	    rangeLabelMin->setBounds (2, H*2/3, W/3 - 4, H/4);
	    rangeLabelMax->setBounds (W/3 + 2, H*2/3, W/3 - 4, H/4);
	    
	    repaint();
	}
	
    
    //==============================================================================
    void buttonClicked (Button* bttn) override
    {
        gesture.deleteParameter (paramId);
    }
    
    void labelTextChanged (Label* lbl) override
    {
        // checks that the string is numbers only (and dot)
        if (lbl->getText().containsOnly ("-.0123456789") == false)
        {
            if (lbl == rangeLabelMin)       lbl->setText (String (mappedParameter.range.getStart(), 2), dontSendNotification);
            else if (lbl == rangeLabelMax)  lbl->setText (String (mappedParameter.range.getEnd(), 2), dontSendNotification);

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
                lbl->setText (String (mappedParameter.range.getStart(), 2), dontSendNotification);
            }
        }
        else if (lbl == rangeLabelMax)
        {
            // Max < Min
            if ( val < mappedParameter.range.getStart()) val = mappedParameter.range.getStart();
            
            // Normal case
            {
				mappedParameter.range.setEnd (val);
                lbl->setText (String (mappedParameter.range.getEnd(), 2), dontSendNotification);
            }
        }
        
        modifyLabelCursors();
    }
    
    void updateDisplay()
    {
        valueLabel->setText (String (mappedParameter.parameter.getValue(), 2), dontSendNotification);
        modifyValueCursor();
		repaint();
    }
    
private:
    //==============================================================================
	void createLabels()
    {
        //=== Value label ===
        
        addAndMakeVisible (valueLabel = new Label ("Value Label", String (mappedParameter.parameter.getValue(), 2)));
        
        valueLabel->setEditable (false, false, false);
        valueLabel->setFont (Font (11.0f, Font::plain));
        valueLabel->setColour (Label::textColourId, Colour(0xff000000));
        valueLabel->setColour (Label::backgroundColourId, Colour(0x00000000));
        valueLabel->setJustificationType (Justification::centredBottom);
        
        valueLabel->setBounds (0, H/3, W*2/3, H/3 - 2);
        
        
        //=== range Control labels ===
        
        addAndMakeVisible (rangeLabelMin = new Label ("Min Label", String (mappedParameter.range.getStart(), 2)));
        addAndMakeVisible (rangeLabelMax = new Label ("Max Label", String (mappedParameter.range.getEnd(), 2)));
        
        // LabelMin style
        rangeLabelMin->setEditable (true, false, false);
        rangeLabelMin->setFont (Font (11.0f, Font::plain));
        rangeLabelMin->setColour (Label::textColourId, Colour(0xffffffff));
        rangeLabelMin->setColour (Label::backgroundColourId, Colour(0xff000000));
        rangeLabelMin->setJustificationType (Justification::centred);
        
        // LabelMax style
        rangeLabelMax->setEditable (true, false, false);
        rangeLabelMax->setFont (Font (11.0f, Font::plain));
        rangeLabelMax->setColour (Label::textColourId, Colour(0xffffffff));
        rangeLabelMax->setColour (Label::backgroundColourId, Colour(0xff000000));
        rangeLabelMax->setJustificationType (Justification::centred);
        
        // Labels settings
        rangeLabelMin->setBounds (2, H*2/3, W/3 - 4, H/4);
        rangeLabelMin->addListener (this);
        
        rangeLabelMax->setBounds (W/3 + 2, H*2/3, W/3 - 4, H/4);
        rangeLabelMax->addListener (this);
    }
    
    //==============================================================================
	void modifyLabelCursors()
    {
    }
    
    void modifyValueCursor()
    {
    }
    
    void drawCursors(Graphics& g)
    {
    }
    
    //==============================================================================
    Gesture& gesture;
    Gesture::MappedParameter& mappedParameter;
    const int paramId;

    //==============================================================================
    ScopedPointer<ImageButton> closeButton;
    ScopedPointer<Label> valueLabel;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappedParameterComponent)
};
