/*
  ==============================================================================

    ScannerComponent.h
    Created: 31 Jan 2019 11:42:32am
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
class ScannerComponent    : public Component,
                            private Button::Listener,
                            private Timer
{
public:
    //==============================================================================
    ScannerComponent (PlumeProcessor& proc);
    ~ScannerComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void timerCallback() override;

private:
    //==============================================================================
    void scanPlugins();
    void cancelScan();
    void scanFinished();
    
    void setComponentsVisible();
    
    //==============================================================================
    ScopedPointer<ShapeButton> scanButton;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<PlumeProgressBar> bar;
    
    PlumeProcessor& processor;
    
    //==============================================================================
    bool scanning = false;
    String pluginBeingScanned = "";
    float scanProgress = 0.0f;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScannerComponent)
};
