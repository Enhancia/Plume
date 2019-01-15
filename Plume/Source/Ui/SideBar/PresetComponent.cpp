/*
  ==============================================================================

    PresetComponent.cpp
    Created: 2 Jul 2018 10:16:13am
    Author:  Alex

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"
#include "Ui/SideBar/PresetComponent.h"

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))

PresetComponent::PresetComponent (PlumeProcessor& p)  : processor (p)
{
    TRACE_IN;
    
    addAndMakeVisible (presetBox = new PresetBox (TRANS ("presetBox"), processor));
    presetBox->setColour (ListBox::backgroundColourId, Colour (0x00000000));
    presetBox->setColour (ListBox::outlineColourId, Colour (0x00000000));
    presetBox->updateContent();
    
    addAndMakeVisible (saveButton = new TextButton ("saveButton"));
    saveButton->setButtonText ("Save");
    saveButton->addListener (this);
    saveButton->setColour (TextButton::buttonColourId, Colour (0x00323232));
    saveButton->setColour (TextButton::textColourOnId, Colour (0xaaffffff));
        
    addAndMakeVisible (newButton = new TextButton ("newButton"));
    newButton->setButtonText ("New");
    newButton->addListener (this);
    newButton->setColour (TextButton::buttonColourId, Colour (0x00323232));
    newButton->setColour (TextButton::textColourOnId, Colour (0xaaffffff));
}

PresetComponent::~PresetComponent()
{
    TRACE_IN;
    presetBox = nullptr;
	saveButton = nullptr;
	newButton = nullptr;
}

void PresetComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    
    //Gradient for the box's inside
    auto gradIn = ColourGradient::vertical (Colour (0x30000000),
                                            0, 
                                            Colour (0x30000000),
                                            getHeight());
                                          
    gradIn.addColour (0.6, Colour (0x00000000));
    gradIn.addColour (1.0 - double (PRESET_BUTTONS_HEIGHT)/getHeight(), Colour (0x30000000));
    g.setGradientFill (gradIn);
    g.fillRect (1, 1, getWidth()-2, getHeight()-2);
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
    g.drawRect (getLocalBounds());
    
	g.drawHorizontalLine (getHeight()-PRESET_BUTTONS_HEIGHT, 0, getWidth());
	g.drawVerticalLine (getWidth()/2, getHeight()-PRESET_BUTTONS_HEIGHT+1, getHeight()-1);
}

void PresetComponent::resized()
{
    auto area = getLocalBounds();
    
    auto buttonArea = area.removeFromBottom (PLUME::UI::PRESET_BUTTONS_HEIGHT);
    saveButton->setBounds (buttonArea.removeFromRight (buttonArea.getWidth()/2));
    newButton->setBounds (buttonArea);
    
    presetBox->setBounds (area);
    
}

void PresetComponent::buttonClicked (Button* bttn)
{
    if (bttn == saveButton)
    {
        savePreset();
    }
    
    else if (bttn == newButton)
    {
        addNewPreset();
    }
}

void PresetComponent::savePreset()
{
    ScopedPointer<XmlElement> presetXml = new XmlElement (processor.getPresetHandler().getCurrentPreset());
	processor.createPluginXml (*presetXml);
	processor.createGestureXml (*presetXml);
	    
	processor.getPresetHandler().savePreset (*presetXml);

	presetXml->deleteAllChildElements();
}

void PresetComponent::addNewPreset()
{
    presetBox->startNewPresetEntry();
}

void PresetComponent::update()
{
}