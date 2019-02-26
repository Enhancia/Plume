/*
  ==============================================================================

    TabbedPanelComponent.h
    Created: 22 Feb 2019 5:02:08pm
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
class TabbedPanelComponent    : public Component,
                                private Button::Listener
{
public:
    //==============================================================================
    TabbedPanelComponent (PlumeProcessor& proc);
    ~TabbedPanelComponent();

    //==============================================================================
    void addTab (Component* panel, String tabName);
    void switchToTab (const int tabNumber);

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    struct Tab
    {
        Tab (Component* panelToUse, String tabName = "")
        {
            tabName = name;
            panel = panelToUse;

            button = new TextButton (name);
            button->setButtonText (name);
        }

        ~Tab ()
        {
            button = nullptr;
        }

        ScopedPointer<TextButton> button;
        ScopedPointer<Component> panel;
        const String name;
    };

    //==============================================================================
    PlumeProcessor& processor;
    OwnedArray<Tab> tabs;
    int selectedTab = -1;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TabbedPanelComponent)
};
