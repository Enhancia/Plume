/*
  ==============================================================================

    PresetHandler.cpp
    Created: 9 Jan 2019 12:15:28pm
    Author:  Alex

  ==============================================================================
*/

#include "Presets/PresetHandler.h"

PresetHandler::PresetHandler()
{
    currentPresetName = "-";
    initialiseDirectories();
}

PresetHandler::~PresetHandler()
{
    defaultPresets.clear();
    userPresets.clear();
}
    
//==============================================================================
void PresetHandler::setUserDirectory (const File& newDir)
{
    userDir = newDir;
}

String PresetHandler::getCurrentPreset()
{
    return currentPresetName.replaceCharacter (' ', '_');
}

void PresetHandler::storePresets()
{
    // Adds default presets
    defaultPresets.clear();
    if (defaultDir.exists() && defaultDir.getNumberOfChildFiles (File::findFiles + File::ignoreHiddenFiles, "*.plume") != 0)
    {
        for (auto f : defaultDir.findChildFiles (File::findFiles + File::ignoreHiddenFiles, false, "*.plume"))
        {
            defaultPresets.add (new File (f));
        }
    }
    
    // Adds user presets
    userPresets.clear();
    if (userDir.exists() && userDir.getNumberOfChildFiles (File::findFiles + File::ignoreHiddenFiles, "*.plume") != 0)
    {
        for (auto f : userDir.findChildFiles (File::findFiles + File::ignoreHiddenFiles, false, "*.plume"))
        {
            userPresets.add (new File (f));
        }
    }
}

int PresetHandler::getNumPresets()
{
    return (defaultPresets.size() + userPresets.size());
}

String PresetHandler::getTextForPresetId (int id)
{
    if (id < 0 || id >= getNumPresets()) return "-";
    
    bool isUser = id >= defaultPresets.size();
    
    if (isUser)
    {
        id -= defaultPresets.size();
        return userPresets[id]->getFileNameWithoutExtension();
    }
    else
    {
        return defaultPresets[id]->getFileNameWithoutExtension();
    }
}

bool PresetHandler::canSavePreset()
{
	return (currentPresetName != "-" && !currentIsDefault);
}

XmlElement* PresetHandler::getPresetXmlToLoad (int selectedPreset)
{
	if (selectedPreset < 0 || selectedPreset > getNumPresets()) return nullptr;
	
	currentIsDefault = selectedPreset < defaultPresets.size();
	
	currentPresetName = getTextForPresetId (selectedPreset);
	
	if (currentIsDefault)
	{
	    return XmlDocument::parse (*defaultPresets[selectedPreset]);
	}
	else
    {
	    selectedPreset -= defaultPresets.size();
	    return XmlDocument::parse (*userPresets[selectedPreset]);
    }	
}

bool PresetHandler::savePreset (XmlElement& presetXml)
{
    if (!canSavePreset()) return false;

    // Tries to write the xml to the specified file
    if (userDir.getChildFile (currentPresetName + ".plume").exists())
    {
        if (presetXml.writeToFile (userDir.getChildFile (currentPresetName + ".plume"), String()))
        {
            DBG ("Preset file succesfully written");
            return true;
	    }
	    else
        {
	        DBG ("Failed to save preset");
	        return false;
        }
    }
    else
    {
        DBG ("Couldn't find preset file : " << userDir.getFullPathName() << currentPresetName << ".plume");
        return false;
    }
}

bool PresetHandler::createNewUserPreset (String presetName, XmlElement& presetXml)
{
    // If the file is succesfully created, changes current preset and saves
    if (userDir.getChildFile (presetName + ".plume").create())
    {
        // Saves the current preset in case saving fails
        String formerPreset = currentPresetName;
        bool currentWasDefault = currentIsDefault;
        
        // Attempt to save preset
        currentPresetName = presetName;
		currentIsDefault = false;
        if (savePreset (presetXml))
        {
            userPresets.add (new File (userDir.getChildFile (presetName + ".plume")));
            return true;
        }
        else
        {
            // restores to former values and deletes file
            currentPresetName = formerPreset;
            currentIsDefault = currentWasDefault;
            userDir.getChildFile (presetName + ".plume").deleteFile();
            return false;
        }
    }
    
    return false;
}


void PresetHandler::resetPreset()
{
    DBG ("Preset Reset!");
    
    currentPresetName = "-";
    currentIsDefault = false;
}
    
//==============================================================================
void PresetHandler::initialiseDirectories()
{
    File baseDir;
    
  #if JUCE_WINDOWS
    baseDir = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/")
                                                                           .getChildFile ("Plume/")
                                                                           .getChildFile ("Presets/");
  #elif JUCE_MAC
    baseDir = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Audio/")
                                                                           .getChildFile ("Presets/")
                                                                           .getChildFile ("Enhancia/")
                                                                           .getChildFile ("Plume/");
  #else
    return; //Should only compile on win or mac
  #endif
    
    defaultDir = baseDir.getChildFile ("Default/");
    userDir = baseDir.getChildFile ("User/");
    
    storePresets();
}