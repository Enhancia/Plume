/*
  ==============================================================================

    MovingCursor.h
    Created: 4 Dec 2018 9:55:10am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../LookAndFeel/PlumeLookAndFeel.h"

class MovingCursor  : public Component
{
public:
    //==============================================================================
	MovingCursor(const float& val, const Range<float> disp,
		NormalisableRange<float> convt = NormalisableRange<float>(0.0f, 1.0f, 0.001f), int size = 4, int offsetFromSides = 5);
	~MovingCursor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	//==============================================================================
	bool isSamePosition();
    
private:
    //==============================================================================
    void drawCursor (Graphics&);
	int getPosition ();
    
    //==============================================================================
    const float& value;
    const Range<float> displayRange;
    NormalisableRange<float> convertRange;
    
    int cursorSize;
	int lastx;
	int offset;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MovingCursor)
};