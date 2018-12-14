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
class SideBarComponent    : public Component
{
public:
    SideBarComponent (PlumeProcessor& proc);
    ~SideBarComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    PlumeProcessor& processor;
    ScopedPointer<PresetComponent> presetComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SideBarComponent)
};
