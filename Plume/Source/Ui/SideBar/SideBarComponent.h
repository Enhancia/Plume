/*
  ==============================================================================

    SideBarComponent.h
    Created: 12 Dec 2018 4:52:31pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Plugin/PluginProcessor.h"
#include "PresetComponent.h"
#include "InfoPanel.h"


//==============================================================================
/*
*/
class SideBarComponent    : public Component,
                            public PlumeComponent,
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
    std::unique_ptr<PresetComponent> presetComponent;
    std::unique_ptr<InfoPanel> infoPanel;
    std::unique_ptr<ShapeButton> optionsButton;
    std::unique_ptr<PlumeShapeButton> hideInfoButton;

    bool infoHidden = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SideBarComponent)
};
