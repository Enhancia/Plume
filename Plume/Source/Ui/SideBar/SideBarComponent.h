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
#include "Plugin/PluginProcessor.h"


//==============================================================================
/*
*/
class SideBarComponent    : public PlumeComponent,
                            private Button::Listener
{
public:
    //==============================================================================
    SideBarComponent (PlumeProcessor& proc);
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
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    PlumeProcessor& processor;
    ScopedPointer<PresetComponent> presetComponent;
    ScopedPointer<ShapeButton> optionsButton;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SideBarComponent)
};
