/*
  ==============================================================================

    GeneralOptionsSubPanel.cpp
    Created: 25 Feb 2019 11:30:56am
    Author:  Alex

  ==============================================================================
*/

#include "GeneralOptionsSubPanel.h"

//==============================================================================
AboutPanel::AboutPanel()
{
    contactComponent.reset (new ContactSubPanel());
    addAndMakeVisible (*contactComponent);
}

AboutPanel::~AboutPanel()
{
}

void AboutPanel::paint (Graphics& g)
{
    g.setFont (PLUME::font::plumeFont.withHeight (17));
    g.setColour (getPlumeColour(topPanelSubText));

    String osString;
    #if JUCE_WINDOWS
    osString = "Win x64";
    #elif JUCE_MAC
    osString = "MacOS";
    #endif

    g.drawText ("Plume " + JUCEApplication::getInstance()->getApplicationVersion()
                                            + " " + osString
                                            + "  |  Copyright 2021 Enhancia, inc.",
                      aboutArea,
                      Justification::centred);
}

void AboutPanel::resized()
{
    auto area = getLocalBounds();

    aboutArea = area.removeFromBottom (area.getHeight()/5);
    contactArea = area;
    
    contactComponent->setBounds (contactArea);
}

ContactSubPanel::ContactSubPanel()
{
    addSeparatorRow ("Contact");
    addButtonRow ("Contact Enhancia", "cnt", "Contact");
    addButtonRow ("Send Bug Report", "sbr", "Send");
}

ContactSubPanel::~ContactSubPanel()
{
}

void ContactSubPanel::buttonClicked (Button* bttn)
{
    if (bttn->getComponentID() == "cnt")
    {
        URL ("https://www.enhancia-music.com/contact/").launchInDefaultBrowser();
    }

    else if (bttn->getComponentID() == "sbr")
    {
        // Send bug report
        if (auto bugReportPanel = getParentComponent() // AboutPanel
                                    ->getParentComponent() // Tabbed Options
                                    ->getParentComponent() // Options Panel
                                    ->getParentComponent() // Plume Editor
                                    ->findChildWithID ("BugReportPanel"))
        {
            bugReportPanel->setVisible (true);
        }
    }
}