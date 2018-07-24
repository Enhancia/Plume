/*
  ==============================================================================

    Tuner.h
    Created: 18 Jul 2018 3:58:44pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"

#define CURSOR_SIZE 10
#define W getWidth()
#define H getHeight()

//==============================================================================
/*
*/
class Tuner    : public Component
{
public:
    //==============================================================================
    Tuner(const float& val, const Range<float>& totRange, const String unit = "")
        :   value (val), totalRange (totRange), valueUnit (unit)
    {
        yCursor = getHeight()/3 - CURSOR_SIZE;
        
        addAndMakeVisible(valueLabel = new Label("value Label"));
        valueLabel->setEditable (false, false, false);
        valueLabel->setText (String(value), dontSendNotification);
        valueLabel->setFont (Font (13.0f, Font::plain));
        valueLabel->setColour (Label::textColourId, Colour(0xffffffff));
        valueLabel->setColour (Label::backgroundColourId, Colour(0xff000000));
        valueLabel->setJustificationType (Justification::centred);
        valueLabel->setBounds ( (W*3/4)*3/8, H*2/3, (W*3/4)/4, H/6);
    }

    ~Tuner()
    {
    }

    
    //==============================================================================
    virtual void paint (Graphics& g) override
    {
        //g.fillAll (Colour (0x50ffffff));
        
        g.setColour (Colours::black);
        
        //g.drawRect(0, 0, getWidth(), getHeight()); // outline
        //g.drawRect(W*3/4, 0, 1, getHeight()); // boundary slider|values
        drawCursor(g);
    }

    virtual void resized() override
    {
        yCursor = H/3 - CURSOR_SIZE;
        sliderPlacement.setStart ((W*3/4)/8);
        sliderPlacement.setEnd ((W*3/4)*7/8);
        
        valueLabel->setBounds ((W*3/4)*7/16, H*2/3,
                               (W*3/4)/8, H/6);
    }
    
    //==============================================================================
    void updateDisplay()
    {
        valueLabel->setText (String(int (value))+valueUnit, dontSendNotification);
        repaint();
    }
    
protected:
    Range<int> sliderPlacement = Range<int> ((W*3/4)/8, (W*3/4)*7/8);
    const String valueUnit;
    
private:
    //==============================================================================
    void drawCursor(Graphics& g)
    {
        int xCursor;
        
        // Regular cursor placement
        if (value > totalRange.getStart() && value < totalRange.getEnd())
        {
            // Formula: x = length * (val - start)/(end - start) + x0;
            xCursor = (sliderPlacement.getLength() - 10)*(value-totalRange.getStart())/(totalRange.getEnd()-totalRange.getStart())
                      + sliderPlacement.getStart()+5;
        }
        
        // Placement if value exceeds limits
        else 
        {
            if (value < totalRange.getStart())      xCursor = sliderPlacement.getStart() + 5;
            else if (value > totalRange.getEnd())   xCursor = sliderPlacement.getEnd() - 5;
        }
        
        g.drawRect (xCursor, yCursor, 1, CURSOR_SIZE);
    }
    
    //==============================================================================
    const float& value;
    const Range<float>& totalRange;
    
    int yCursor;
    //ScopedPointer<Path> displayTriangle;
    ScopedPointer<Label> valueLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tuner)
};