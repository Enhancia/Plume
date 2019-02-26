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
		g.setColour (currentTheme.getColour (colour::topPanelMainText));

		for (auto* row : rows)
		{
    		g.setFont (PLUME::font::plumeFont.withHeight (12.0f));
    
    		g.drawText (row->name, 
                		textArea.removeFromTop (rowHeight),
                		Justification::centredLeft, false);

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
			getChildComponent (i)->setBounds (compArea.removeFromTop (rowHeight));

    		compArea.removeFromTop (MARGIN);
		}
    }
}

void SubPanelComponent::addRow (String name, Component* rowCompToAdd)
{
	rows.add (new Row (rowCompToAdd, name));
	addAndMakeVisible (rowCompToAdd);
}