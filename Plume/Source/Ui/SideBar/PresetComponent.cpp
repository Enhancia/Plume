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
                            6.0f);

    // Bottom background
    g.fillRoundedRectangle (getLocalBounds().withTop (presetBox->getBounds().getY() - 1).toFloat(),
                            6.0f);
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
                               .reduced (MARGIN, 0));

    searchBar->setBounds (area.removeFromTop (PRESET_SEARCHBAR_HEIGHT).reduced (MARGIN, 3));
    
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
    const String bullet = " " + String::charToString (juce_wchar(0x2022));
    
    return "Preset List :\n\n"
           + bullet + " Manages all your presets.\n"
           + bullet + " Use the upper part to filter the presets you want. \n"
           + bullet + " Double click a preset to load it.";
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
    if (!(cmbx->getSelectedId() == 1 && processor.getPresetHandler().getCurrentSettings().descriptiveName.isEmpty()) &&
        cmbx->getText() != processor.getPresetHandler().getCurrentSettings().descriptiveName)
    {
        if (cmbx->getSelectedId() == 1)
            processor.getPresetHandler().setPluginSearchSetting (String());
        else
            processor.getPresetHandler().setPluginSearchSetting (cmbx->getText());
        
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

//bool compareFunction (String a, String b) { return a < b; }

void PresetComponent::createComboBox()
{
    // Combo bex items
    pluginSelectBox->clear (dontSendNotification);
    pluginSelectBox->addItem ("All Plugins", 1);
    
    KnownPluginList& kpl = processor.getWrapper().getList();
    int searchedPlugin = 1;

	Array<String> listPlugins;

	// fill list of plugins and set uppercase first letter
	for (int i = 0; i < kpl.getNumTypes (); i++)
	{
		std::string currentName = kpl.getType (i)->descriptiveName.toStdString ();
		currentName[0] = toupper (currentName[0]);
		listPlugins.add ((String)currentName);
	}

	// sort the plugins list
	std::sort (listPlugins.begin (), listPlugins.end (), [](String a, String b) { return a < b; });

	// add plugins to combobox
	for (int i = 0; i < listPlugins.size (); i++)
	{
		pluginSelectBox->addItem (listPlugins[i], i + 2);

		if (listPlugins[i] == processor.getPresetHandler ().getCurrentSettings ().descriptiveName)
			searchedPlugin = i + 2;
	}
    
    pluginSelectBox->setSelectedId (searchedPlugin, dontSendNotification);

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
