/*
  ==============================================================================

    PresetHandler.cpp
    Created: 9 Jan 2019 12:15:28pm
    Author:  Alex

  ==============================================================================
*/

#include "Presets/PresetHandler.h"

PresetHandler::PresetHandler (ValueTree presetDirValue) : userDirValue (presetDirValue)
{
    initialiseDirectories();
    updateSearchedPresets();
}

PresetHandler::~PresetHandler()
{
	searchedPresets.clear();
    defaultPresets.clear();
    userPresets.clear();
}
    
//==============================================================================
void PresetHandler::setUserDirectory (const File& newDir)
{
    userDirValue.setProperty (PLUME::treeId::value, newDir.getFullPathName(), nullptr);
}

File PresetHandler::getUserDirectory()
{
    return File (userDirValue.getProperty (PLUME::treeId::value, var("")).toString());
}

String PresetHandler::getCurrentPresetName()
{
    return currentPreset.isValid() ? currentPreset.getName() : "-"; //.replaceCharacter (' ', '_');
}

void PresetHandler::storePresets()
{
    // Adds default presets
    defaultPresets.clear();
    if (defaultDir.exists() && defaultDir.getNumberOfChildFiles (File::findFiles + File::ignoreHiddenFiles, "*.plume") != 0)
    {
        for (auto f : defaultDir.findChildFiles (File::findFiles + File::ignoreHiddenFiles, false, "*.plume"))
        {
            defaultPresets.add (new PlumePreset (f, PlumePreset::defaultPreset));
        }
    }
    
    // Adds user presets
    userPresets.clear();
    File userDir (getUserDirectory());
    
    if (userDir.exists() && userDir.getNumberOfChildFiles (File::findFiles + File::ignoreHiddenFiles, "*.plume") != 0)
    {
        for (auto f : userDir.findChildFiles (File::findFiles + File::ignoreHiddenFiles, false, "*.plume"))
        {
            userPresets.add (new PlumePreset (f));
        }
    }
}

int PresetHandler::getNumPresets()
{
    return (defaultPresets.size() + userPresets.size() /*+ communityPresets.size()*/);
}

int PresetHandler::getNumSearchedPresets()
{
    return (searchedPresets.size());
}

String PresetHandler::getTextForPresetId (int id)
{
    if (id < 0 || id >= getNumSearchedPresets()) return "-";
    
    return searchedPresets[id]->getName();
}

bool PresetHandler::isUserPreset (int id)
{
	return (searchedPresets[id]->presetType == PlumePreset::userPreset);
}

bool PresetHandler::canSavePreset()
{
	return (currentPreset.presetType && currentPreset.isValid());
}

XmlElement* PresetHandler::getPresetXmlToLoad (int selectedPreset)
{
	if (selectedPreset < 0 || selectedPreset > getNumSearchedPresets()) return nullptr;

	currentPreset = getPresetForId (selectedPreset);
	
	XmlDocument doc (searchedPresets[selectedPreset]->getFile());
	    
	if (XmlElement* elem = doc.getDocumentElement())
    {
        return elem;
    }
    else
    {
        DBG ("Failed to parse : " << doc.getLastParseError());
        return nullptr;
    }	
}

bool PresetHandler::savePreset (XmlElement& presetXml)
{
    if (!canSavePreset()) return false;

    // Tries to write the xml to the specified file
    if (getUserDirectory().getChildFile (currentPreset.getName() + ".plume").exists())
    {
        if (presetXml.writeToFile (getUserDirectory().getChildFile (currentPreset.getName() + ".plume"), String()))
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
        DBG ("Couldn't find preset file : " << getUserDirectory().getFullPathName() << currentPreset.getName() << ".plume");
        return false;
    }
}

bool PresetHandler::createNewUserPreset (String presetName, XmlElement& presetXml)
{
    if (presetName.isEmpty()) return false;
    
    presetName = File::createLegalFileName (presetName);
    
    // If the file is succesfully created, changes current preset and saves
    if (getUserDirectory().getChildFile (presetName + ".plume").create())
    {
        // Saves the current preset in case saving fails
        String formerPresetName = currentPreset.getName();
        int formerType = currentPreset.presetType;
        
        // Attempt to save preset
        currentPreset.setName (presetName);
		currentPreset.presetType = PlumePreset::userPreset;
		
        if (savePreset (presetXml))
        {
            currentPreset = PlumePreset (getUserDirectory().getChildFile (currentPreset.getName() + ".plume"));
            
            // checks if the file exists already
            for (auto* prst : userPresets)
            {
                if (prst->getFile().getFileNameWithoutExtension() == presetName) return false;
            }
            
            userPresets.add (new PlumePreset (getUserDirectory().getChildFile (presetName).withFileExtension("plume")));
            
            updateSearchedPresets();
            return true;
        }
        else
        {
            // restores to former values and deletes file
            currentPreset.setName (formerPresetName);
            currentPreset.presetType = formerType;
			getUserDirectory().getChildFile (presetName + ".plume").deleteFile();
            return false;
        }
    }
    
    return false;
}


bool PresetHandler::renamePreset (String newName, const int id)
{
    if (newName.isEmpty()) return false;
    
    newName = File::createLegalFileName (newName);
    File presetToRename = getUserDirectory().getChildFile (getTextForPresetId (id)).withFileExtension ("plume");
    
    if (presetToRename.exists())
    {
        if (ScopedPointer<XmlElement> presetXml = XmlDocument::parse (userPresets[id - defaultPresets.size()]->getFile()))
        {
            // Changes name of the preset for the main Xml tag
            presetXml->setTagName (newName.replace (" ", "::spc::"));
            
            // Writes the new xml to the old file
            if (presetXml->writeToFile (presetToRename, String()))
            {
                // Renames the file
                if (presetToRename.moveFileTo (presetToRename.getSiblingFile (newName).withFileExtension ("plume")))
                {
                    presetXml->deleteAllChildElements();
                    
                    // Changes the file in the array..
                    userPresets.set(id - defaultPresets.size(), new PlumePreset (getUserDirectory().getChildFile (newName)
                                                                                                   .withFileExtension ("plume")));
                    updateSearchedPresets();
                    return true;
                }
            }
            // Avoids memory leaks if the file wasn't written succesfully
            presetXml->deleteAllChildElements();
        }
    }
    
    return false;
}
    
void PresetHandler::resetPreset()
{
    DBG ("Preset Reset!");
    
    currentPreset = PlumePreset();
}

PlumePreset PresetHandler::getPresetForId(int id)
{
	if (id >= getNumSearchedPresets() || id < 0) return PlumePreset();
    
	return (*searchedPresets[id]);
}

bool PresetHandler::deletePresetForId (int id)
{
    if (id < 0 || id >= getNumSearchedPresets()) return false;
    
    if (currentPreset.getName() == getTextForPresetId (id))
    {
        resetPreset();
    }
    
    if (searchedPresets[id]->presetType == PlumePreset::userPreset)
    {
		searchedPresets[id]->getFile().deleteFile();
        userPresets.removeObject (searchedPresets[id]);
        searchedPresets.remove (id);
        return true;
    }
    
    DBG ("Failed to delete file " << getTextForPresetId (id));
    return false;
}

void PresetHandler::showPresetInExplorer (int id)
{
    if (id < 0 || id >= getNumPresets()) return;
        
    searchedPresets[id]->getFile().revealToUser();
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
    setUserDirectory (baseDir.getChildFile ("User/"));
    
    storePresets();
}

void PresetHandler::setSearchSettings (int type, int filter, String pluginName, String name)
{
    if (settings.presetType == type       &&
        settings.filterType == filter     &&
        settings.plugin     == pluginName &&
        settings.nameSearch == name)
    {
        return; //No update if nothing changed
    }
    
    // Sets the values and updates the searched presets list
    if (settings.presetType != type)
    {
        if (type > -1 && type < PlumePreset::numTypes) settings.presetType = type;
        else                                           settings.presetType = -1;
    }
    
    if (settings.filterType != filter)
    {
        if (filter > -1 && filter < PlumePreset::numFilters) settings.filterType = filter;
        else                                                 settings.filterType = -1;
    }
    
    if (settings.plugin     != pluginName) settings.plugin     = pluginName;
    if (settings.nameSearch != name)       settings.nameSearch = name;
    
    updateSearchedPresets();
}


void PresetHandler::setTypeSearchSetting (int type)
{
    if (settings.presetType == type)
    {
        return; //No update if nothing changed
    }
    
    // Sets the value and updates the searched presets list
    if (type > -1 && type < PlumePreset::numTypes) settings.presetType = type;
    else                                           settings.presetType = -1;
    
    updateSearchedPresets();
}

void PresetHandler::setFilterSearchSetting (int filter)
{
    if (settings.filterType == filter)
    {
        return; //No update if nothing changed
    }
    
    // Sets the value and updates the searched presets list
    if (filter > -1 && filter < PlumePreset::numFilters) settings.filterType = filter;
    else                                                 settings.filterType = -1;
    
    updateSearchedPresets();
}

void PresetHandler::setPluginSearchSetting (String pluginName)
{
    if (settings.plugin == pluginName)
    {
        return; //No update if nothing changed
    }
    
    // Sets the value and updates the searched presets list
    settings.plugin = pluginName;
    
    updateSearchedPresets();
}

void PresetHandler::setNameSearchSetting (String name)
{
    if (settings.nameSearch == name)
    {
        return; //No update if nothing changed
    }
    
    // Sets the value and updates the searched presets list
    settings.nameSearch = name;
    
    updateSearchedPresets();
}

void PresetHandler::updateSearchedPresets()
{
    searchedPresets.clear();
    
    if (settings.presetType == -1 || settings.presetType == PlumePreset::defaultPreset)
    {
        for (auto* preset : defaultPresets)
        {
            if (preset->matchesSettings (settings.filterType, settings.plugin, settings.nameSearch))
            {
                searchedPresets.add (preset);
            }
        }
    }
    
    if (settings.presetType == -1 || settings.presetType == PlumePreset::userPreset)
    {
        for (auto* preset : userPresets)
        {
            if (preset->matchesSettings (settings.filterType, settings.plugin, settings.nameSearch))
            {
                searchedPresets.add (preset);
            }
        }
    }
    
	/*
    if (settings.presetType == -1 || settings.presetType == PlumePreset::communityPresets)
    {
        for (auto* preset : communityPresets)
        {
            if (preset->matchesSettings (settings.filterType, settings.plugin, settings.nameSearch))
            {
                searchedPreset.add (preset);
            }
        }
    }
	*/
}
