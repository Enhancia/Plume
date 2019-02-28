/*
  ==============================================================================

    FileOptionsSubPanel.h
    Created: 25 Feb 2019 11:31:12am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"
#include "Ui/Top/ScannerComponent.h"
#include "SubPanelComponent.h"

//==============================================================================
/*
*/
class FileOptionsSubPanel    : public SubPanelComponent
{
public:
    //==============================================================================
    FileOptionsSubPanel (PlumeProcessor& proc);
    ~FileOptionsSubPanel();
    
    //==============================================================================
    void labelTextChanged (Label* lbl) override;

private:
    //==============================================================================
    ScopedPointer<ScannerComponent> scanner;
    
    //==============================================================================
    PlumeProcessor& processor;
    
    Value pluginDir;
    Value presetDir;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileOptionsSubPanel)
};