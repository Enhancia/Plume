/*
  ==============================================================================

    ScannerComponent.h
    Created: 31 Jan 2019 11:42:32am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/PlumeCommon.h"
#include "../../../Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class ScannerComponent    : public Component,
                            private Button::Listener,
                            private Timer
{
public:
    //==============================================================================
    ScannerComponent (PlumeProcessor& proc, int buttonWidth =-1);
    ~ScannerComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void timerCallback() override;

private:
    //==============================================================================
    void scanPlugins (bool clearList = true);
    void cancelScan();
    void scanFinished();
    
    //==============================================================================
    ScopedPointer<TextButton> scanButton;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<PlumeProgressBar> bar;
    
    PlumeProcessor& processor;
    
    //==============================================================================
    int buttonW;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScannerComponent)
};
