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
void PresetHandler::setUserDirectory (const File& newDir, bool moveFiles)
{
    searchedPresets.clear();

    File oldDir = getUserDirectory();

	if (newDir.isDirectory())
	{
		File newDirUser = newDir;
		if (newDirUser.getFileName() != "User" && newDirUser.getChildFile ("User").createDirectory().wasOk())
		{
			newDirUser = newDirUser.getChildFile ("User");
		}

		// If both exist, moves the old dir's content to the new one
		if (moveFiles && oldDir.exists()  &&
                         newDir.exists()  &&
                         oldDir != newDir &&
                         oldDir != newDirUser)
		{
		    oldDir.moveFileTo (newDirUser);
		}

        if (oldDir != newDirUser)
        {
            deleteArborescence(oldDir);
			/*
            if (!newDirUser.isAChildOf (oldDir))
            {
                oldDir.deleteRecursively();
            }
			*/
        }

		userDirValue.setProperty(PLUME::treeId::value, newDirUser.getFullPathName(), nullptr);
		createDirectoryArborescence (newDirUser);
		savePresetDirectoryToFile();

		storePresets();
	}
}

File PresetHandler::getUserDirectory()
{
    return File (userDirValue.getProperty (PLUME::treeId::value, var("")).toString());
}

String PresetHandler::getCurrentPresetName()
{
    return currentPreset.isValid() ? currentPreset.getName() : "-"; //.replaceCharacter (' ', '_');
}

int PresetHandler::getCurrentPresetIdInSearchList()
{
    if (currentPreset.isValid())
    {
        for (int presetId=0; presetId < getNumSearchedPresets(); presetId++)
        {
            if (currentPreset == *searchedPresets[presetId]) return presetId;
        }
    }

    return -1;
}

void PresetHandler::storePresets()
{
    searchedPresets.clear();

    // Adds default presets
    defaultPresets.clear();
    DBG (defaultDir.getNumberOfChildFiles (File::findFiles + File::ignoreHiddenFiles));

    if (defaultDir.exists() /*&& defaultDir.containsSubDirectories()*/)
    {
        for (auto f : defaultDir.findChildFiles (File::findFiles + File::ignoreHiddenFiles, true, "*.plume"))
        {
            defaultPresets.add (new PlumePreset (f, PlumePreset::defaultPreset));
        }
    }
    
    // Adds user presets
    userPresets.clear();
    File userDir (getUserDirectory());
    DBG (userDir.getNumberOfChildFiles (File::findFiles + File::ignoreHiddenFiles));
    
    if (userDir.exists() && userDir.containsSubDirectories())
    {
        for (auto f : userDir.findChildFiles (File::findFiles + File::ignoreHiddenFiles, true, "*.plume"))
        {
            userPresets.add (new PlumePreset (f));
        }
    }

    /*
    // Adds community presets
    communityPresets.clear();
    if (commuDir.exists() && commuDir.containsSubDirectories())
    {
        for (auto f : commuDir.findChildFiles (File::findFiles + File::ignoreHiddenFiles, true, "*.plume"))
        {
            communityPresets.add (new PlumePreset (f));
        }
    }
    */

    updateSearchedPresets();
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

String PresetHandler::getFilterTextForPresetId (const int id)
{
    if (id < 0 || id >= getNumSearchedPresets()) return "Custom";
    
    return searchedPresets[id]->getFilterString();
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
	    
	if (std::unique_ptr<XmlElement> elem = doc.getDocumentElement())
    {
        return new XmlElement (*elem);
    }
    else
    {
        DBG ("Failed to parse : " << doc.getLastParseError());
        return nullptr;
    }	
}

bool PresetHandler::savePreset (XmlElement& presetXml, String filterType)
{
    if (!canSavePreset()) return false;

    // Tries to write the xml to the specified file
    if (getUserDirectory().getChildFile (filterType)
                          .getChildFile (currentPreset.getName() + ".plume").exists())
    {
        if (presetXml.writeToFile (getUserDirectory().getChildFile (filterType)
                                                     .getChildFile (currentPreset.getName() + ".plume"), String()))
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
    String category;

    if (XmlElement* info = presetXml.getChildByName ("INFO"))
    {
        category = PlumePreset::getFilterTypeString (info->getIntAttribute ("filter"));
    }
    
    // If the file is succesfully created, changes current preset and saves
    if (getUserDirectory().getChildFile (category).getChildFile (presetName + ".plume").create())
    {
        // Saves the current preset in case saving fails
        String formerPresetName = currentPreset.getName();
        int formerType = currentPreset.presetType;
		File formerFile = currentPreset.getFile();
        
        // Attempt to save preset
        currentPreset.setName (presetName);
		currentPreset.presetType = PlumePreset::userPreset;
		currentPreset.setFile (getUserDirectory().getChildFile(category).getChildFile (presetName + ".plume"));
		
        if (savePreset (presetXml, category))
        {
            currentPreset = PlumePreset (getUserDirectory().getChildFile (category)
                                                           .getChildFile (currentPreset.getName() + ".plume"));
            
            // checks if the file exists already
            for (auto* prst : userPresets)
            {
                if (prst->getFile().getFileNameWithoutExtension() == presetName) return false;
            }
            
            userPresets.add (new PlumePreset (getUserDirectory().getChildFile (category)
                                                                .getChildFile (presetName).withFileExtension("plume")));
            
            updateSearchedPresets();
            return true;
        }
        else
        {
            // restores to former values and deletes file
            currentPreset.setName (formerPresetName);
            currentPreset.presetType = formerType;
			currentPreset.setFile (formerFile);
			getUserDirectory().getChildFile (category).getChildFile (presetName ).withFileExtension("plume")
                                                                                 .deleteFile();
            return false;
        }
    }
    
    return false;
}

bool PresetHandler::renamePreset (String newName, const int id)
{
    if (newName.isEmpty()) return false;
    
    newName = File::createLegalFileName (newName);
    
    if (searchedPresets[id]->presetType == PlumePreset::userPreset && searchedPresets[id]->getFile().exists())
    {
        File f = searchedPresets[id]->getFile();
		std::unique_ptr<XmlElement> presetXml = XmlDocument::parse(f);

        if (presetXml != nullptr)
        {
            // Changes name of the preset for the main Xml tag
            presetXml->setTagName (newName.replace (" ", "_"));
            
            // Writes the new xml to the old file
            if (presetXml->writeToFile (f, String()))
            {
                // Renames the file
                if (f.moveFileTo (f.getSiblingFile (newName).withFileExtension ("plume")))
                {
                    presetXml->deleteAllChildElements();
                    
                    // Changes the file in the array..
                    userPresets.set (userPresets.indexOf (searchedPresets[id]),
                                     new PlumePreset (f.getSiblingFile(newName).withFileExtension("plume"), PlumePreset::userPreset));
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
    //File defaultDir;
    DBG ("Plume " + String(JucePlugin_VersionString));
  #if JUCE_WINDOWS
    defaultDir = File::getSpecialLocation (File::globalApplicationsDirectoryX86).getChildFile ("Enhancia/")
                                                                              .getChildFile ("Plume " +
                                                                                             String(JucePlugin_VersionString))
                                                                              .getChildFile ("Default_Presets/");

    loadPresetDirectoryFromFile();
    if (!getUserDirectory().exists())
    {
        setUserDirectory (File::getSpecialLocation (File::userDocumentsDirectory).getChildFile ("Enhancia/")
                                                                                 .getChildFile ("Plume/")
                                                                                 .getChildFile ("Presets/")
                                                                                 .getChildFile ("User/"), false);
    }

  #elif JUCE_MAC
    defaultDir = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Audio/")
                                                                              .getChildFile ("Presets/")
                                                                              .getChildFile ("Enhancia/")
                                                                              .getChildFile ("Plume/")
                                                                              .getChildFile ("Default/");
    //createDirectoryArborescence (defaultDir);

    loadPresetDirectoryFromFile();
    setUserDirectory (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/")
                                                                                       .getChildFile ("Plume/")
                                                                                       .getChildFile ("Presets/")
                                                                                       .getChildFile ("User/"), false);
  #else
    return; //Should only compile on win or mac
  #endif

    //createDirectoryArborescence (defaultDir);
    
    File f = getUserDirectory();
    createDirectoryArborescence (f);

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
    name = name.removeCharacters (" -_");
    
    if (settings.nameSearch == name)
    {
        return; //No update if nothing changed
    }
    
    // Sets the value and updates the searched presets list
    settings.nameSearch = name;
    
    updateSearchedPresets();
}

const PresetHandler::PresetSearchSettings PresetHandler::getCurrentSettings()
{
    return settings;
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

void PresetHandler::createDirectoryArborescence (File& dir)
{
    for (int i=0; i<PlumePreset::numFilters; i++)
    {
        File f (dir.getChildFile (PlumePreset::getFilterTypeString (i)));

        if (!f.exists())
        {
            f.createDirectory();
        }
    }
}

void PresetHandler::deleteArborescence (File& dir)
{
    for (int i=0; i<PlumePreset::numFilters; i++)
    {
        File f (dir.getChildFile (PlumePreset::getFilterTypeString (i)));

        if (f.exists())
        {
            f.deleteRecursively();
        }
    }
}

void PresetHandler::savePresetDirectoryToFile()
{
    // Create file if it doesn't exist yet
    File presetDirFile;
    
  #if JUCE_WINDOWS
    presetDirFile = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/")
                                                                                  .getChildFile ("Plume/");
  #elif JUCE_MAC
    presetDirFile = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Application Support/")
                                                                                  .getChildFile ("Plume/");
  #endif
  
    presetDirFile = presetDirFile.getChildFile ("plumepr.cfg");
  
    if (!presetDirFile.exists())
    {
        presetDirFile.create();
    }
    
	std::unique_ptr<XmlElement> presetDirXml = userDirValue.createXml();
    presetDirXml->writeToFile (presetDirFile, StringRef());
    presetDirXml->deleteAllChildElements();
}

void PresetHandler::loadPresetDirectoryFromFile()
{
    File presetDirFile;
    
  #if JUCE_WINDOWS
    presetDirFile = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/")
                                                                                  .getChildFile ("Plume/");
  #elif JUCE_MAC
    presetDirFile = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Application Support/")
                                                                                  .getChildFile ("Plume/");
  #endif
  
    presetDirFile = presetDirFile.getChildFile ("plumepr.cfg");
  
    if (presetDirFile.exists())
    {
		std::unique_ptr<XmlElement> presetDirXml = XmlDocument::parse (presetDirFile);
        
        if (presetDirXml != nullptr)
        {
            userDirValue = ValueTree::fromXml (*presetDirXml);
        }
        
        presetDirXml->deleteAllChildElements();
    }
}