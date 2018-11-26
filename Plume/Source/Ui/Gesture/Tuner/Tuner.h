/*
  ==============================================================================

    Tuner.h
    Created: 18 Jul 2018 3:58:44pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

#define CURSOR_SIZE 4
#define W getWidth()
#define H getHeight()

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
//==============================================================================
/*
*/
class Tuner    : public Component
{
public:
    //==============================================================================
    Tuner(const float& val, NormalisableRange<float> gestRange, const Range<float> dispRange, const String unit = "", bool show = true)
        :   value (val), gestureRange (gestRange), displayRange (dispRange), valueUnit (unit), showValue (show)
    {
        TRACE_IN;
        yCursor = getHeight()/3 - CURSOR_SIZE - 2;
        
        addAndMakeVisible(valueLabel = new Label("value Label"));
        valueLabel->setEditable (false, false, false);
        valueLabel->setText (String(int (value)), dontSendNotification);
        valueLabel->setFont (Font (13.0f, Font::plain));
        
        if (!showValue)
        {
            valueLabel->setVisible (false);
        }
            
        valueLabel->setJustificationType (Justification::centred);
    }

    ~Tuner()
    {
        TRACE_IN;
    }

    
    //==============================================================================
    virtual void paint (Graphics& g) override
    {
        g.setColour (Colours::black);
        
        if (showValue && value > displayRange.getStart() && value < displayRange.getEnd())
        {
            drawCursor(g);
        }
    }

    virtual void resized() override
    {
        yCursor = H/3 - CURSOR_SIZE - 2;
        sliderPlacement.setStart ((W*3/4)/8);
        sliderPlacement.setEnd ((W*3/4)*7/8);
        
        valueLabel->setBounds ((W*3/4)*7/16, H*2/3,
                               (W*3/4)/8, H/6);
    }
    
    //==============================================================================
    void updateDisplay()
    {
        float convertedValue = gestureRange.convertFrom0to1 (value);

		// Normal case
        if (convertedValue >= displayRange.getStart() && convertedValue <= displayRange.getEnd())
        {
            if (showValue)   valueLabel->setText (String(int (convertedValue))+valueUnit, dontSendNotification);
            repaint();
        }
    }
    
	virtual void updateComponents() = 0;
    
protected:
    Range<int> sliderPlacement = Range<int> ((W*3/4)/8, (W*3/4)*7/8);
    const String valueUnit;
    
private:
    //==============================================================================
    void drawCursor(Graphics& g)
    {
        int xCursor = 0;
		int valueLab = int (gestureRange.convertFrom0to1(value));
		
		//if (valueUnit != "") valueLab = valueLabel->getText().upToFirstOccurrenceOf(valueUnit, false, false).getIntValue();
		//else                 valueLab = valueLabel->getText().getIntValue();
        
        // if out of bounds doesn't draw anything
		if (valueLab < displayRange.getStart() || valueLab > displayRange.getEnd()) return;
        
        // Formula: x = length * (val - start)/(end - start) + x0;
        xCursor = (sliderPlacement.getLength() - 10)*(valueLab-displayRange.getStart())/(displayRange.getEnd()-displayRange.getStart())
                   + sliderPlacement.getStart()+5;
        
        /*
        // Placement if value exceeds limits
        else
        {
            if (valueLab < displayRange.getStart())      xCursor = sliderPlacement.getStart() + 5;
            else if (valueLab > displayRange.getEnd())   xCursor = sliderPlacement.getEnd() - 5;
        */
        
        // draws the cursor
        Path cursorPath;
        
        cursorPath.startNewSubPath (xCursor - CURSOR_SIZE, yCursor);
        cursorPath.lineTo (xCursor, yCursor + CURSOR_SIZE);
        cursorPath.lineTo (xCursor + CURSOR_SIZE, yCursor);
        
		g.setColour (getLookAndFeel().findColour (Slider::backgroundColourId));
		g.strokePath (cursorPath, { 2.0f, PathStrokeType::curved, PathStrokeType::rounded });
    }
    
    //==============================================================================
    const float& value;
    const Range<float> displayRange;
    NormalisableRange<float> gestureRange;
    const bool showValue;
    
    int yCursor;
    
    //==============================================================================
    //ScopedPointer<Path> displayTriangle;
    ScopedPointer<Label> valueLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tuner)
};
