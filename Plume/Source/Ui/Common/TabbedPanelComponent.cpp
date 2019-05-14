/*
  ==============================================================================

    TabbedPanelComponent.cpp
    Created: 22 Feb 2019 5:02:08pm
    Author:  Alex

  ==============================================================================
*/

#include "TabbedPanelComponent.h"

//==============================================================================
TabbedPanelComponent::TabbedPanelComponent()
{
    using namespace PLUME;

    Colour defaultColours[] = {
                                  UI::currentTheme.getColour (colour::topPanelSubText),
                                  UI::currentTheme.getColour (colour::topPanelMainText),
                                  Colour (0x00000000),
                                  Colour (0x00000000),
                                  UI::currentTheme.getColour (colour::topPanelSubText)
                              };

    for (int i=0; i < numColours; i++)
    {
        colours.add (new Colour (defaultColours[i]));
    }
}

TabbedPanelComponent::~TabbedPanelComponent()
{
    removeAllChildren();
    tabs.clear();
    colours.clear();
}

//==============================================================================
void TabbedPanelComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;

    juce::Rectangle<int> tabsArea;

    if (style == tabsVertical)
    {
        tabsArea = getLocalBounds().removeFromLeft (getWidth()*1/5)
                                   .reduced (0, 2*MARGIN);
    }
    else
    {
        tabsArea = getLocalBounds().removeFromTop (30)
                                   .reduced (2*MARGIN, 0);
    }
    

    if (!tabs.isEmpty())
    {
        //int tabHeight = (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size();
        int tabHeightOrWidth = (style == tabsVertical)
                                    ? jmin (30 , (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size())
                                    : (tabsArea.getWidth() - (tabs.size() - 1) * MARGIN)/tabs.size();

        for (int i =0; i < tabs.size(); i++)
        {
            auto tabArea = (style == tabsVertical) ? tabsArea.removeFromTop (tabHeightOrWidth)
                                                   : tabsArea.removeFromLeft (tabHeightOrWidth);

            if (i == selectedTab)
            {
                g.setColour (*colours[tabSelectedHighlight]);
                g.fillRect ((style == tabsVertical) ? tabArea.withWidth (3)
                                                    : tabArea.withHeight (3));
            }

            g.setColour (i == selectedTab ? *colours[tabSelectedText]
                                          : *colours[tabUnselectedText]);
            g.setFont (PLUME::font::plumeFontBook.withHeight (16.0f));
            g.drawText (tabs[i]->name,
                        (style == tabsVertical) ? tabArea.withTrimmedLeft (PLUME::UI::MARGIN + 5)
                                                : tabArea.withTrimmedTop (5),
                        (style == tabsVertical) ? Justification::centredLeft
                                                : Justification::centred,
                        true);

            (style == tabsVertical) ? tabsArea.removeFromTop (MARGIN)
                                    : tabsArea.removeFromLeft (MARGIN);
        }
    }
}

void TabbedPanelComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds();

    juce::Rectangle<int> panelArea, tabsArea;
    
    
    if (style == tabsVertical)
    {
        panelArea = area.removeFromRight (getWidth()*4/5).reduced (MARGIN, 2*MARGIN);
        tabsArea = area.reduced (0, 2*MARGIN);
    }
    else
    {
        tabsArea = area.removeFromTop (30).reduced (2*MARGIN, 0);
        panelArea = area.reduced (MARGIN, MARGIN);
    }

    if (!tabs.isEmpty())
    {
        //int tabHeight = (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size();
        int tabHeightOrWidth = (style == tabsVertical)
                                    ? jmin (30 , (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size())
                                    : (tabsArea.getWidth() - (tabs.size() - 1) * MARGIN)/tabs.size();

        for (auto* tab : tabs)
        {
            tab->button->setBounds ((style == tabsVertical) ? tabsArea.removeFromTop (tabHeightOrWidth)
                                                            : tabsArea.removeFromLeft (tabHeightOrWidth));

            (style == tabsVertical) ? tabsArea.removeFromTop (MARGIN)
                                    : tabsArea.removeFromLeft (MARGIN);
            

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


Component* TabbedPanelComponent::getComponentFromTab (const int tabNumber)
{
    if (tabNumber < 0 || tabNumber >= tabs.size()) return nullptr;

    return tabs[tabNumber]->panel;
}

Component* TabbedPanelComponent::getComponentFromTab (const String tabName)
{
    for (auto* tab : tabs)
    {
        if (tab->name == tabName)
        {
            return tab->panel;
        }
    }

	return nullptr;
}

Component* TabbedPanelComponent::getComponentFromSelectedTab()
{
    return getComponentFromTab (selectedTab);
}

//==============================================================================
void TabbedPanelComponent::setColour (TabbedPanelColourId id, Colour colourToSet)
{
	if (id == numColours) return;

    colours.set ((int) id, new Colour (colourToSet), true);
}

void TabbedPanelComponent::setStyle (TabbedPanelStyle newStyle)
{
    if (newStyle != style)
    {
        style = newStyle;
        resized();
        repaint();
    }
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