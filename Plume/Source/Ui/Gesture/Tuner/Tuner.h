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

#ifndef W 
#define W Component::getWidth()
#endif

#ifndef H 
#define H Component::getHeight()
#endif

//==============================================================================
/*
*/
class Tuner    : public Component
{
public:
    //==============================================================================
	Tuner(const float& val, NormalisableRange<float> gestRange, const Range<float> dispRange, const String unit = "", bool show = true);
	~Tuner();

    //==============================================================================
	virtual void paint(Graphics& g) override;
	virtual void resized() override;
    
    //==============================================================================
	virtual void updateDisplay();
	virtual void updateComponents() = 0;
/*
    virtual void mouseDown (MouseEvent& e);
    virtual void mouseDrag (MouseEvent& e);
    virtual void mouseUp (MouseEvent& e);*/
	
protected:
    //==============================================================================
    Range<int> sliderPlacement;
    const String valueUnit;
    
private:
    //==============================================================================
    const float& value;
    const Range<float> displayRange;
    NormalisableRange<float> gestureRange;
    const bool showValue;
    
    //==============================================================================
    ScopedPointer<Label> valueLabel;
    ScopedPointer<MovingCursor> cursor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tuner)
};