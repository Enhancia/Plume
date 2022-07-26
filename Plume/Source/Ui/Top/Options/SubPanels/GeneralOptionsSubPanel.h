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

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../../Common/PlumeCommon.h"
#include "../../../../Plugin/PluginProcessor.h"
#include "SubPanelComponent.h"



//==============================================================================
/*
*/

class ContactSubPanel : public SubPanelComponent
{
public:
    //==============================================================================
    ContactSubPanel();
    ~ContactSubPanel();

    //==============================================================================
    void buttonClicked(Button* bttn) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContactSubPanel)
};

class AboutPanel: public Component
{
public:
    //==============================================================================
    AboutPanel();
    ~AboutPanel();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    std::unique_ptr<ContactSubPanel> contactComponent;

    //==============================================================================
    juce::Rectangle<int> aboutArea;
    juce::Rectangle<int> contactArea;
    juce::Rectangle<int> creditsArea;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AboutPanel)
};