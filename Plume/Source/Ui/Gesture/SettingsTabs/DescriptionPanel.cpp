/*
  ==============================================================================

    DescriptionPanel.cpp
    Created: 20 May 2019 11:19:36am
    Author:  Alex

  ==============================================================================
*/

#include "DescriptionPanel.h"

//==============================================================================
DescriptionPanel::DescriptionPanel (Gesture& g)	: gesture (g)
{
	setComponentID ("descriptionPanel");

    addAndMakeVisible (*(descriptionLabel = std::make_unique<Label> ("Description Panel Text Editor")));
    descriptionLabel->setColour (Label::textColourId, Colours::black);
    descriptionLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    descriptionLabel->setColour (Label::outlineColourId, Colour (0x20000000));
    descriptionLabel->setColour (Label::textWhenEditingColourId, Colour (0xe0000000));
    descriptionLabel->setColour (Label::backgroundWhenEditingColourId, Colour (0x00000000));
    descriptionLabel->setColour (Label::outlineWhenEditingColourId, Colour (0x20000000));

    descriptionLabel->setJustificationType (Justification::left);
    descriptionLabel->setEditable (true, false, false);
    descriptionLabel->setFont (PLUME::font::plumeFontBook.withHeight (12.0f));
    descriptionLabel->setMouseCursor (MouseCursor (MouseCursor::IBeamCursor));

	descriptionLabel->addListener (this);

	if (gesture.getDescription().isEmpty())
	{
		resetEditorToDefault();
	}
	else
	{
		descriptionLabel->setText (gesture.getDescription(), sendNotification);
	}
}

DescriptionPanel::~DescriptionPanel()
{
    descriptionLabel = nullptr;
}

const String DescriptionPanel::getInfoString()
{
    const String bullet = " " + String::charToString (juce_wchar(0x2022));
    
	return "Description Editor:\n\n"
           + bullet + " Use this area to write a short description for the gesture.";
}

void DescriptionPanel::update()
{
	descriptionLabel->setText (gesture.getDescription(), sendNotification);
}
    
void DescriptionPanel::paint (Graphics& g)
{
}

void DescriptionPanel::resized()
{
    descriptionLabel->setBounds (getLocalBounds().reduced (PLUME::UI::MARGIN, 0));
}


void DescriptionPanel::labelTextChanged (Label* lbl)
{
	gesture.setDescription (lbl->getText());

	if (lbl->getText().isEmpty())
	{
		resetEditorToDefault();
	}
	else
	{
		descriptionLabel->setColour (Label::textColourId, Colour (0xff000000));
	}
}

void DescriptionPanel::editorShown (Label* lbl, TextEditor& ed)
{
	ed.setColour (TextEditor::highlightedTextColourId, Colour (0x80000000));
	ed.setColour (TextEditor::highlightColourId, Colour (0x10000000));
	ed.setColour (CaretComponent::caretColourId, Colour (0x80000000));
	ed.setInputRestrictions (90);
	
    if (lbl->getText() == "Add Description...")
    {
        // The user doesn't have to manually remove "Preset Name :"
        ed.setText ("", false);
    }
}

void DescriptionPanel::resetEditorToDefault()
{
	descriptionLabel->setText ("Add Description...", dontSendNotification);
	descriptionLabel->setColour (Label::textColourId, Colour (0x80000000));
}