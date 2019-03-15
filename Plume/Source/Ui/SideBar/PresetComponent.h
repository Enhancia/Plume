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
#include "Ui/SideBar/FilterBox.h"
#include "Ui/SideBar/TypeToggleComponent.h"
#include "Ui/SideBar/PresetSearchBar.h"

//==============================================================================
/*
*/

class PresetComponent    : public PlumeComponent,
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
    ScopedPointer<PresetBox> presetBox;
    ScopedPointer<FilterBox> filterBox;
    ScopedPointer<TypeToggleComponent> typeToggle;
    ScopedPointer<ComboBox> pluginSelectBox;
    ScopedPointer<PresetSearchBar> searchBar;
    
    //==============================================================================
    PlumeProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetComponent)
};
