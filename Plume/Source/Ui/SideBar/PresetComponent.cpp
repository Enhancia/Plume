/*
  ==============================================================================

    PresetComponent.cpp
    Created: 2 Jul 2018 10:16:13am
    Author:  Alex

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"
#include "PresetComponent.h"

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))

PresetComponent::PresetComponent(PlumeProcessor& p)  : processor (p)
{
    TRACE_IN;
    addAndMakeVisible (nameLabel = new Label ("nameLabel", TRANS ("[No current preset]")));
    nameLabel->setJustificationType (Justification::centred);
    nameLabel->setEditable (true, false, false);
    nameLabel->setColour (Label::backgroundColourId, Colour (0xff323232));
	nameLabel->addListener (this);
        
    addAndMakeVisible (saveButton = new TextButton ("saveButton"));
    saveButton->setButtonText ("Save");
    saveButton->addListener (this);
    saveButton->setColour (TextButton::buttonColourId, Colour (0xff323232));
        
    addAndMakeVisible (loadButton = new TextButton ("loadButton"));
    loadButton->setButtonText ("Load");
    loadButton->addListener (this);
    loadButton->setColour (TextButton::buttonColourId, Colour (0xff323232));
}

PresetComponent::~PresetComponent()
{
    TRACE_IN;
	nameLabel->removeListener (this);
    nameLabel  = nullptr;
    saveButton = nullptr;
    loadButton = nullptr;
}

void PresetComponent::paint (Graphics& g)
{
    if (auto* lf = dynamic_cast<PlumeLookAndFeel*> (&getLookAndFeel()))
    {
        g.fillAll (lf->getPlumeColour (PlumeLookAndFeel::topPanelBackground));
    }
}

void PresetComponent::resized ()
{
    nameLabel->setBounds (0, 0, getWidth(), getHeight()/3);
    saveButton->setBounds (getWidth()/16, getHeight()/2, getWidth()*3/8, getHeight()/3);
    loadButton->setBounds (getWidth()*9/16, getHeight()/2, getWidth()*3/8, getHeight()/3);
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

void PresetComponent::labelTextChanged (Label* lbl)
{
    if (lbl == nameLabel)
    {
        String s = nameLabel->getText();
        
        if (s.isEmpty())
        {
            nameLabel->setText ("[No current preset]", dontSendNotification);
        }
        else
        {
            // replaces spaces with underscores
            if (s.containsChar(' '))
            {
                nameLabel->setText (s.replaceCharacter (' ', '_'), dontSendNotification);
            }
        }
    }
}

void PresetComponent::savePreset()
{
    TRACE_IN;
    // Lets the user chose the location and name to save a preset file
    File f (File::getSpecialLocation (File::userApplicationDataDirectory));
    Result res = createPlumeAndPresetDir (f);
    
    if (res.failed())
    {
        DBG (res.getErrorMessage());
        return;
    }
    
    if (nameLabel->getText().compare ("[No current preset]") != 0)
    {
        f = f.getChildFile (nameLabel->getText()).withFileExtension ("plume");
    }
    
    FileChooser presetSaver ("Select the folder you want the preset to be saved to.",
                             f,
                             "*.plume",
                             true);
        
    if (presetSaver.browseForFileToSave(false))
    {
        // Creates the preset's Xml
        ScopedPointer<XmlElement> presetXml = new XmlElement (presetSaver.getResult().getFileNameWithoutExtension().replaceCharacter (' ', '_'));
        processor.createPluginXml  (*presetXml);
        processor.createGestureXml (*presetXml);
        
        // Tries to write the xml to the specified file
        if (presetXml->writeToFile (presetSaver.getResult(), String()))
        {
            DBG ("Preset file succesfully written");
            
	        nameLabel->setText (TRANS (presetXml->getTagName()), 
					            dontSendNotification);
		}
		
		presetXml->deleteAllChildElements();
    }
}

void PresetComponent::loadPreset()
{
    TRACE_IN;
  #if JUCE_WINDOWS
    File f (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile("Enhancia/").getChildFile("Plume/").getChildFile("Presets/"));
  #elif JUCE_MAC
    File f (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile("Audio/").getChildFile("Presets/")
                                                                         .getChildFile("Enhancia/").getChildFile("Plume/"));
  #endif
  
    if (!f.exists()) return;
    
    // Lets the user chose a preset file to load
    FileChooser presetLoader ("Select the preset file to load.",
                              f,
                              "*.plume",
                              true);
                              
    if (presetLoader.browseForFileToOpen())
    {
        // Creates a binary memory block with the loaded xml data
        MemoryBlock presetData;
        ScopedPointer<XmlElement> presetXml = XmlDocument::parse (presetLoader.getResult());
        if (presetXml == nullptr)
        {
            DBG ("Couldn't load preset..");
            return;
        }
        
	    AudioProcessor::copyXmlToBinary (*presetXml, presetData);
                
	    // Calls the plugin's setStateInformation method to load the preset
	    PLUME::UI::ANIMATE_UI_FLAG = false;
        processor.setStateInformation (presetData.getData(), presetData.getSize());
	    PLUME::UI::ANIMATE_UI_FLAG = true;

        //currentPreset = presetXml->getTagName();
		nameLabel->setText (TRANS (presetXml->getTagName()),
							dontSendNotification);
			    
	    presetXml->deleteAllChildElements(); // frees the memory
    }
}

void PresetComponent::update()
{
}

Result PresetComponent::createPlumeAndPresetDir (File& initialDir)
{
    // If they already exist just changes the initial dir to Enhancia/Plume/Presets/User/
  #if JUCE_WINDOWS
    if (initialDir.getChildFile ("Enhancia/").getChildFile ("Plume/").getChildFile ("Presets/").getChildFile ("User/").exists())
    {
        initialDir = initialDir.getChildFile ("Enhancia/").getChildFile ("Plume/").getChildFile ("Presets/").getChildFile ("User/");
        return Result::ok();
    }
  #elif JUCE_MAC
    if (initialDir.getChildFile ("Audio/").getChildFile ("Presets/").getChildFile ("Enhancia/").getChildFile ("Plume/")
                                                                                               .getChildFile ("User/").exists())
    {
        initialDir = initialDir.getChildFile ("Audio/").getChildFile ("Presets/").getChildFile ("Enhancia/").getChildFile ("Plume/")
                                                                                                            .getChildFile ("User/");
        return Result::ok();
    }
  #endif
  
    // Creates the missing directories and sets initialDir to the deepest one
    File dirTemp (initialDir);
    
  #if JUCE_WINDOWS
    dirTemp = dirTemp.getChildFile ("Enhancia/").getChildFile ("Plume/").getChildFile ("Presets/").getChildFile ("User/");
  #elif JUCE_MAC
    dirTemp = dirTemp.getChildFile ("Audio/").getChildFile ("Presets/").getChildFile ("Enhancia/").getChildFile ("Plume/")
                                                                                                  .getChildFile ("User/");
  #endif
  
    if (!(dirTemp.exists()))
    {
        if (dirTemp.createDirectory().failed())
        {
            return Result::fail ("Couldn't create Directory");
        }
    }

    initialDir = dirTemp;
    
    return Result::ok();
}