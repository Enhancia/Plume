/*
  ==============================================================================

    InfoPanel.cpp
    Created: 15 Feb 2019 12:19:08pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "InfoPanel.h"

//==============================================================================
InfoPanel::InfoPanel()
{
	setComponentID ("infoPanel");

    addAndMakeVisible (textEditor = new TextEditor ("Info Panel Text Editor"));
    textEditor->setMultiLine (true, true);
    textEditor->setReturnKeyStartsNewLine (true);
    textEditor->setReadOnly (true);
    textEditor->setScrollbarsShown (true);
    textEditor->setPopupMenuEnabled (false);
    textEditor->applyColourToAllText (PLUME::UI::currentTheme.getColour (PLUME::colour::sideBarMainText), true);
    textEditor->setJustification (Justification::left);
    textEditor->setFont (PLUME::font::plumeFontBook.withHeight (12.0f));
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
    
    g.fillAll (Colour (0x05000000));

    //Gradient for the box's outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               float(MARGIN), 
                                               Colour (0x10ffffff),
                                               float(getWidth() - MARGIN));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);

    
    Path p;
    p.startNewSubPath (getWidth()/2.0f - float(2*MARGIN), 0.0f);
    p.lineTo (0.0f, 0.0f);
    p.lineTo (0.0f, float(getHeight()));
    p.lineTo (float(getWidth()), float(getHeight()));
    p.lineTo (float(getWidth()), 0.0f);
    p.lineTo (getWidth()/2.0f + float(2*MARGIN), 0.0f);
    g.strokePath (p, PathStrokeType (1.0f));

    g.setColour (getPlumeColour (sideBarMainText));
    g.setFont (PLUME::font::plumeFont.withHeight (9.0f));
}

void InfoPanel::resized()
{
    textEditor->setBounds (getLocalBounds().reduced (PLUME::UI::MARGIN, 0));
}

void InfoPanel::actionListenerCallback (const String& alertMessage)
{
    alerted = true;
    textEditor->applyColourToAllText (Colour (0xffe03030), true);
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
                textEditor->applyColourToAllText (getPlumeColour (sideBarMainText), true);
            }

            textEditor->setText (plumeComp->getInfoString(), false);
        }
    }
}