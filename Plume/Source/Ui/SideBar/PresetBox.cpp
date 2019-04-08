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
    editLabel = new Label ("editLabel", "NewPreset");
    editLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    editLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::presetsBoxStandartText));
    editLabel->setFont (PLUME::font::plumeFont.withHeight (float (getRowHeight())/2));
    editLabel->setInterceptsMouseClicks (false, false);
    editLabel->addListener (this);
    
    auto& scrollBar = getVerticalScrollBar();
    scrollBar.setColour (ScrollBar::thumbColourId, UI::currentTheme.getColour (colour::presetsBoxScrollBar));
}

PresetBox::~PresetBox()
{
    editLabel = nullptr;
}

void PresetBox::paint (Graphics& g)
{
    ListBox::paint (g);
    
    using namespace PLUME::UI;
    
    //Gradient for the box's inside
    auto gradIn = ColourGradient::vertical (Colour (0x30000000),
                                            0.0f, 
                                            Colour (0x25000000),
                                            float(getHeight()));
                                          
    gradIn.addColour (0.6, Colour (0x00000000));
    gradIn.addColour (0.8, Colour (0x25000000));
    g.setGradientFill (gradIn);
    g.fillRect (1, 1, getWidth()-2, getHeight()-2);
}

void PresetBox::paintOverChildren (Graphics& g)
{
    if (getOutlineThickness() > 0)
    {
        using namespace PLUME::UI;
    
        //Gradient for the box's outline
        auto gradOut = ColourGradient::horizontal (currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                                   float(MARGIN), 
                                                   currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                                   float(getWidth() - MARGIN));
        gradOut.addColour (0.5, currentTheme.getColour(PLUME::colour::sideBarSeparatorIn));

        g.setGradientFill (gradOut);
        g.drawRect (getLocalBounds(), getOutlineThickness());
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
        g.setColour (rowIsSelected ? UI::currentTheme.getColour (colour::presetsBoxHighlightedBackground)
                                   : UI::currentTheme.getColour (colour::presetsBoxBackground));
                               
        g.fillRect (1, 1, width-2, height-2);
    
        // Text
        g.setColour (rowIsSelected ? UI::currentTheme.getColour (colour::presetsBoxHighlightedText)
                                   : UI::currentTheme.getColour (colour::presetsBoxStandartText));
                               
        g.setFont (PLUME::font::plumeFont.withHeight (jmin (PLUME::font::SIDEBAR_LABEL_FONT_H, 
                                                            float (height*5)/6)));

        String text = processor.getPresetHandler().getTextForPresetId (rowNumber);
    
        g.drawText (text, PLUME::UI::MARGIN, 0, width, height,
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
        return editLabel;
    }
    else if (existingComponentToUpdate == editLabel)
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
        rightClickMenu.addItem (2, "Delete", isUser);
        rightClickMenu.addItem (3, "Show In Explorer", isUser);
        
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

void PresetBox::deleteKeyPressed (int)
{
}

//==============================================================================
void PresetBox::labelTextChanged (Label*)
{
    String presetName = editLabel->getText();
    
    if (XmlElement::isValidXmlName (presetName.replace (" ", "_")))
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

void PresetBox::startRenameEntry (const int row)
{
    // Puts right id and text to edit
    newPresetEntry = false;
    presetIdToEdit = row;
    editLabel->setText (processor.getPresetHandler().getTextForPresetId (row), dontSendNotification);
    updateContent();
    
    if (getComponentForRowNumber (presetIdToEdit) == editLabel)
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
            
        case 2: // Delete preset
            deletePreset (row);
            break;
            
        case 3: // Show in explorer
			processor.getPresetHandler().showPresetInExplorer (row);
    }
    
    repaint();
}

void PresetBox::setPreset (const int row)
{
    // Gets the preset Xml and loads it using the processor
    if (ScopedPointer<XmlElement> presetXml = processor.getPresetHandler().getPresetXmlToLoad (row))
    {
        MemoryBlock presetData;
        AudioProcessor::copyXmlToBinary (*presetXml, presetData);
            
        // Calls the plugin's setStateInformation method to load the preset
	    prepareGesturePanelToPresetChange();
       //Time::waitForMillisecondCounter(Time::getMillisecondCounter() + 10);

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
    ScopedPointer<XmlElement> presetXml = new XmlElement (presetName.replace (" ", "_"));
	processor.createPluginXml (*presetXml);
	processor.createGestureXml (*presetXml);
	
    processor.getPresetHandler().createNewUserPreset (presetName, *presetXml);
    updateHeader();
    
    presetXml->deleteAllChildElements();
}

void PresetBox::renamePreset (const String& newName)
{
    processor.getPresetHandler().renamePreset (newName, presetIdToEdit);
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