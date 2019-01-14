/*
  ==============================================================================

    PresetBoxModel.cpp
    Created: 10 Jan 2019 11:42:59am
    Author:  Alex

  ==============================================================================
*/

#include "PresetBoxModel.h"

PresetBoxModel::PresetBoxModel (PlumeProcessor& p)  : processor (p)
{
}

PresetBoxModel::~PresetBoxModel() {}

//==============================================================================
int PresetBoxModel::getNumRows()
{
    return processor.getPresetHandler().getNumPresets();
}

void PresetBoxModel::paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    using namespace PLUME;
    
    // Background
    g.setColour (rowIsSelected ? UI::currentTheme.getColour (colour::presetsBoxHighlightedBackground)
                               : UI::currentTheme.getColour (colour::presetsBoxBackground));
                               
    g.fillRect (1, 1, width-2, height-2);
    
    // Text
    g.setColour (rowIsSelected ? UI::currentTheme.getColour (colour::presetsBoxHighlightedText)
                               : UI::currentTheme.getColour (colour::presetsBoxStandartText));
                               
    g.setFont (Font (UI::font, float (height)/2, Font::plain));
    String text = processor.getPresetHandler().getTextForPresetId (rowNumber);
    
    g.drawText (text, PLUME::UI::MARGIN, 0, width, height,
                Justification::centredLeft, true);
                                      
}

void PresetBoxModel::listBoxItemDoubleClicked (int row, const MouseEvent& event)
{
    if (event.mods.isLeftButtonDown())
    {
        // Gets the preset Xml and loads it using the processor
        if (ScopedPointer<XmlElement> presetXml = processor.getPresetHandler().getPresetXmlToLoad (row))
        {
            MemoryBlock presetData;
            AudioProcessor::copyXmlToBinary (*presetXml, presetData);
            
            // Calls the plugin's setStateInformation method to load the preset
	        PLUME::UI::ANIMATE_UI_FLAG = false;
	        Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 100);

            processor.setStateInformation (presetData.getData(), presetData.getSize());
			presetXml->deleteAllChildElements(); // frees the memory
        }
	    else
        {
            processor.getPresetHandler().resetPreset ();
        }
    }
}