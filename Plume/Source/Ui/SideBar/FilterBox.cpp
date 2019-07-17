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
}

FilterBox::~FilterBox()
{
}

void FilterBox::paint (Graphics& g)
{
    ListBox::paint (g);
    
    using namespace PLUME::UI;
    
    //Gradient for the box's inside
    auto gradIn = ColourGradient::vertical (Colour (0x30000000),
                                            0.0f, 
                                            Colour (0x25000000),
                                            float(getHeight()));
                                          
    gradIn.addColour (0.6, Colour (0x00000000));
    gradIn.addColour (0.8, Colour (0x20000000));
    
    g.setGradientFill (gradIn);
    g.fillRect (1, 1, getWidth()-2, getHeight()-2);
}

void FilterBox::paintOverChildren (Graphics& g)
{
    if (getOutlineThickness() > 0)
    {
        using namespace PLUME::UI;
    
        //Gradient for the box's outline
        auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                                   float(MARGIN), 
                                                   Colour (0x10ffffff),
                                                   float(getWidth() - MARGIN));
        gradOut.addColour (0.5, Colour (0x50ffffff));

        g.setGradientFill (gradOut);
        g.drawRect (getLocalBounds(), getOutlineThickness());
    }
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
                              
    g.fillRect (1, 1, width-2, height-2);
   
    // Text
    g.setColour (rowIsSelected ? getPlumeColour (presetsBoxRowTextHighlighted)
                               : getPlumeColour (presetsBoxRowText));
                               
    g.setFont (PLUME::font::plumeFont.withHeight (jmin (PLUME::font::SIDEBAR_LABEL_FONT_H, 
                                                        float (height*5)/6)));

    String text = rowNumber == 0 ? "All" : PlumePreset::getFilterTypeString (rowNumber - 1);

    g.drawText (text, PLUME::UI::MARGIN, 0, width, height,
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
            presetBox->repaint();
	    }
    }
}