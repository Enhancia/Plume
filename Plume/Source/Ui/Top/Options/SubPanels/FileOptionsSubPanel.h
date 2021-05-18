/*
  ==============================================================================

    FileOptionsSubPanel.h
    Created: 25 Feb 2019 11:31:12am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../../Common/PlumeCommon.h"
#include "../../../../Plugin/PluginProcessor.h"
#include "../ScannerComponent.h"
#include "SubPanelComponent.h"

//==============================================================================
/*
*/
class FileOptionsSubPanel    : public SubPanelComponent
{
public:
    //==============================================================================
    explicit FileOptionsSubPanel (PlumeProcessor& proc);
    ~FileOptionsSubPanel();
    
    //==============================================================================
    void fileScanned (const String& scannerID, const File& fileThatWasScanned) override;
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    std::unique_ptr<ScannerComponent> scanner;
    
    //==============================================================================
    PlumeProcessor& processor;
    
    Value pluginDir;
    Value presetDir;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileOptionsSubPanel)
};