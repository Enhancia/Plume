/*
  ==============================================================================

    SubPanelComponent.cpp
    Created: 26 Feb 2019 11:24:31am
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "SubPanelComponent.h"

//==============================================================================
SubPanelComponent::SubPanelComponent ()
{
}

SubPanelComponent::~SubPanelComponent()
{
	removeAllChildren();
	rows.clear();
}

void SubPanelComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;

    auto textArea = getLocalBounds().removeFromLeft (getWidth()/4).reduced (MARGIN);

    if (!rows.isEmpty())
    {
		for (auto* row : rows)
		{
			g.setColour (row->isSeparator() ? Colour (0x50ffffff)
											: currentTheme.getColour (PLUME::colour::topPanelMainText));

    		g.setFont (PLUME::font::plumeFont.withHeight (12.0f));

    		if (row->type == Row::separator && row != rows.getFirst())
    		{
    			textArea.removeFromTop (MARGIN);  // Additional space between sections
    		}

    		g.drawText (row->name, 
                		textArea.removeFromTop (row->height),
                		//row->isSeparator() ? Justification::centredRight : Justification::centredLeft,
                		Justification::centredLeft,
                		false);

    		textArea.removeFromTop (MARGIN);
		}
    }
}

void SubPanelComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds();
    auto compArea = getLocalBounds().removeFromRight (getWidth()*3/4).reduced (MARGIN);

    if (!rows.isEmpty())
    {
		for (int i =0; i < rows.size(); i++)
        {
        	Row& row = *rows[i];

        	switch (row.type)
        	{
        		case Row::separator:
        			if (i != 0) compArea.removeFromTop (MARGIN); // Additional space between sections

        			getChildComponent (i)->setBounds (jmin (row.name.length()*7, compArea.getX()),
        											  compArea.getY(),
        											  getWidth() - jmin (row.name.length()*7, compArea.getX()),
        											  row.height);
        			compArea.removeFromTop (row.height);
        			break;

        		case Row::toggle:
        			getChildComponent (i)->setBounds (compArea.removeFromTop (row.height).withWidth(row.height));
        			break;

        		case Row::button:
        			getChildComponent (i)->setBounds (compArea.removeFromTop (row.height).withWidth(4*row.height));
        			break;

        		default:
        			getChildComponent (i)->setBounds (compArea.removeFromTop (row.height));
        	}
			
    		compArea.removeFromTop (MARGIN);
		}
    }
}


void SubPanelComponent::addRow (String rowText, Component* rowCompToAdd, int height)
{
	rows.add (new Row (rowCompToAdd, rowText, Row::custom, height));
	addAndMakeVisible (rowCompToAdd);
}

void SubPanelComponent::addSeparatorRow (String rowText)
{
	Separator* sep = new Separator();

	rows.add (new Row (sep, rowText, Row::separator));
	addAndMakeVisible (sep);
}

void SubPanelComponent::addToggleRow (String rowText, String buttonID)
{
	ToggleButton* tggle = new ToggleButton();
	tggle->setComponentID (buttonID);

	rows.add (new Row (tggle, rowText, Row::toggle));
	addAndMakeVisible (tggle);
	tggle->addListener(this);
}

void SubPanelComponent::addButtonRow (String rowText, String buttonID, String buttonText)
{
	TextButton* bttn = new TextButton (buttonID);
	bttn->setComponentID (buttonID);
	bttn->setButtonText (buttonText);

	rows.add (new Row (bttn, rowText, Row::button));
	addAndMakeVisible (bttn);
	bttn->addListener(this);
}

void SubPanelComponent::addLabelRow (String rowText, String labelID, String labelText)
{
	Label* lbl = new Label (labelID, labelText);
	lbl->setComponentID (labelID);
    lbl->setColour (Label::backgroundColourId, Colour (0x30000000));
    lbl->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    lbl->setEditable (true, false, false);
    lbl->setMouseCursor (MouseCursor (MouseCursor::IBeamCursor));

	rows.add (new Row (lbl, rowText, Row::label));
	addAndMakeVisible (lbl);
	lbl->addListener(this);
}