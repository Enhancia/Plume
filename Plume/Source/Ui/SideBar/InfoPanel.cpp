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
    auto gradOut = ColourGradient::horizontal (currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               float(MARGIN), 
                                               currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               float(getWidth() - MARGIN));
    gradOut.addColour (0.5, currentTheme.getColour(PLUME::colour::sideBarSeparatorIn));

    g.setGradientFill (gradOut);

    
    Path p;
    p.startNewSubPath (getWidth()/2.0f - float(MARGIN), 0.0f);
    p.lineTo (0.0f, 0.0f);
    p.lineTo (0.0f, float(getHeight()));
    p.lineTo (float(getWidth()), float(getHeight()));
    p.lineTo (float(getWidth()), 0.0f);
    p.lineTo (getWidth()/2.0f + float(MARGIN), 0.0f);
    g.strokePath (p, PathStrokeType (1.0f));

    g.setColour (currentTheme.getColour (PLUME::colour::sideBarMainText));
    g.setFont (PLUME::font::plumeFont.withHeight (9.0f));

    /*
    g.drawText (displayText, getLocalBounds().reduced (MARGIN),
                Justification::centred, true);
                */
}

void InfoPanel::resized()
{
    textEditor->setBounds (getLocalBounds().reduced (PLUME::UI::MARGIN, 0));
}

void InfoPanel::actionListenerCallback (const String&)
{
}

void InfoPanel::mouseMove (const MouseEvent& event)
{
    if (auto* plumeComp = dynamic_cast<PlumeComponent*> (event.eventComponent))
    {
        textEditor->setText (plumeComp->getInfoString(), false);
    }
}