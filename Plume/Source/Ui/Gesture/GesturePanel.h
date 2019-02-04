/*
  ==============================================================================

    GesturePanel.h
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Wrapper/PluginWrapper.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

//==============================================================================
/*
*/
class GesturePanel    : public Component,
                        public Timer,
                        private AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    GesturePanel (GestureArray& gestureArray, PluginWrapper& wrap, AudioProcessorValueTreeState& params, int freqHz);
    ~GesturePanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void timerCallback() override;
    void parameterChanged (const String &parameterID, float newValue) override;

    //==============================================================================
    void initialize();
    void addGestureComponent (Gesture& gest);
    void removeGestureComponent (int gestureId);
    void removeListenerForAllParameters();
    
private:
    //==============================================================================
    class GestureComponent;
    int freq;

    //==============================================================================
    OwnedArray<GestureComponent> gestureComponents;
    GestureArray& gestureArray;
    PluginWrapper& wrapper;
    AudioProcessorValueTreeState& parameters;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};
