/*
  ==============================================================================

    PresetBox.cpp
    Created: 10 Jan 2019 11:42:59am
    Author:  Alex

  ==============================================================================
*/

#include "PresetBox.h"

PresetBox::PresetBox (const String& componentName, PlumeProcessor& p)  : ListBox (componentName, this),
                                                                         processor (p)
{
    /*
    editLabel = new Label ("editLabel", "NewPreset");
    editLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    editLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::presetsBoxStandartText));
    editLabel->setFont (Font (UI::font, float (getRowHeight())/2, Font::plain));
    */
    
    auto& scrollBar = getVerticalScrollBar();
    scrollBar.setColour (ScrollBar::thumbColourId, UI::currentTheme.getColour (colour::presetsBoxScrollBar));
}

PresetBox::~PresetBox()
{
}

//==============================================================================
int PresetBox::getNumRows()
{
    return processor.getPresetHandler().getNumPresets() + newPresetEntry;
}

void PresetBox::paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    
    // Writes the preset name except for the presetIdToEdit row number (which has an editable label)
    if (rowNumber != presetIdToEdit)
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
}

Component* PresetBox::refreshComponentForRow (int rowNumber,
                                              bool isRowSelected,
                                              Component* existingComponentToUpdate)
{
    // Creates a Label object only for the the presetIdToEdit row number
    if (rowNumber == presetIdToEdit)
    {
        if (existingComponentToUpdate == nullptr)
        {
            lbl->setText ("NewPreset", dontSendNotification);
            lbl->setColour (Label::backgroundColourId, Colour (0x00000000));
            lbl->setColour (Label::textColourId, UI::currentTheme.getColour (colour::presetsBoxStandartText));
            lbl->setFont (Font (UI::font, float (getRowHeight())/2, Font::plain));
            
            return lbl;
        }
        else if (auto* lbl = dynamic_cast<Label*> (existingComponentToUpdate))
        {
            lbl->setText ("NewPreset", dontSendNotification);
            
            return lbl;
        }
    }
    else if (existingComponentToUpdate != nullptr)
    {
        existingComponentToUpdate = nullptr;
    }

	return nullptr;
}

void PresetBox::listBoxItemDoubleClicked (int row, const MouseEvent& event)
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
            processor.getPresetHandler().resetPreset();
        }
    }
}

void PresetBox::backgroundClicked (const MouseEvent& event)
{
    if (event.mods.isLeftButtonDown())
    {
        deselectAllRows();
    }
}

//==============================================================================
void PresetBox::labelTextChanged (Label* lbl)
{
    DBG ("LABEL CALLBACK");
    lbl->removeListener (this);
    String presetName = lbl->getText().replaceCharacter (' ', '_');
    
    if (presetName != "NewPreset")
    {
        // Creates the preset using the processor and PresetHandler
        ScopedPointer<XmlElement> presetXml = new XmlElement (presetName);
	    processor.createPluginXml (*presetXml);
	    processor.createGestureXml (*presetXml);
	
        processor.getPresetHandler().createNewUserPreset (presetName, *presetXml);
    
        presetXml->deleteAllChildElements();
    }
    
    // Resets the box to non entry
	//lbl = nullptr;
    newPresetEntry = false;
    presetIdToEdit = -1;
    updateContent();
}

//==============================================================================
void PresetBox::startNewPresetEntry()
{
    // Adds a new row at the end to edit the preset name
    newPresetEntry = true;
    presetIdToEdit = processor.getPresetHandler().getNumPresets();
    updateContent();
    scrollToEnsureRowIsOnscreen (presetIdToEdit);
    
    if (auto* lbl = dynamic_cast<Label*> (getComponentForRowNumber (presetIdToEdit)))
    {
        // Gives the focus to the label and waits for the callback
        lbl->addListener (this);
        lbl->showEditor();
    }
    else
    {
        // Aborts the changes
        newPresetEntry = false;
        presetIdToEdit = -1;
        updateContent();
    }
}