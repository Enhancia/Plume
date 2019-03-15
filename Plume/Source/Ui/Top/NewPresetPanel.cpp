/*
  ==============================================================================

    NewPresetPanel.cpp
    Created: 12 Feb 2019 4:08:14pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "NewPresetPanel.h"

//==============================================================================
NewPresetPanel::NewPresetPanel (PlumeProcessor& proc)   : processor (proc)
{
    setName ("New Preset Window");
    setComponentID ("newPresetPanel");
    
    // Panel Area
    panelArea = getBounds().reduced (getWidth()/3, getHeight()/4);
    
    using namespace PLUME;
    createLabels();
    createBox();
    
    addAndMakeVisible (cancelButton = new TextButton ("cancelButton"));
    addAndMakeVisible (saveButton = new TextButton ("saveButton"));
    cancelButton->setButtonText ("Cancel");
    saveButton->setButtonText ("Save");
    cancelButton->addListener (this);
    saveButton->addListener (this);
}

NewPresetPanel::~NewPresetPanel()
{
    nameLabel = nullptr;
    authorLabel = nullptr;
    verLabel = nullptr;
    typeBox = nullptr;
    pluginLabel = nullptr;
    saveButton = nullptr;
    cancelButton = nullptr;
}

void NewPresetPanel::paint (Graphics& g)
{
    using namespace PLUME::UI;
    
    ColourGradient gradFill (currentTheme.getColour (PLUME::colour::topPanelBackground)
                                         .overlaidWith (Colour (0x10000000)),
                             float (panelArea.getCentreX()),
                             float (panelArea.getBottom()),
                             currentTheme.getColour (PLUME::colour::topPanelBackground),
                             float (panelArea.getCentreX()),
                             float (panelArea.getY()),
                             true);
    gradFill.addColour (0.7, currentTheme.getColour (PLUME::colour::topPanelBackground)
                                         .overlaidWith (Colour (0x10000000)));

    // transparent area
    g.setColour (currentTheme.getColour (PLUME::colour::topPanelTransparentArea));
    g.fillRect (getBounds());
    
    // panel area
    g.setGradientFill (gradFill);
    g.fillRect (panelArea);
    
    // panel outline
    auto gradOut = ColourGradient::horizontal (currentTheme.getColour(colour::sideBarSeparatorOut),
                                               float(panelArea.getX()), 
                                               currentTheme.getColour(colour::sideBarSeparatorOut),
                                               float(panelArea.getRight()));
    gradOut.addColour (0.5, currentTheme.getColour(colour::sideBarSeparatorIn));

    g.setGradientFill (gradOut);
    g.drawRect (panelArea);
    
    auto textArea = panelArea.reduced (MARGIN).removeFromLeft (panelArea.reduced (MARGIN).getWidth()/3)
                                              .reduced (MARGIN, 0);
    
    // Text
    g.setColour (currentTheme.getColour (colour::topPanelMainText));
    g.setFont (PLUME::font::plumeFontBook.withHeight (13.0f));
    
    g.drawText ("Name :", 
                textArea.removeFromTop (OPTIONS_HEIGHT + MARGIN).withTrimmedBottom (MARGIN),
                Justification::centredLeft, true);
                
    g.drawText ("Author :", 
                textArea.removeFromTop (OPTIONS_HEIGHT + MARGIN).withTrimmedBottom (MARGIN),
                Justification::centredLeft, true);
                
    g.drawText ("Version :", 
                textArea.removeFromTop (OPTIONS_HEIGHT + MARGIN).withTrimmedBottom (MARGIN),
                Justification::centredLeft, true);
                
    g.drawText ("Category :", 
                textArea.removeFromTop (OPTIONS_HEIGHT + MARGIN).withTrimmedBottom (MARGIN),
                Justification::centredLeft, true);
                
    g.drawText ("Plugin :", 
                textArea.removeFromTop (OPTIONS_HEIGHT + MARGIN).withTrimmedBottom (MARGIN),
                Justification::centredLeft, true);
}

void NewPresetPanel::resized()
{
	using namespace PLUME::UI;
    int h = OPTIONS_HEIGHT < (getHeight() - 16*MARGIN)/6 ? OPTIONS_HEIGHT : (getHeight() - 16*MARGIN)/7;
    
    // Panel Area
    panelArea = getBounds().reduced (getWidth()/3, 0)
                           .withHeight (6*h + 8*MARGIN)
                           .withCentre (getBounds().getCentre());
    
    auto valuesArea = panelArea.reduced (MARGIN).removeFromRight (panelArea.reduced (MARGIN).getWidth()*2/3)
                                                .reduced (MARGIN, 0);
                                                
    nameLabel  ->setBounds (valuesArea.removeFromTop (h + MARGIN).withTrimmedBottom (MARGIN));
    authorLabel->setBounds (valuesArea.removeFromTop (h + MARGIN).withTrimmedBottom (MARGIN));
    verLabel   ->setBounds (valuesArea.removeFromTop (h + MARGIN).withTrimmedBottom (MARGIN));
    typeBox    ->setBounds (valuesArea.removeFromTop (h + MARGIN).withTrimmedBottom (MARGIN));
    pluginLabel->setBounds (valuesArea.removeFromTop (h + MARGIN).withTrimmedBottom (MARGIN));
    
    auto buttonArea = panelArea.reduced (MARGIN).removeFromBottom (h);
                                                
    cancelButton->setBounds (buttonArea.removeFromLeft (buttonArea.getWidth()/2).reduced (3*MARGIN, MARGIN/2));
    saveButton  ->setBounds (buttonArea.reduced (3*MARGIN, MARGIN/2));
}

void NewPresetPanel::visibilityChanged()
{
    if (isVisible())
    {
        update();

        // Makes the name ready to be edited
        if (getParentComponent() != nullptr) nameLabel->showEditor();
    }
}

void NewPresetPanel::buttonClicked (Button* bttn)
{
    if (bttn == cancelButton)
    {
        // TODO alert are you sure??
    }
    else if (bttn == saveButton)
    {
        if (nameLabel->getText() == "Preset Name...")
        {
            nameLabel->setColour (Label::outlineColourId, Colour (0xaaff0000));
            return;
        }
        
        createUserPreset();
    }
    
    setVisible (false);
}

void NewPresetPanel::labelTextChanged (Label* lbl)
{
    if (lbl == nameLabel)
    {
        if (lbl->getText().isEmpty())
        {
            lbl->setText ("Preset Name...", dontSendNotification);
            lbl->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                                 .withAlpha (0.6f));
        }
        else
        {
            lbl->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText));
        }
    }
    
    else if (lbl == authorLabel)
    {
        if (lbl->getText().isEmpty())
        {
            lbl->setText ("Author Name...", dontSendNotification);
            lbl->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                                 .withAlpha (0.6f));
        }
        else
        {
            lbl->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText));
        }
    }
    
    else if (lbl == verLabel)
    {
        if (lbl->getText().isEmpty() || !lbl->getText().containsOnly ("0123456789."))
        {
            lbl->setText ("1.0", dontSendNotification);
            lbl->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                                 .withAlpha (0.6f));
        }
        else
        {
            lbl->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText));
        }
    }
}

void NewPresetPanel::editorShown (Label* lbl, TextEditor& ed)
{
    if (lbl == nameLabel && lbl->getText() == "Preset Name...")
    {
        lbl->setColour (Label::outlineColourId, Colour (0x000000));
        
        // The user doesn't have to manually remove "Preset Name :"
        ed.setText ("", false);
    }
    
    if (lbl == authorLabel && lbl->getText() == "Author Name...")
    {
        // The user doesn't have to manually remove "Preset Name :"
        ed.setText ("", false);
    }
    
    if (lbl == verLabel && lbl->getText() == "1.0")
    {
        // The user doesn't have to manually remove "Preset Name :"
        ed.setText ("", false);
    }
}

void NewPresetPanel::update()
{
    //PresetHandler::PresetSearchSettings settings = processor.getPresetHandler().getCurrentSettings();
    
    nameLabel->setText ("Preset Name...", dontSendNotification);
    nameLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                               .withAlpha (0.6f));
                                                               
    authorLabel->setText ("Author Name...", dontSendNotification);
    authorLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                                 .withAlpha (0.6f));
                                                                 
    verLabel->setText ("1.0", dontSendNotification);
    pluginLabel->setText (processor.getWrapper().getWrappedPluginName(), dontSendNotification);
    
    int filterToSelect = processor.getPresetHandler().getCurrentSettings().filterType;
    if (filterToSelect < 0 || filterToSelect > PlumePreset::numFilters) filterToSelect = (int) PlumePreset::custom;
    typeBox->setSelectedId (filterToSelect+1, dontSendNotification);
}

void NewPresetPanel::createLabels()
{
    addAndMakeVisible (nameLabel = new Label ("name", "Preset Name..."));
    nameLabel->setColour (Label::backgroundColourId, Colour (0x30000000));
    nameLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                               .withAlpha (0.6f));
    nameLabel->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    nameLabel->setEditable (true, false, false);
    nameLabel->setMouseCursor (MouseCursor (MouseCursor::IBeamCursor));
    nameLabel->addListener (this);
    
    addAndMakeVisible (authorLabel = new Label ("author", "Author Name..."));
    authorLabel->setColour (Label::backgroundColourId, Colour (0x30000000));
    authorLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                                 .withAlpha (0.6f));
    authorLabel->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    authorLabel->setEditable (true, false, false);
    authorLabel->setMouseCursor (MouseCursor (MouseCursor::IBeamCursor));
    authorLabel->addListener (this);
    
    addAndMakeVisible (verLabel = new Label ("version", "1.0"));
    verLabel->setColour (Label::backgroundColourId, Colour (0x30000000));
    verLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                              .withAlpha (0.6f));
    verLabel->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    verLabel->setEditable (true, false, false);
    verLabel->setMouseCursor (MouseCursor (MouseCursor::IBeamCursor));
    verLabel->addListener (this);
    
    addAndMakeVisible (pluginLabel = new Label ("plugin", "None"));
    pluginLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    pluginLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                                 .withAlpha (0.6f));
    pluginLabel->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    pluginLabel->setEditable (false, false, false);
    pluginLabel->addListener (this);
}

void NewPresetPanel::createBox()
{
    addAndMakeVisible (typeBox = new ComboBox ("typeBox"));
    
    typeBox->setJustificationType (Justification::centredLeft);
    typeBox->setColour (ComboBox::outlineColourId, Colour (0x8000000));
    typeBox->setColour (ComboBox::backgroundColourId, Colour (0x3000000));
    typeBox->setColour (ComboBox::textColourId,
                                PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText));
    typeBox->setColour (ComboBox::arrowColourId,
                                PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText)
                                                       .withAlpha (0.6f));
        
    typeBox->setTextWhenNothingSelected ("Category...");
    
    for (int i=0; i<PlumePreset::numFilters; i++)
    {
        typeBox->addItem (PlumePreset::getFilterTypeString (i), i+1);
    }
    
    typeBox->setSelectedId (PlumePreset::custom+1, dontSendNotification);
}

void NewPresetPanel::createUserPreset()
{
    ScopedPointer<XmlElement> presetXml = new XmlElement (nameLabel->getText().replace (" ", "_"));
	processor.createPluginXml (*presetXml);
	processor.createGestureXml (*presetXml);
    PlumePreset::addPresetInfoXml (*presetXml, authorLabel->getText() == "Author Name..." ? "" : authorLabel->getText(),
                                               verLabel->getText(),
                                               pluginLabel->getText() == "No plugin" ? "" : pluginLabel->getText(),
                                               PlumePreset::userPreset,
                                               typeBox->getSelectedId()-1);
	
    processor.getPresetHandler().createNewUserPreset (nameLabel->getText(), *presetXml);
    
    // Updates Header..
    if (auto* hdr = dynamic_cast<PlumeComponent*> ( getParentComponent() // editor
				                                   ->findChildWithID("header")))
	{
		hdr->update();
	}
	
    // Updates Preset box..
	if (auto* presetBox = dynamic_cast<ListBox*> ( getParentComponent() // editor
				                                   ->findChildWithID("sideBar")
				                                   ->findChildWithID("presetComponent")
				                                   ->findChildWithID("presetBox")))
	{
		presetBox->updateContent();
	}
    
    presetXml->deleteAllChildElements();
}
