/*
  ==============================================================================

    PresetSearchBar.cpp
    Created: 12 Feb 2019 9:40:15am
    Author:  Alex

  ==============================================================================
*/

#include "PresetSearchBar.h"

//==============================================================================
PresetSearchBar::PresetSearchBar (PlumeProcessor& proc) : processor (proc), searchLabel (std::make_unique<Label> ("searchLabel", "Search..."))
{
    addAndMakeVisible (*searchLabel);
    searchLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    searchLabel->setColour (Label::textColourId, getPlumeColour (presetsBoxRowText));
    searchLabel->setFont (PLUME::font::plumeFont.withHeight (PLUME::font::SIDEBAR_LABEL_FONT_H));
    searchLabel->setColour (Label::outlineWhenEditingColourId, Colour (0x00000000));
    searchLabel->setEditable (true, false, false);
    searchLabel->setMouseCursor (MouseCursor (MouseCursor::IBeamCursor));
    searchLabel->addListener (this);
    
    addAndMakeVisible (*(cancelButton = std::make_unique<PlumeShapeButton> ("Cancel Button",
                                                            Colour (0x00000000),
                                                            getPlumeColour (sideBarButtonFill))));
	
	Path p;
	p.startNewSubPath (0, 0);
	p.lineTo (20, 20);
	p.startNewSubPath (0, 20);
	p.lineTo (20, 0);
	
	cancelButton->setShape (p, false, true, false);
    cancelButton->setBorderSize (BorderSize<int> (0));
    cancelButton->setVisible (false);
	cancelButton->addListener (this);
}

PresetSearchBar::~PresetSearchBar()
{
    searchLabel = nullptr;
    cancelButton = nullptr;
}

void PresetSearchBar::paint (Graphics& g)
{
    using namespace PLUME::UI;

    // background
    g.setColour (getPlumeColour (presetsSearchBarFill));
    g.fillRoundedRectangle (barArea.toFloat(),
                            barArea.getHeight()/2.0f);

    // magnifying glass draw
    Path magnifGlass = PLUME::path::createPath (PLUME::path::magnifyingGlass);

    auto magnifBounds = barArea.withX (MARGIN_SMALL).reduced (3).toFloat();

    magnifGlass.scaleToFit (magnifBounds.getX(),
                            magnifBounds.getY(),
                            magnifBounds.getHeight(),
                            magnifBounds.getHeight(),
                            true);

    g.setColour (getPlumeColour (presetsBoxRowText));
    g.fillPath (magnifGlass);
}

void PresetSearchBar::resized()
{
    using namespace PLUME::UI;
    barArea = getLocalBounds().withSizeKeepingCentre (getWidth(), jmin (getHeight()*3/4, 30));
    
    cancelButton->setBounds (barArea.withLeft (getRight() - 20 - MARGIN_SMALL).reduced (3));
    searchLabel->setBounds (barArea.withTrimmedLeft (20 + MARGIN_SMALL));
}


void PresetSearchBar::buttonClicked (Button*)
{
    if (searchLabel->getText() != "Search...")
    {
        processor.getPresetHandler().setNameSearchSetting ("");
		searchLabel->setText("Search...", dontSendNotification);

        // updates the interface
        if (getParentComponent() != nullptr)
	    {
		    if (auto* presetBox = dynamic_cast<ListBox*> (getParentComponent() // presetComp
			                                                ->findChildWithID("presetBox")))
		    {
			    presetBox->updateContent();
                presetBox->selectRow (0);
                presetBox->scrollToEnsureRowIsOnscreen (0);
		    }
	    }

        cancelButton->setVisible (false);
    }
}

void PresetSearchBar::labelTextChanged (Label* lbl)
{
    // sets default test to "Search..."
    if (lbl->getText().isEmpty())
    {
        lbl->setText ("Search...", dontSendNotification);
        cancelButton->setVisible (false);
    }
}


void PresetSearchBar::editorShown (Label* lbl, TextEditor& ed)
{
    if (lbl->getText() == "Search...")
    {
        // The user doesn't have to manually remove "Search..."
        ed.setText ("", false);
    }
    
    ed.addListener (this);
}

void PresetSearchBar::editorHidden (Label*, TextEditor& ed)
{
    ed.removeListener (this);
}

void PresetSearchBar::textEditorTextChanged (TextEditor&)
{
    if (!searchLabel->getText (true).isEmpty())
    {
        cancelButton->setVisible (true);
    }

    processor.getPresetHandler().setNameSearchSetting (searchLabel->getText (true));
    
    // updates the interface
    if (getParentComponent() != nullptr)
	{
		if (auto* presetBox = dynamic_cast<ListBox*> (getParentComponent() // presetComp
			                                              ->findChildWithID("presetBox")))
		{
			presetBox->updateContent();
            presetBox->selectRow (0);
            presetBox->scrollToEnsureRowIsOnscreen (0);
		}
	}
}