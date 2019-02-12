/*
  ==============================================================================

    PresetSearchBar.cpp
    Created: 12 Feb 2019 9:40:15am
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "PresetSearchBar.h"

//==============================================================================
PresetSearchBar::PresetSearchBar (PlumeProcessor& proc) : processor (proc)
{
    addAndMakeVisible (searchLabel = new Label ("searchLabel", "Search..."));
    searchLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    searchLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::presetsBoxStandartText)
                                                                 .withAlpha (0.6f));
    searchLabel->setFont (Font (PLUME::UI::font, 10.0f, Font::plain));
    searchLabel->setEditable (true, false, false);
    searchLabel->addListener (this);
    
    addAndMakeVisible (cancelButton = new ShapeButton ("Cancel Button",
                                                        Colour (0x00000000),
                                                        Colour (0x00000000),
                                                        Colour (0x00000000)));
	
	Path p;
	p.startNewSubPath (0, 0);
	p.lineTo (20, 20);
	p.startNewSubPath (0, 20);
	p.lineTo (20, 0);
	
	cancelButton->setShape (p, false, true, false);
	cancelButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill)
                                                     .withAlpha (0.6f), 1.0f);
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
    
    /*
    //Gradient for the bar's inside
    auto gradIn = ColourGradient::vertical (Colour (0x30000000),
                                            0, 
                                            Colour (0x25000000),
                                            getHeight());
                                          
    gradIn.addColour (0.6, Colour (0x00000000));
    gradIn.addColour (0.8, Colour (0x20000000));
    
    g.setGradientFill (gradIn);
    g.fillRect (0, 0, getWidth(), jmin (getHeight(), 30));
    */
    
    //Gradient for the bar's outline
    auto gradOut = ColourGradient::horizontal (currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               MARGIN, 
                                               currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               getWidth() - MARGIN);
    gradOut.addColour (0.5, currentTheme.getColour(PLUME::colour::sideBarSeparatorIn));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (0, 0, getWidth(), jmin (getHeight(), 30), 10.0f, 1.0f);
}

void PresetSearchBar::resized()
{
    using namespace PLUME::UI;
    auto area = getLocalBounds().withHeight (jmin (getHeight(), 30));
    
    cancelButton->setBounds (area.removeFromRight (20 + MARGIN).withTrimmedRight (MARGIN).reduced (6));
    searchLabel->setBounds (area.withTrimmedLeft (MARGIN));
}


void PresetSearchBar::buttonClicked (Button* bttn)
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
		    }
	    }
    }
}

void PresetSearchBar::labelTextChanged (Label* lbl)
{
    // sets default test to "Search..."
    if (lbl->getText().isEmpty())
    {
        lbl->setText ("Search...", dontSendNotification);
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

void PresetSearchBar::editorHidden (Label* lbl, TextEditor& ed)
{
    ed.removeListener (this);
}

void PresetSearchBar::textEditorTextChanged (TextEditor& ed)
{
    processor.getPresetHandler().setNameSearchSetting (searchLabel->getText (true));
    
    // updates the interface
    if (getParentComponent() != nullptr)
	{
		if (auto* presetBox = dynamic_cast<ListBox*> (getParentComponent() // presetComp
			                                              ->findChildWithID("presetBox")))
		{
			presetBox->updateContent();
		}
	}
}