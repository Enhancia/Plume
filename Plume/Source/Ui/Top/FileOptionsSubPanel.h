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

#if JUCE_WINDOWS
#include <windows.h>
#include <ShellAPI.h>
#elif JUCE_MAC
#include <stdlib.h>
#endif

//==============================================================================
/*
*/
class FileOptionsSubPanel    : public Component,
                               private Button::Listener,
                               private Label::Listener
{
public:
    //==============================================================================
    FileOptionsSubPanel (PlumeProcessor& proc);
    ~FileOptionsSubPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void labelTextChanged (Label* lbl) override;

private:
    //==============================================================================
    ScopedPointer<Label> presetDirLabel;
    ScopedPointer<Label> pluginDirLabel;
    
    ScopedPointer<ShapeButton> scanButton;
    ScopedPointer<ScannerComponent> scanner;
    ScopedPointer<TextButton> mailButton;
    
    PlumeProcessor& processor;
    
    Value pluginDir;
    Value presetDir;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileOptionsSubPanel)
};