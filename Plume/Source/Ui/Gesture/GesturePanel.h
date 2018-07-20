/*
  ==============================================================================

    GesturePanel.h
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class GesturePanel    : public Component,
                        public Timer
{
public:
    GesturePanel (GestureArray& gestureArray, int freqHz);
    ~GesturePanel();

    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

    void initialize();
    
private:
    class GestureComponent;
    
    OwnedArray<GestureComponent> gestureComponents;
    GestureArray& gestureArray;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};
