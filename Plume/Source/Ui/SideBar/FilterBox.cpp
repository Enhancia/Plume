/*
  ==============================================================================

    FilterBox.cpp
    Created: 7 Feb 2019 5:39:29pm
    Author:  Alex

  ==============================================================================
*/

#include "FilterBox.h"

FilterBox::FilterBox (const String& componentName, PlumeProcessor& p)  : ListBox (componentName, this),
                                                                         processor (p)
{
    //sets own colours
    setColour (ListBox::backgroundColourId, Colour (0x00000000));
    setColour (ListBox::outlineColourId, Colour (0x00000000));
    setOutlineThickness (1);
    setRowHeight (16);
    
    auto& scrollBar = getVerticalScrollBar();
    scrollBar.setColour (ScrollBar::thumbColourId, getPlumeColour (presetsBoxScrollBar));
    getViewport()->setScrollBarThickness (8);
}

FilterBox::~FilterBox()
{
}

void FilterBox::paint (Graphics& g)
{
    if (getViewport()->canScrollVertically())
    {
        g.excludeClipRegion (juce::Rectangle<int> (getViewport()->getScrollBarThickness()/2 - 1, getHeight())
                                .withX (getWidth() - getViewport()->getScrollBarThickness()));
    }

    g.setColour (getPlumeColour (presetsBoxBackground));
    g.fillRect (getLocalBounds().reduced (1, 0));
}

void FilterBox::paintOverChildren (Graphics& g)
{
}

//==============================================================================
int FilterBox::getNumRows()
{
    return PlumePreset::numFilters + 1;
}

void FilterBox::paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    // Writes the each Filter type
    using namespace PLUME;
    
    // Background
    g.setColour (rowIsSelected ? getPlumeColour (presetsBoxRowBackgroundHighlighted)
                               : getPlumeColour (presetsBoxRowBackground));
                              
    g.fillRoundedRectangle (float (UI::MARGIN_SMALL), 1.0f,
                            float (width - 2*UI::MARGIN_SMALL),
                            height - 2.0f,
                            height/2.0f);

    // Text
    g.setColour (rowIsSelected ? getPlumeColour (presetsBoxRowTextHighlighted)
                               : getPlumeColour (presetsBoxRowText));
                               
    g.setFont (PLUME::font::plumeFont.withHeight (jmin (PLUME::font::SIDEBAR_LABEL_FONT_H, 
                                                        float (height*5)/6)));

    String text = rowNumber == 0 ? "All" : PlumePreset::getFilterTypeString (rowNumber - 1);

    g.drawText (text, PLUME::UI::MARGIN*2, 0, width, height,
                Justification::centredLeft, true);
}

void FilterBox::listBoxItemClicked (int row, const MouseEvent& event)
{
    if (event.mods.isLeftButtonDown())
    {
        processor.getPresetHandler().setFilterSearchSetting (row - 1);
        
        if (auto* presetBox = dynamic_cast<ListBox*> (getParentComponent() // presetComp
                                                          ->findChildWithID ("presetBox"))) 
	    {
		    presetBox->updateContent();
            presetBox->selectRow (0);
            presetBox->repaint();
	    }
    }
}