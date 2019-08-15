/*
  ==============================================================================

    PresetComponent.cpp
    Created: 2 Jul 2018 10:16:13am
    Author:  Alex

  ==============================================================================
*/

#include "Ui/SideBar/PresetComponent.h"

PresetComponent::PresetComponent (PlumeProcessor& p)  : processor (p)
{
    TRACE_IN;
    
    setName ("Preset List");
    setComponentID ("presetComponent");
    
    // PresetBox
    addAndMakeVisible (presetBox = new PresetBox (TRANS ("presetBox"), processor));
    presetBox->updateContent();
    
    // ComboBox
    addAndMakeVisible (pluginSelectBox = new ComboBox ("pluginSelectBox"));
    createComboBox();
    
    // FilterBox
    addAndMakeVisible (filterBox = new FilterBox (TRANS ("filterBox"), processor));
    filterBox->updateContent();
    filterBox->selectRow (0);
    
    // Type toggle
    addAndMakeVisible (typeToggle = new TypeToggleComponent (processor));
    
    // Search Bar
    addAndMakeVisible (searchBar = new PresetSearchBar (processor));
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
    
    auto area = getLocalBounds();
    
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
    if (processor.getPresetHandler().getCurrentPresetIdInSearchList() != -1)
    {
        presetBox->selectRow (processor.getPresetHandler().getCurrentPresetIdInSearchList());
    }
    else
    {
        presetBox->deselectRow (presetBox->getLastRowSelected());
    }

    presetBox->updateContent();


    createComboBox();
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
    if (cmbx->getSelectedId() == 1) processor.getPresetHandler().setPluginSearchSetting (String());
    else                            processor.getPresetHandler().setPluginSearchSetting (cmbx->getText());
    
    presetBox->deselectRow (presetBox->getLastRowSelected());
    presetBox->updateContent();
}

void PresetComponent::savePreset()
{
    ScopedPointer<XmlElement> presetXml = new XmlElement (processor.getPresetHandler().getCurrentPresetName());
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