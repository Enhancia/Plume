/*
  ==============================================================================

    HeaderComponent.h
    Created: 12 Dec 2018 4:53:18pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class HeaderComponent    : public Component
{
public:
    HeaderComponent (PlumeProcessor& proc);
    ~HeaderComponent();

    void paint (Graphics&) override;
    void resized() override;
    
    void update();

private:
    PlumeProcessor& processor;
    ScopedPointer<Label> pluginNameLabel;
    ScopedPointer<Label> presetNameLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComponent)
};
