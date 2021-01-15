/*
  ==============================================================================

    InfoPanel.cpp
    Created: 15 Feb 2019 12:19:08pm
    Author:  Alex

  ==============================================================================
*/

#include "InfoPanel.h"

//==============================================================================
InfoPanel::InfoPanel (Button& hideButton) : hideInfoButton (hideButton)
{
	setComponentID ("infoPanel");

    addAndMakeVisible (textEditor = new TextEditor ("Info Panel Text Editor"));
    textEditor->setMultiLine (true, true);
    textEditor->setReturnKeyStartsNewLine (true);
    textEditor->setReadOnly (true);
    textEditor->setScrollbarsShown (false);
    textEditor->setPopupMenuEnabled (false);
    textEditor->applyColourToAllText (PLUME::UI::currentTheme.getColour (PLUME::colour::sideBarMainText), true);
    textEditor->setJustification (Justification::left);
    textEditor->setFont (PLUME::font::plumeFontBook.withHeight (11.0f));
    textEditor->setMouseCursor (MouseCursor (MouseCursor::NormalCursor));
    textEditor->setInterceptsMouseClicks (false, false);
}

InfoPanel::~InfoPanel()
{
    textEditor = nullptr;
}

void InfoPanel::paint (Graphics& g)
{
	using namespace PLUME::UI;
    
    // Background
    g.setColour (getPlumeColour (infoPanelBackground));

    if (!hideInfoButton.getToggleState())
        g.fillRoundedRectangle (getLocalBounds().withTrimmedTop (MARGIN).toFloat(), 10.0f);
    else
        g.fillRoundedRectangle (getLocalBounds().withTrimmedLeft (10).toFloat(), 10.0f);


    // Info Icon
    g.setColour (getPlumeColour (infoPanelIcon));
    g.fillEllipse (getLocalBounds().withSize (30, 30).toFloat());

    Path infoPath = PLUME::path::createPath (PLUME::path::info);
    infoPath.scaleToFit (5, 5, 20, 20, true);

    g.setColour (getPlumeColour (infoPanelBackground));
    g.setFont (PLUME::font::plumeFontBold.withHeight (25.0f));
    g.fillPath (infoPath);

    // Version Text
    g.setColour (getPlumeColour (infoPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight (9.0f));
    g.drawText ("Plume " + String(JucePlugin_VersionString),
                getLocalBounds().reduced (MARGIN, MARGIN_SMALL),
                Justification::bottomRight, true);
}

void InfoPanel::resized()
{
    textEditor->setVisible (!hideInfoButton.getToggleState());

    if (textEditor->isVisible())
        textEditor->setBounds (getLocalBounds().withTrimmedTop (PLUME::UI::MARGIN*2)
                                               .withTrimmedLeft (PLUME::UI::MARGIN*2)
                                               .reduced (PLUME::UI::MARGIN));
}

void InfoPanel::actionListenerCallback (const String& alertMessage)
{
    alerted = true;
    textEditor->applyColourToAllText (Colour (0xfff03030), true);
    textEditor->setText (alertMessage, false);
}

void InfoPanel::mouseMove (const MouseEvent& event)
{
    if (auto* plumeComp = dynamic_cast<PlumeComponent*> (event.eventComponent))
    {
        if (plumeComp->hasInfoString())
        {
            if (alerted)
            {
                alerted = false;
                textEditor->applyColourToAllText (getPlumeColour (infoPanelMainText), true);
            }

            textEditor->setText (plumeComp->getInfoString(), false);
        }
    }
}