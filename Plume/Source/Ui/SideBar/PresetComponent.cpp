/*
  ==============================================================================

    PresetComponent.cpp
    Created: 2 Jul 2018 10:16:13am
    Author:  Alex

  ==============================================================================
*/

#include "Ui/SideBar/PresetComponent.h"

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))

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
    
    pluginSelectBox->setJustificationType (Justification::centredLeft);
    pluginSelectBox->setColour (ComboBox::outlineColourId, Colour (0x00000000));
    pluginSelectBox->setColour (ComboBox::textColourId,
                                PLUME::UI::currentTheme.getColour (PLUME::colour::presetsBoxStandartText));
    pluginSelectBox->setColour (ComboBox::arrowColourId,
                                PLUME::UI::currentTheme.getColour (PLUME::colour::presetsBoxStandartText));
                                
    pluginSelectBox->setColour (ComboBox::backgroundColourId, Colour (0x30000000));
        
    pluginSelectBox->setTextWhenNothingSelected ("All Plugins");
    pluginSelectBox->addListener (this);
    
    // FilterBox
    addAndMakeVisible (filterBox = new FilterBox (TRANS ("filterBox"), processor));
    filterBox->updateContent();
    filterBox->selectRow (0);
    
    // Type toggle
    addAndMakeVisible (typeToggle = new TypeToggleComponent (processor));
    
    // newButton    
    addAndMakeVisible (newButton = new TextButton ("newButton"));
    newButton->setButtonText ("New Preset");
    newButton->addListener (this);
    newButton->setColour (TextButton::buttonColourId, Colour (0x00323232));
    newButton->setColour (TextButton::textColourOnId, Colour (0xaaffffff));
    
    
}

PresetComponent::~PresetComponent()
{
    TRACE_IN;
    presetBox = nullptr;
    filterBox = nullptr;
	newButton = nullptr;
	typeToggle = nullptr;
	pluginSelectBox = nullptr;
}

void PresetComponent::paint (Graphics& g)
{
    auto newButtonArea = getLocalBounds().removeFromBottom (PLUME::UI::PRESET_BUTTONS_HEIGHT);
    
    g.setColour (Colour (0x30000000));
	g.fillRect (newButtonArea.reduced(1));
}

void PresetComponent::paintOverChildren (Graphics& g)
{
    using namespace PLUME::UI;
    
    //Gradient for the box's outline
    auto gradOut = ColourGradient::horizontal (currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               MARGIN, 
                                               currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               getWidth() - MARGIN);
    gradOut.addColour (0.5, currentTheme.getColour(PLUME::colour::sideBarSeparatorIn));
    g.setGradientFill (gradOut);
    
    
    // Draws some outlines
    auto area = getLocalBounds();
    
    typeToggle->setBounds (area.removeFromTop (PRESET_BUTTONS_HEIGHT + MARGIN)
                               .withTrimmedBottom (MARGIN)); // Type toggle
                               
    //area.removeFromTop (PRESET_BUTTONS_HEIGHT + MARGIN); // Search Bar
    area.removeFromTop (4*PRESET_BUTTONS_HEIGHT + MARGIN); // Filter Box
    
    // ComboBox
	g.drawRect (area.removeFromTop (PRESET_BUTTONS_HEIGHT + MARGIN).withTrimmedBottom (MARGIN));
    
	//New preset Button
	g.drawRect (area.removeFromBottom (PRESET_BUTTONS_HEIGHT));
}

void PresetComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();
    
    area.removeFromTop (PRESET_BUTTONS_HEIGHT + MARGIN); // Type toggle
    //area.removeFromTop (PRESET_BUTTONS_HEIGHT + MARGIN); // Search Bar
    
    // Filter Box
    filterBox->setBounds (area.removeFromTop (4*PRESET_BUTTONS_HEIGHT + MARGIN)
                              .withTrimmedBottom (MARGIN));
                              
    pluginSelectBox->setBounds (area.removeFromTop (PRESET_BUTTONS_HEIGHT + MARGIN)
                                    .withTrimmedBottom (MARGIN)); // Plugin Filter comboBox
    
    newButton->setBounds (area.removeFromBottom (PRESET_BUTTONS_HEIGHT + MARGIN)
                              .withTrimmedTop (MARGIN)); // New Preset Button
    
    presetBox->setBounds (area);
    
}

//==============================================================================
const String PresetComponent::getInfoString()
{
    return "- Preset List.\n"
           "- Manages all your presets. Make sure you added a proper User directory"
           "to create your custom presets.";
}

void PresetComponent::update()
{
    presetBox->deselectRow (presetBox->getLastRowSelected());
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

void PresetComponent::buttonClicked (Button* bttn)
{
    /*
    if (bttn == saveButton)
    {
        savePreset();
    }
    */
    
    if (bttn == newButton)
    {
        addNewPreset();
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
	    
	processor.getPresetHandler().savePreset (*presetXml);

	presetXml->deleteAllChildElements();
}

void PresetComponent::addNewPreset()
{
    presetBox->startNewPresetEntry();
}

void PresetComponent::createComboBox()
{
    pluginSelectBox->clear (dontSendNotification);
    pluginSelectBox->addItem ("All Plugins", 1);
    
    KnownPluginList& kpl = processor.getWrapper().getList();
    
    for (int i=0; i<kpl.getNumTypes(); i++)
    {
        pluginSelectBox->addItem (kpl.getType (i)->name, i+2);
    }
    
    pluginSelectBox->setSelectedId (1, sendNotification);
}