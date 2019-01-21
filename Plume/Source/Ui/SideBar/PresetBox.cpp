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
    editLabel = new Label ("editLabel", "NewPreset");
    editLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    editLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::presetsBoxStandartText));
    editLabel->setFont (Font (UI::font, float (getRowHeight())/2, Font::plain));
    editLabel->setInterceptsMouseClicks (false, false);
    editLabel->addListener (this);
    
    auto& scrollBar = getVerticalScrollBar();
    scrollBar.setColour (ScrollBar::thumbColourId, UI::currentTheme.getColour (colour::presetsBoxScrollBar));
}

PresetBox::~PresetBox()
{
    editLabel = nullptr;
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
        editLabel->setVisible (true);
        return editLabel;
    }
    else if (existingComponentToUpdate == editLabel)
    {
        editLabel->setVisible (false);
        return nullptr;
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
	    else // failed to get preset xml somehow
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

void PresetBox::deleteKeyPressed (int lastRowSelected)
{
    if (processor.getPresetHandler().deletePresetForId (lastRowSelected))
    {
        updateContent();
        if (auto* hdr = dynamic_cast<PlumeComponent*> (  getParentComponent() // presetComp
                                                       ->getParentComponent() // sideBarComp
                                                       ->getParentComponent() // editor
                                                       ->findChildWithID ("header")))
        {
            hdr->update();
        }
    }
}

//==============================================================================
void PresetBox::labelTextChanged (Label* lbl)
{
    String presetName = editLabel->getText().replaceCharacter (' ', '_');
    DBG ("Label callback ( text: " << presetName << " )");
    
    if (XmlElement::isValidXmlName (presetName))
    {
        if (newPresetEntry)
        {
            createUserPreset (presetName);
        }
        else
        {
            renamePreset (presetName);
        }
    }
    
    // Resets the box to non entry
    newPresetEntry = false;
    presetIdToEdit = -1;
    updateContent();
}

void PresetBox::editorHidden (Label* lbl, TextEditor& ted)
{
    DBG ("Text Editor callback");
    
    if (newPresetEntry)
    {
        if (editLabel->getText() == "NewPreset")
        {
            // Resets the box to non entry
            newPresetEntry = false;
            presetIdToEdit = -1;
            updateContent();
        }
    }
    else // Preset being renamed
    {
        if (editLabel->getText() == processor.getPresetHandler().getCurrentPreset())
        {
            // Resets the box to non entry
            presetIdToEdit = -1;
            updateContent();
        }
    }
}

//==============================================================================
void PresetBox::startNewPresetEntry()
{
    // Adds a new row at the end to edit the preset name
    newPresetEntry = true;
    presetIdToEdit = processor.getPresetHandler().getNumPresets();
    editLabel->setText ("NewPreset", dontSendNotification);
    updateContent();
    scrollToEnsureRowIsOnscreen (presetIdToEdit);
    
    if (getComponentForRowNumber (presetIdToEdit) == editLabel)
    {
        // Gives the focus to the label and waits for the callback
        editLabel->showEditor();
    }
    else
    {
        // Aborts the changes
        newPresetEntry = false;
        presetIdToEdit = -1;
        updateContent();
    }
}

void PresetBox::startRenameEntry()
{
    // TODO put right id to edit
}

void PresetBox::createUserPreset (const String& presetName)
{
    ScopedPointer<XmlElement> presetXml = new XmlElement (presetName);
	processor.createPluginXml (*presetXml);
	processor.createGestureXml (*presetXml);
	
    processor.getPresetHandler().createNewUserPreset (presetName, *presetXml);
    
    presetXml->deleteAllChildElements();
}

void PresetBox::renamePreset (const String& newName)
{
    // TODO faire methode pour renommer preset (aussi dans preset handler)
}