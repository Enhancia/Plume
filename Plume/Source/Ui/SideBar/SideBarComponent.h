/*
  ==============================================================================

    SideBarComponent.h
    Created: 12 Dec 2018 4:52:31pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Ui/SideBar/PresetComponent.h"
#include "Ui/SideBar/InfoPanel.h"
#include "Plugin/PluginProcessor.h"


//==============================================================================
/*
*/
class SideBarComponent    : public PlumeComponent,
                            private Button::Listener
{
public:
    //==============================================================================
    SideBarComponent (PlumeProcessor& proc, Component& optsPanel);
    ~SideBarComponent();

    //==============================================================================
    // PlumeComponent
    const String getInfoString() override;
    void update() override;
    
    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void addInfoPanelAsMouseListener (Component* componentToListen);

    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;

private:
    //==============================================================================
    void createHideInfoButtonPath();

    //==============================================================================
    PlumeProcessor& processor;
    Component& optionsPanel;
    ScopedPointer<PresetComponent> presetComponent;
    ScopedPointer<InfoPanel> infoPanel;
    ScopedPointer<ShapeButton> optionsButton;
    ScopedPointer<ShapeButton> hideInfoButton;

    bool infoHidden = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SideBarComponent)
};
