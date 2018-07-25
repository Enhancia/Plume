/*
  ==============================================================================

    PresetComponent.cpp
    Created: 2 Jul 2018 10:16:13am
    Author:  Alex

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"
#include "Ui/Presets/PresetComponent.h"

PresetComponent::PresetComponent(PlumeProcessor& p)  : processor (p)
{   
    addAndMakeVisible (nameLabel = new Label ("nameLabel", TRANS ("No current preset")));
    nameLabel->setJustificationType (Justification::centred);
    nameLabel->setEditable (false, false, false);
    nameLabel->setColour (Label::backgroundColourId, Colour (0xff505050));
    nameLabel->setColour (Label::textColourId, Colour (0xffffffff));
    nameLabel->setColour (Label::outlineColourId, Colour (0x00000000));
        
    addAndMakeVisible (saveButton = new TextButton ("saveButton"));
    saveButton->setButtonText ("Save");
    saveButton->addListener (this);
    saveButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
        
    addAndMakeVisible (loadButton = new TextButton ("loadButton"));
    loadButton->setButtonText ("Load");
    loadButton->addListener (this);
    loadButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
}

PresetComponent::~PresetComponent()
{
    nameLabel  = nullptr;
    saveButton = nullptr;
    loadButton = nullptr;
}

void PresetComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xff909090));
}

void PresetComponent::resized ()
{
    nameLabel->setBounds (0, 0, getWidth(), getHeight()/4);
    saveButton->setBounds (getWidth()/7, getHeight()/2, getWidth()*2/7, getHeight()/4);
    loadButton->setBounds (getWidth()*4/7, getHeight()/2, getWidth()*2/7, getHeight()/4);
}

void PresetComponent::buttonClicked (Button* bttn)
{
    if (bttn == saveButton)
    {
        savePreset();
    }
    else if (bttn == loadButton)
    {
        loadPreset();
    }
}

void PresetComponent::savePreset()
{
    // Lets the user chose the location and name to save a preset file
    FileChooser presetSaver ("Select the folder you want the preset to be saved to.",
                             File::getSpecialLocation (File::currentApplicationFile).getParentDirectory(),
                             ".xml");
        
    if (presetSaver.browseForFileToSave(true))
    {
        // Creates the preset's Xml
        ScopedPointer<XmlElement> presetXml = new XmlElement (presetSaver.getResult().getFileNameWithoutExtension());
        processor.createPluginXml   (*presetXml);
        processor.createGestureXml (*presetXml);
        
        // Tries to write the xml to the specified file
        if (presetXml->writeToFile (presetSaver.getResult(), String()))
        {
            DBG ("Preset file succesfully written");
            //currentPreset = presetXml->getTagName();
	        nameLabel->setText (TRANS (presetXml->getTagName()), 
					            dontSendNotification);
		}
				
		presetXml->deleteAllChildElements();
    }
}

void PresetComponent::loadPreset()
{
    // Lets the user chose a preset file to load
    FileChooser presetLoader ("Select the preset file to load.",
                              File::getSpecialLocation (File::currentApplicationFile).getParentDirectory(),
                              "*.xml");
                              
    if (presetLoader.browseForFileToOpen())
    {
        DBG ("File Opened");
        
        // Creates a binary memory block with the loaded xml data
        MemoryBlock presetData;
        ScopedPointer<XmlElement> presetXml = XmlDocument::parse (presetLoader.getResult());
	    AudioProcessor::copyXmlToBinary(*presetXml, presetData);
                
	    // Calls the plugin's setStateInformation method to load the preset
        processor.setStateInformation (presetData.getData(), presetData.getSize());

        //currentPreset = presetXml->getTagName();
		nameLabel->setText (TRANS (presetXml->getTagName()),
							dontSendNotification);
			    
	    presetXml->deleteAllChildElements();
    }
}

void PresetComponent::update()
{
    //nameLabel->setText (TRANS (processor.getWrapper().getWrappedPluginName()), dontSendNotification);
}