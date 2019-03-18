/*
  ==============================================================================

    GeneralOptionsSubPanel.h
    Created: 25 Feb 2019 11:30:56am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#if JUCE_WINDOWS
#include <windows.h>
#include <ShellAPI.h>
#elif JUCE_MAC
#include <stdlib.h>
#endif

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"
#include "SubPanelComponent.h"



//==============================================================================
/*
*/
class GeneralOptionsSubPanel    : public SubPanelComponent
{
public:
    explicit GeneralOptionsSubPanel (PlumeProcessor& proc);
    ~GeneralOptionsSubPanel();

    void buttonClicked (Button* bttn) override;
private:
    PlumeProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneralOptionsSubPanel)
};
