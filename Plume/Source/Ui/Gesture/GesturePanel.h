/*
  ==============================================================================

    GesturePanel.h
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Wrapper/PluginWrapper.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

//==============================================================================
/*
*/
class GesturePanel    : public Component,
                        public Timer
{
public:
    GesturePanel (GestureArray& gestureArray, PluginWrapper& wrap, int freqHz);
    ~GesturePanel();

    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

    void initialize();
    
private:
    class GestureComponent;
    int freq;
    
    OwnedArray<GestureComponent> gestureComponents;
    GestureArray& gestureArray;
    PluginWrapper& wrapper;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};
