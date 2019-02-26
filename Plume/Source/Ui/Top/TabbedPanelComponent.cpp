/*
  ==============================================================================

    TabbedPanelComponent.cpp
    Created: 22 Feb 2019 5:02:08pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "TabbedPanelComponent.h"

//==============================================================================
TabbedPanelComponent::TabbedPanelComponent (PlumeProcessor& proc) : processor (proc)
{
}

TabbedPanelComponent::~TabbedPanelComponent()
{
    removeAllChildren();
    tabs.clear();
}

//==============================================================================
void TabbedPanelComponent::paint (Graphics& g)
{
}

void TabbedPanelComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds();

    auto panelArea = area.removeFromRight (getWidth()*3/4).reduced (2*MARGIN, 2*MARGIN);
    
    if (!tabs.isEmpty())
    {
        int tabHeight = (area.getHeight()/tabs.size()) - (tabs.size() - 1) * MARGIN;

        for (auto* tab : tabs)
        {
            tab->button->setBounds (area.removeFromTop (tabHeight + MARGIN).withTrimmedBottom (MARGIN));
            tab->panel->setBounds (panelArea);
        }
    }
}

//==============================================================================
void TabbedPanelComponent::addTab (Component* panel, String tabName)
{
    tabs.add (new Tab (panel, tabName));
    addAndMakeVisible (tabs.getLast()->button);
    addChildAndSetID (tabs.getLast()->panel, "panel" + tabs.size() - 1);
    findChildWithID ("panel" + tabs.size() - 1)->setVisible (false);
    tabs.getLast()->button->addListener (this);

    if (tabs.size() == 1) switchToTab (0); // Displays the first tab that is added
}

void TabbedPanelComponent::switchToTab (const int tabNumber)
{
    if (tabNumber < 0 || tabNumber >= tabs.size() || tabNumber == selectedTab) return;

    findChildWithID ("panel" + selectedTab)->setVisible (false);
    findChildWithID ("panel" + tabNumber  )->setVisible (true);
    selectedTab = tabNumber;

    repaint();
}

//==============================================================================
void TabbedPanelComponent::buttonClicked (Button* bttn)
{
    for (int i =0; i < tabs.size(); i++)
    {
        if (bttn == tabs[i]->button)
        {
            switchToTab(i);
        }
    }
}