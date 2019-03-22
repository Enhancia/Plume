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
    using namespace PLUME::UI;

    auto area = getLocalBounds();
    area.removeFromRight (getWidth()*4/5).reduced (2*MARGIN, 2*MARGIN); // Panel Area

    auto tabsArea = area.reduced (0, 2*MARGIN).withTrimmedLeft (MARGIN);

    if (!tabs.isEmpty())
    {
        //int tabHeight = (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size();
        int tabHeight = 30;

        for (int i =0; i < tabs.size(); i++)
        {
            auto tabArea = tabsArea.removeFromTop (tabHeight);

            if (i == selectedTab)
            {
                g.setColour (currentTheme.getColour (PLUME::colour::topPanelSubText));
                g.fillRect (tabArea.withWidth (3));
            }

            g.setColour (i == selectedTab ? currentTheme.getColour (PLUME::colour::topPanelMainText)
                                          : currentTheme.getColour (PLUME::colour::topPanelSubText));
            g.setFont (font::plumeFontBook.withHeight (16.0f));
            g.drawText (tabs[i]->name, tabArea.withTrimmedLeft (PLUME::UI::MARGIN + 5),
                        Justification::centredLeft, false);
            tabsArea.removeFromTop (MARGIN);
        }
    }
}

void TabbedPanelComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds();

    auto panelArea = area.removeFromRight (getWidth()*4/5).reduced (MARGIN, 2*MARGIN);
    auto tabsArea = area.reduced (0, 2*MARGIN).withTrimmedLeft (MARGIN);
    
    if (!tabs.isEmpty())
    {
        //int tabHeight = (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size();
        int tabHeight = 30;

        for (auto* tab : tabs)
        {
            tab->button->setBounds (tabsArea.removeFromTop (tabHeight));
            tabsArea.removeFromTop (MARGIN);

            tab->panel->setBounds (panelArea);
        }
    }
}

//==============================================================================
void TabbedPanelComponent::addTab (Component* panel, String tabName)
{
    tabs.add (new Tab (panel, tabName));
    addAndMakeVisible (tabs.getLast()->button);
    addChildAndSetID (tabs.getLast()->panel, "panel" + String(tabs.size() - 1));
    findChildWithID ("panel" + String(tabs.size() - 1))->setVisible (false);
    tabs.getLast()->button->addListener (this);

    // Displays the first tab that is added
    if (tabs.size() == 1)
    {
        switchToTab (0);
    } 
}

void TabbedPanelComponent::switchToTab (const int tabNumber)
{
    if (tabNumber < 0 || tabNumber >= tabs.size()) return;

	if (tabNumber == selectedTab)
		if (auto* pan = findChildWithID("panel" + String(selectedTab))) pan->setVisible(true);

    findChildWithID ("panel" + String(selectedTab))->setVisible (false);
    findChildWithID ("panel" + String(tabNumber)  )->setVisible (true);
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