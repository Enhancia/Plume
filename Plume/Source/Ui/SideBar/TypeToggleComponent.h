/*
  ==============================================================================

    TypeToggleComponent.h
    Created: 8 Feb 2019 11:25:26am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class TypeToggleComponent    : public Component,
                               private Button::Listener
{
public:
    //==============================================================================
    TypeToggleComponent (PlumeProcessor& p);
    ~TypeToggleComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void paintOverChildren (Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    void toggleButton (int buttonId);
    
    //==============================================================================
    PlumeProcessor& processor;
    OwnedArray<TextButton> toggles;
    CriticalSection togglesLock;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TypeToggleComponent)
};
