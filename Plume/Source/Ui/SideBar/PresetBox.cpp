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
    setComponentID ("presetBox");
    
    //sets own colours
    setColour (ListBox::backgroundColourId, Colour (0x00000000));
    setColour (ListBox::outlineColourId, Colour (0x00000000));
	setOutlineThickness (1);
    setRowHeight (16);
    
    // Sub components
    editLabel.reset (new Label ("editLabel", "NewPreset"));
    editLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    editLabel->setColour (Label::textColourId, getPlumeColour (presetsBoxRowText));
    editLabel->setFont (PLUME::font::plumeFont.withHeight (float (getRowHeight())/2));
    editLabel->setInterceptsMouseClicks (false, false);
    editLabel->addListener (this);
    
    auto& scrollBar = getVerticalScrollBar();
    scrollBar.setColour (ScrollBar::thumbColourId, getPlumeColour (presetsBoxScrollBar));
    getViewport()->setScrollBarThickness (8);
}

PresetBox::~PresetBox()
{
    editLabel = nullptr;
}

void PresetBox::paint (Graphics& g)
{
    if (getViewport()->canScrollVertically())
    {
        g.excludeClipRegion (juce::Rectangle<int> (getViewport()->getScrollBarThickness()/2 - 1, getHeight())
                                .withX (getWidth() - getViewport()->getScrollBarThickness()));
    }

    g.setColour (getPlumeColour (presetsBoxBackground));
    g.fillRect (getLocalBounds().reduced (1, 0));
}

void PresetBox::paintOverChildren (Graphics&)
{
}

//==============================================================================
void PresetBox::update()
{
    const int currentPresetId = processor.getPresetHandler().getCurrentPresetIdInSearchList();
    selectRow (currentPresetId == -1 ? 0 : currentPresetId);
}

const String PresetBox::getInfoString()
{
    if (getSelectedRow() == -1)
    {
        return "Preset List :\n\n"
            "- Use your left click and arrows to navigate between presets.\n"
            "- Press Enter or delete to load or delete the currently selected preset.\n";
    }
    else
    {    
        return getTooltipForRow (getSelectedRow());
    }
}

//==============================================================================
int PresetBox::getNumRows()
{
    return processor.getPresetHandler().getNumSearchedPresets() + newPresetEntry;
}

void PresetBox::paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected)
{
    // Writes the preset name except for the presetIdToEdit row number (which has an editable label)
    if (rowNumber != presetIdToEdit)
    {
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

        String text = processor.getPresetHandler().getTextForPresetId (rowNumber);
    
        g.drawText (text, PLUME::UI::MARGIN*2, 0, width, height,
                    Justification::centredLeft, true);
    }
}

Component* PresetBox::refreshComponentForRow (int rowNumber,
                                              bool,
                                              Component* existingComponentToUpdate)
{
    // Creates a Label object only for the the presetIdToEdit row number
    if (rowNumber == presetIdToEdit)
    {
        editLabel->setVisible (true);
        return editLabel.get();
    }
    else if (existingComponentToUpdate == editLabel.get())
    {
        editLabel->setVisible (false);
        return nullptr;
    }

	return nullptr;
}

void PresetBox::listBoxItemClicked (int row, const MouseEvent& event)
{
    if (event.mods.isPopupMenu())
    {
        rightClickMenu.clear();
		bool isUser = processor.getPresetHandler().isUserPreset (row);

        rightClickMenu.addItem (1, "Rename", isUser);
        rightClickMenu.addItem (2, "Show In Explorer", isUser);
        rightClickMenu.addItem (3, "Delete", isUser);
        
        handleMenuResult (row,
                          rightClickMenu.showMenu (PopupMenu::Options().withParentComponent (  getParentComponent()
                                                                                             ->getParentComponent()
                                                                                             ->getParentComponent())
                                                                       .withMaximumNumColumns (3)
                                                                       .withPreferredPopupDirection (PopupMenu::
																		                             Options::
																		                             PopupDirection::
																		                             downwards)
                                                                       .withStandardItemHeight (15)));
    }
}

void PresetBox::listBoxItemDoubleClicked (int row, const MouseEvent& event)
{
    if (event.mods.isLeftButtonDown())
    {
        setPreset (row);
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
    ignoreUnused(lastRowSelected);
    //deletePreset (lastRowSelected);
}

void PresetBox::returnKeyPressed (int lastRowSelected)
{
    setPreset (lastRowSelected);
}

void PresetBox::selectedRowsChanged (int lastRowSelected)
{
    ignoreUnused (lastRowSelected);
    if (auto* infoText = dynamic_cast<TextEditor*> (getParentComponent() // presetComp
                                                    ->getParentComponent() // sideBarComp
                                                    ->findChildWithID("infoPanel")
                                                    ->findChildWithID("infoText")))
    {
        infoText->setText (getInfoString(), false);
    }
}

String PresetBox::getTooltipForRow (int row)
{
    return processor.getPresetHandler().getDescriptionForPresetId (row);
}

//==============================================================================
void PresetBox::labelTextChanged (Label*)
{
    String presetName = editLabel->getText();
    
    //if (XmlElement::isValidXmlName (presetName.replace (" ", "_")))
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

void PresetBox::editorHidden (Label*, TextEditor&)
{
    // This callback is triggered when user finishes entering the text
    // It will trigger regardless of the text changing or not.
    // Here, cancels the new preset / rename entry if the text wasn't changed.
    
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
        if (editLabel->getText() == processor.getPresetHandler().getTextForPresetId (getSelectedRow()))
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
    presetIdToEdit = processor.getPresetHandler().getNumSearchedPresets();
    editLabel->setText ("NewPreset", dontSendNotification);
    updateContent();
    scrollToEnsureRowIsOnscreen (presetIdToEdit);
    
    if (getComponentForRowNumber (presetIdToEdit) == editLabel.get())
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

void PresetBox::startRenameEntry (const int row)
{
    // Puts right id and text to edit
    newPresetEntry = false;
    presetIdToEdit = row;
    editLabel->setText (processor.getPresetHandler().getTextForPresetId (row), dontSendNotification);
    updateContent();
    
    if (getComponentForRowNumber (presetIdToEdit) == editLabel.get())
    {
        // Gives the focus to the label and waits for the callback
        editLabel->showEditor();
    }
    else
    {
        // Aborts the changes
        presetIdToEdit = -1;
        updateContent();
    }
}

void PresetBox::deletePreset (const int row)
{
    //TODO fenêtre d'avertissement avant suppression
    
    bool shouldUpdateHeader = (processor.getPresetHandler().getCurrentPresetName()
                            == processor.getPresetHandler().getTextForPresetId (row));
    
    PLUME::log::writeToLog ("Deleting preset : " + processor.getPresetHandler().getPresetForId (row).getName(), PLUME::log::presets);
	
    if (processor.getPresetHandler().deletePresetForId(row))
	{
		updateContent();

		// Updates the header in case the selected preset was deleted
		if (shouldUpdateHeader)
		{
			updateHeader();
	    }
    }
}
//==============================================================================
void PresetBox::handleMenuResult (const int row, const int menuResult)
{
    switch (menuResult)
    {
        case 0: // No choice
            break;
            
        case 1: // Rename preset
            if (processor.getPresetHandler().isUserPreset (row))
            {
                startRenameEntry (row);
            }
            break;
            
        case 2: // Show in explorer
            processor.getPresetHandler().showPresetInExplorer (row);
            break;

        case 3: // Delete preset
            deletePreset (row);
            
    }
    
    repaint();
}

void PresetBox::setPreset (const int row)
{
    // Gets the preset Xml and loads it using the processor
    if (std::unique_ptr<XmlElement> presetXml = processor.getPresetHandler().getPresetXmlToLoad (row))
    {
        PLUME::log::writeToLog ("Loading preset (from preset list) : " + processor.getPresetHandler().getPresetForId (row).getName(), PLUME::log::presets);

        MemoryBlock presetData;
        AudioProcessor::copyXmlToBinary (*presetXml, presetData);
            
	    prepareGesturePanelToPresetChange();
        processor.getWrapper().removeAllChangeListeners();
        //Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 10);

        // Calls the plugin's setStateInformation method to load the preset
        processor.setStateInformation (presetData.getData(), int (presetData.getSize()));
        presetXml->deleteAllChildElements(); // frees the memory
    }
    else // failed to get preset xml somehow
    {
        processor.getPresetHandler().resetPreset();
    }
}

void PresetBox::createUserPreset (const String& presetName)
{
    ignoreUnused(presetName);

    auto presetXml = std::make_unique<XmlElement> ("PLUME");
	processor.createPluginXml (*presetXml);
	processor.createGestureXml (*presetXml);
	
    processor.getPresetHandler().createNewUserPreset (*presetXml);
    updateHeader();
    
    presetXml->deleteAllChildElements();
}

void PresetBox::renamePreset (const String& newName)
{
    const int currentPresetId = processor.getPresetHandler().getCurrentPresetIdInSearchList();

    PLUME::log::writeToLog ("Renaming preset : " + processor.getPresetHandler().getPresetForId (presetIdToEdit).getName()
                                                 + " => " + newName, PLUME::log::presets);

    processor.getPresetHandler().renamePreset (newName, presetIdToEdit);
    
    if (presetIdToEdit == currentPresetId) setPreset (currentPresetId);

    repaint();
}

void PresetBox::prepareGesturePanelToPresetChange()
{
    if (GesturePanel* gesturePanel = dynamic_cast<GesturePanel*> ( getParentComponent() // presetComp
                                                         ->getParentComponent() // sideBarComp
                                                         ->getParentComponent() // editor
                                                         ->findChildWithID("gesturePanel")))
    {
		gesturePanel->stopTimer();
        gesturePanel->removeListenerForAllParameters();
    }
}

void PresetBox::updateHeader()
{
    if (auto* hdr = dynamic_cast<PlumeComponent*> ( getParentComponent() // presetComp
				                                   ->getParentComponent() // sideBarComp
				                                   ->getParentComponent() // editor
				                                   ->findChildWithID("header")))
	{
		hdr->update();
	}
}