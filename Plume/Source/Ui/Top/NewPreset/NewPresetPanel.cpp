/*
  ==============================================================================

    NewPresetPanel.cpp
    Created: 12 Feb 2019 4:08:14pm
    Author:  Alex

  ==============================================================================
*/

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
    
    cancelButton.reset (new TextButton ("cancelButton"));
    addAndMakeVisible (*cancelButton);
    saveButton.reset (new TextButton ("saveButton"));
    addAndMakeVisible (*saveButton);
    
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

    // transparent area
    g.setColour (getPlumeColour (topPanelTransparentArea));
    g.fillRect (getBounds());
    
    // panel area
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

    g.setGradientFill (gradFill);
    g.fillRoundedRectangle (panelArea.toFloat(), 10.0f);
    
    // panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               float(panelArea.getX()), 
                                               Colour (0x10ffffff),
                                               float(panelArea.getRight()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (panelArea.toFloat(), 10.0f, 1.0f);
    
    auto textArea = panelArea.reduced (MARGIN).removeFromLeft (panelArea.reduced (MARGIN).getWidth()/3)
                                              .reduced (MARGIN, 0);
    
    // Text
    g.setColour (currentTheme.getColour (PLUME::colour::topPanelMainText));
    g.setFont (PLUME::font::plumeFontMedium.withHeight (13.0f));
    
    g.drawText ("Preset Name :", 
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
    int h = (OPTIONS_HEIGHT < (getHeight() - 16*MARGIN)/6) ? OPTIONS_HEIGHT
                                                           : (getHeight() - 16*MARGIN)/7;
    
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

        // Makes the name ready to be edited!
        if (getParentComponent() != nullptr)
            nameLabel->showEditor();
    }
}

void NewPresetPanel::buttonClicked (Button* bttn)
{
    if (bttn == cancelButton.get())
    {
        // TODO alert are you sure??
    }
    else if (bttn == saveButton.get())
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
	using namespace PLUME;
    if (lbl == nameLabel.get())
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
    
    else if (lbl == authorLabel.get())
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
    
    else if (lbl == verLabel.get())
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
    if (lbl == nameLabel.get() && lbl->getText() == "Preset Name...")
    {
        lbl->setColour (Label::outlineColourId, Colour (0x000000));
        
        // The user doesn't have to manually remove "Preset Name :"
        ed.setText ("", false);
    }
    
    if (lbl == authorLabel.get() && lbl->getText() == "Author Name...")
    {
        // The user doesn't have to manually remove "Preset Name :"
        ed.setText ("", false);
    }
    
    if (lbl == verLabel.get() && lbl->getText() == "1.0")
    {
        // The user doesn't have to manually remove "Preset Name :"
        ed.setText ("", false);
    }
}

void NewPresetPanel::update()
{
	using namespace PLUME;

    auto currentPreset = processor.getPresetHandler().getCurrentPreset();
    const bool useCurrentPresetInfo = (currentPreset.isValid() && currentPreset.presetType != PlumePreset::defaultPreset);
    
    nameLabel->setText ((useCurrentPresetInfo && currentPreset.getName().isNotEmpty()) ? currentPreset.getName(): "Preset Name...", dontSendNotification);
    nameLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                               .withAlpha (0.6f));
                                                               
    authorLabel->setText ((useCurrentPresetInfo && currentPreset.getAuthor().isNotEmpty()) ? currentPreset.getAuthor() : "Author Name...", dontSendNotification);
    authorLabel->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                                 .withAlpha (0.6f));
                                                                 
    verLabel->setText ((useCurrentPresetInfo && currentPreset.getVersion().isNotEmpty()) ? currentPreset.getVersion() : "1.0", dontSendNotification);
    pluginLabel->setText (processor.getWrapper().getWrappedPluginName(), dontSendNotification);
    
    typeBox->setSelectedId (useCurrentPresetInfo ? currentPreset.getFilter()+1 : (int) PlumePreset::other+1, dontSendNotification);
}

void NewPresetPanel::createLabels()
{
    nameLabel.reset (new Label ("name", "Preset Name..."));
    addAndMakeVisible (*nameLabel);
    setLabelProperties (*nameLabel);
    
    authorLabel.reset (new Label ("author", "Author Name..."));
    addAndMakeVisible (*authorLabel);
    setLabelProperties (*authorLabel);
    
    verLabel.reset (new Label ("version", "1.0"));
    addAndMakeVisible (*verLabel);
    setLabelProperties (*verLabel);
    
    pluginLabel.reset (new Label ("plugin", "None"));
    addAndMakeVisible (*pluginLabel);
    setLabelProperties (*pluginLabel, false);
}

void NewPresetPanel::setLabelProperties (Label& labelToSet, bool editable)
{
    labelToSet.setColour (Label::textColourId, PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText)
                                                                      .withAlpha (0.6f));
    labelToSet.setFont (PLUME::font::plumeFont.withHeight (14.0f));
    labelToSet.addListener (this);

    if (editable)
    {
        labelToSet.setColour (Label::backgroundColourId, Colour (0x30000000));
        labelToSet.setEditable (true, false, false);
        labelToSet.setMouseCursor (MouseCursor (MouseCursor::IBeamCursor));
    }
    else
    {
        labelToSet.setColour (Label::backgroundColourId, Colour (0x00000000));
        labelToSet.setEditable (false, false, false);
    }
}

void NewPresetPanel::createBox()
{
    typeBox.reset (new ComboBox ("typeBox"));
    addAndMakeVisible (*typeBox);
    
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
    
    typeBox->setSelectedId (PlumePreset::other+1, dontSendNotification);
}

void NewPresetPanel::createUserPreset()
{
    auto presetXml = std::make_unique<XmlElement> ("PLUME");
	processor.createPluginXml (*presetXml);
	processor.createGestureXml (*presetXml);
    PlumePreset::addPresetInfoXml (*presetXml, nameLabel->getText() == "Preset Name..." ? "" : nameLabel->getText(),
                                               authorLabel->getText() == "Author Name..." ? "" : authorLabel->getText(),
                                               verLabel->getText(),
                                               pluginLabel->getText() == "No plugin" ? "" : pluginLabel->getText(),
                                               PlumePreset::userPreset,
                                               typeBox->getSelectedId()-1);
	
    processor.getPresetHandler().createNewOrOverwriteUserPreset (*presetXml);
    
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
