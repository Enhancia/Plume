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
#include "Common/PlumeCommon.h"
#include "Ui/Common/MovingCursor.h"

//==============================================================================
/*
*/
class Tuner    : public Component
{
public:
    //==============================================================================
	Tuner (const String unit = String(), Colour colour = Colour (0xff7c80de));
	~Tuner();

    //==============================================================================
	virtual void paint(Graphics& g) override;
	virtual void resized() override;
    
    //==============================================================================
	virtual void updateDisplay();
	virtual void updateComponents() = 0;

protected:
    Colour tunerColour;
    const String valueUnit;
    Range<int> sliderPlacement;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tuner)
};