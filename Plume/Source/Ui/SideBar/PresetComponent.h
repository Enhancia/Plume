/*
  ==============================================================================

    PresetComponent.h
    Created: 20 Jun 2018 10:58:08am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"
#include "Ui/SideBar/PresetBox.h"

//==============================================================================
/*
*/

class PresetComponent    : public Component,
                           private Button::Listener
{
public:
    //==============================================================================
    PresetComponent(PlumeProcessor& p);
    ~PresetComponent();

    //==============================================================================
    void paint (Graphics& g) override;
    void paintOverChildren (Graphics& g) override;
    void resized() override;
    void focusLost (FocusChangeType cause) override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    
    //==============================================================================
    void savePreset();
    void addNewPreset();
    
    //==============================================================================
    void update();

private:
    //==============================================================================
    ScopedPointer<PresetBox> presetBox;
    ScopedPointer<TextButton> saveButton;
    ScopedPointer<TextButton> newButton;
    
    //==============================================================================
    PlumeProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetComponent)
};
