/*
  ==============================================================================

    Tuner.h
    Created: 18 Jul 2018 3:58:44pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/PlumeCommon.h"
#include "../../LookAndFeel/PlumeLookAndFeel.h"
#include "../../Common/MovingCursor.h"

//==============================================================================
/*
*/
class Tuner    : public PlumeComponent
{
public:
    //==============================================================================
	Tuner (const String unit = String(), Colour colour = Colour (0xff7c80de))
        : tunerColour (colour), valueUnit(unit)
    {}

	~Tuner() {}

    //==============================================================================
    virtual const String getInfoString() override 
    {
        return "Gesture sensitivity Tuner: \n\n"
               "- Use the sliders to tweak the gesture's behaviour.\n";
    }

    virtual void update() override {}

    //==============================================================================
	virtual void paint(Graphics&) override {}
	virtual void resized() override {}
    
    //==============================================================================
	virtual void updateDisplay() {}
	virtual void updateComponents() = 0;

    //==============================================================================
    Colour getColour() { return tunerColour; }
    virtual void setColour (const Colour newColour) { tunerColour = newColour; }

protected:
    Colour tunerColour;
    const String valueUnit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tuner)
};
