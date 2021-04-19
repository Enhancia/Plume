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

    g.setColour (currentTheme.getColour(PLUME::colour::topPanelBackground).brighter (0.03f));
    g.fillRect (panelArea);

    auto area = tabsArea;

    if (!tabs.isEmpty())
    {
        for (int i =0; i < tabs.size(); i++)
        {
            auto tabArea = tabs[i]->button->getBounds();

            if (i == selectedTab)
            {
                g.setColour (currentTheme.getColour(PLUME::colour::topPanelBackground).brighter (0.03f));
                g.fillRect (tabArea);

                g.setColour (currentTheme.getColour(PLUME::colour::topPanelMainText));
                g.fillRect ((style == tabsVertical) ? tabArea.withWidth (3)
                                                    : tabArea.withHeight (3));
            }

            g.setColour (i == selectedTab ? currentTheme.getColour(PLUME::colour::topPanelMainText)
                                          : currentTheme.getColour(PLUME::colour::topPanelSubText));
            
            g.setFont (PLUME::font::plumeFont.withHeight (16.0f));
            g.drawText (tabs[i]->name,
                        (style == tabsVertical) ? tabArea.withTrimmedLeft (MARGIN + 5)
                                                : tabArea.withTrimmedTop (5),
                        (style == tabsVertical) ? Justification::centredLeft
                                                : Justification::centred,
                        true);

            if (tabs[i]->alert)
            {
                auto alertArea = juce::Rectangle<int> (10, 10).withCentre ({tabArea.getRight() - 8 - PLUME::UI::MARGIN * 2,
                                                                            tabArea.getCentreY() + 2});
                
                g.setColour (Colour (0xff8090f0));
                g.fillEllipse (alertArea.toFloat());
            }
        }
    }
}

void TabbedPanelComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds();
    
    if (style == tabsVertical)
    {
        panelArea = area.removeFromRight (getWidth()*4/5);
        tabsArea = area;
    }
    else
    {
        tabsArea = area.removeFromTop (30);
        panelArea = area;
    }

    if (!tabs.isEmpty())
    {
        auto tabsAreaTemp = tabsArea;

        //int tabHeight = (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size();
        int tabHeightOrWidth = (style == tabsVertical)
                                    ? jmin (30 , (tabsArea.getHeight() - (tabs.size() - 1) * MARGIN)/tabs.size())
                                    : (tabsArea.getWidth() - (tabs.size() - 1) * MARGIN)/tabs.size();

        for (auto* tab : tabs)
        {
            tab->button->setBounds (tab == tabs.getLast() ? tabsAreaTemp
                                                          : (style == tabsVertical) ? tabsAreaTemp.removeFromTop (tabHeightOrWidth)
                                                                                    : tabsAreaTemp.removeFromLeft (tabHeightOrWidth));

            if (tab != tabs.getLast())
            {
                (style == tabsVertical) ? tabsAreaTemp.removeFromTop (MARGIN)
                                        : tabsAreaTemp.removeFromLeft (MARGIN);
            }

            tab->panel->setBounds (panelArea.reduced ((style == tabsVertical) ? 0 : MARGIN, 0));
        }
    }
}

//==============================================================================
void TabbedPanelComponent::addTab (Component* panel, String tabName)
{
    tabs.add (new Tab (panel, tabName));
    addAndMakeVisible (tabs.getLast()->button.get());
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

void TabbedPanelComponent::setTabAlert (const int tabNumber, const bool alertToSet)
{
    if (tabNumber < 0 || tabNumber > tabs.size()) return;

    tabs[tabNumber]->alert = alertToSet;
    repaint();
}

void TabbedPanelComponent::setTabAlert (const String tabName, const bool alertToSet)
{
    for (int i = 0; i < tabs.size(); i++)
    {
        if (tabs[i]->name == tabName)
        {
            setTabAlert (i, alertToSet);
            return;
        }
    }
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
        if (bttn == tabs[i]->button.get())
        {
            switchToTab(i);
        }
    }
}