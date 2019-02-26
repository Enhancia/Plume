/*
  ==============================================================================

    GeneralOptionsSubPanel.h
    Created: 25 Feb 2019 11:30:56am
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
class GeneralOptionsSubPanel    : public Component
{
public:
    GeneralOptionsSubPanel();
    ~GeneralOptionsSubPanel();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneralOptionsSubPanel)
};
