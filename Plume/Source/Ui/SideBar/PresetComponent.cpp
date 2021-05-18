/*
  ==============================================================================

    PresetComponent.cpp
    Created: 2 Jul 2018 10:16:13am
    Author:  Alex

  ==============================================================================
*/

#include "PresetComponent.h"

PresetComponent::PresetComponent (PlumeProcessor& p)  : processor (p)
{
    TRACE_IN;
    
    setName ("Preset List");
    setComponentID ("presetComponent");
    
    // PresetBox
    presetBox.reset (new PresetBox (TRANS ("presetBox"), processor));
    addAndMakeVisible (*presetBox);
    presetBox->updateContent();
    
    // ComboBox
    pluginSelectBox.reset (new ComboBox ("pluginSelectBox"));
    addAndMakeVisible (*pluginSelectBox);
    createComboBox();
    
    // FilterBox
    filterBox.reset (new FilterBox (TRANS ("filterBox"), processor));
    addAndMakeVisible (*filterBox);
    filterBox->updateContent();
    filterBox->selectRow (0);
    
    // Type toggle
    typeToggle.reset (new TypeToggleComponent (processor));
    addAndMakeVisible (*typeToggle);

    // Search Bar
    searchBar.reset (new PresetSearchBar (processor));
    addAndMakeVisible (*searchBar);
}

PresetComponent::~PresetComponent()
{
    TRACE_IN;
    presetBox = nullptr;
    filterBox = nullptr;
	typeToggle = nullptr;
	pluginSelectBox = nullptr;
	searchBar = nullptr;
}

void PresetComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    
    // Top background
    g.setColour (getPlumeColour (sideBarObjectFillBackground));

    g.fillRoundedRectangle (getLocalBounds().withBottom (filterBox->getBounds().getBottom() + 1).toFloat(),
                            8.0f);

    // Bottom background
    g.fillRoundedRectangle (getLocalBounds().withTop (presetBox->getBounds().getY() - 1).toFloat(),
                            8.0f);
    // SearchBarFill
    g.saveState();
    g.reduceClipRegion (searchBar->getBounds().expanded (MARGIN, MARGIN_SMALL));
    g.setColour (getPlumeColour (sideBarObjectFill));
    g.fillRoundedRectangle (getLocalBounds().withBottom (filterBox->getBounds().getBottom() + 1).toFloat(),
                            8.0f);
    g.restoreState();
}

void PresetComponent::paintOverChildren (Graphics&)
{
}

void PresetComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();
    
    typeToggle->setBounds (area.removeFromTop (PRESET_BUTTONS_HEIGHT)
                               .reduced (2*MARGIN, 0));

    searchBar->setBounds (area.removeFromTop (PRESET_SEARCHBAR_HEIGHT).reduced (MARGIN, MARGIN_SMALL));
    
    filterBox->setBounds (area.removeFromTop (80 + MARGIN)
                              .withTrimmedBottom (MARGIN_SMALL)
                              .reduced (MARGIN, 2));
    
    pluginSelectBox->setBounds (area.removeFromTop (20 + MARGIN_SMALL)
                                    .withTrimmedBottom (MARGIN_SMALL)); // Plugin Filter comboBox
    
    presetBox->setBounds (area.reduced (MARGIN, 2));
}

//==============================================================================
const String PresetComponent::getInfoString()
{
    return "Preset List :\n\n"
           "- Manages all your presets.\n"
           "- Use the upper part to filter the presets you want. \n"
           "- Double click a preset to load it.";
}

void PresetComponent::update()
{
    presetBox->updateContent();
    createComboBox();
    presetBox->update();
}

//==============================================================================
void PresetComponent::focusLost (Component::FocusChangeType cause)
{
    if (cause == Component::focusChangedByMouseClick)
    {
        presetBox->deselectAllRows();
    }
}

void PresetComponent::comboBoxChanged (ComboBox* cmbx)
{
    if (!(cmbx->getSelectedId() == 1 && processor.getPresetHandler().getCurrentSettings().plugin.isEmpty()) &&
        cmbx->getText() != processor.getPresetHandler().getCurrentSettings().plugin)
    {
        if (cmbx->getSelectedId() == 1) processor.getPresetHandler().setPluginSearchSetting (String());
        else                            processor.getPresetHandler().setPluginSearchSetting (cmbx->getText());
        
        presetBox->updateContent();
        presetBox->selectRow (0);
        presetBox->scrollToEnsureRowIsOnscreen (0);
    }
}

void PresetComponent::savePreset()
{
    auto presetXml = std::make_unique<XmlElement> (processor.getPresetHandler().getCurrentPresetName());
	processor.createPluginXml (*presetXml);
	processor.createGestureXml (*presetXml);
	    
	//processor.getPresetHandler().savePreset (*presetXml);

	presetXml->deleteAllChildElements();
}

void PresetComponent::addNewPreset()
{
    presetBox->startNewPresetEntry();
}

void PresetComponent::createComboBox()
{
    // Combo bex items
    pluginSelectBox->clear (dontSendNotification);
    pluginSelectBox->addItem ("All Plugins", 1);
    
    KnownPluginList& kpl = processor.getWrapper().getList();
    
    for (int i=0; i<kpl.getNumTypes(); i++)
    {
        pluginSelectBox->addItem (kpl.getType (i)->name, i+2);
    }
    
    pluginSelectBox->setSelectedId (1, sendNotification);

    // Combo box look
    pluginSelectBox->setJustificationType (Justification::centredLeft);

    pluginSelectBox->setColour (ComboBox::outlineColourId, Colour (0x00000000));
    pluginSelectBox->setColour (ComboBox::textColourId,
                                getPlumeColour (presetsBoxRowText).withAlpha (0.6f));
    pluginSelectBox->setColour (ComboBox::arrowColourId,
                                getPlumeColour (presetsBoxRowText).withAlpha (0.6f));
    pluginSelectBox->setColour (ComboBox::backgroundColourId, getPlumeColour (sideBarObjectFillBackground));
    pluginSelectBox->setTextWhenNothingSelected ("All Plugins");
    pluginSelectBox->addListener (this);
}
