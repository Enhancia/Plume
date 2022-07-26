/*
  ==============================================================================

    PresetComponent.h
    Created: 20 Jun 2018 10:58:08am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Plugin/PluginProcessor.h"
#include "PresetBox.h"
#include "FilterBox.h"
#include "TypeToggleComponent.h"
#include "PresetSearchBar.h"

//==============================================================================
/*
*/

class PresetComponent    : public Component,
                           public PlumeComponent,
                           private ComboBox::Listener
{
public:
    //==============================================================================
    PresetComponent (PlumeProcessor& p);
    ~PresetComponent();
    
    //==============================================================================
    // PlumeComponent
    const String getInfoString() override;
    void update() override;
    
    //==============================================================================
    // Component
    void paint (Graphics& g) override;
    void paintOverChildren (Graphics& g) override;
    void resized() override;
    void focusLost (FocusChangeType cause) override;
    
    //==============================================================================
    void comboBoxChanged (ComboBox* cmbx) override;
    
    //==============================================================================
    void savePreset();
    void addNewPreset();

private:
    //==============================================================================
    void createComboBox();
    
    //==============================================================================
    std::unique_ptr<PresetBox> presetBox;
    std::unique_ptr<FilterBox> filterBox;
    std::unique_ptr<TypeToggleComponent> typeToggle;
    std::unique_ptr<ComboBox> pluginSelectBox;
    std::unique_ptr<PresetSearchBar> searchBar;
    
    //==============================================================================
    PlumeProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetComponent)
};
