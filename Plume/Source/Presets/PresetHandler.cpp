/*
  ==============================================================================

    PresetHandler.cpp
    Created: 9 Jan 2019 12:15:28pm
    Author:  Alex

  ==============================================================================
*/

#include "PresetHandler.h"

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
	if (newDir.isDirectory())
	{
        searchedPresets.clear();

        if (moveFiles) // A mess that creates a new User/ folder in new location then copies everything hopefully
        {
            File oldDir = getUserDirectory();
            File newDirUser = newDir;

            if (newDirUser.getFileName() != "User" && newDirUser.getChildFile ("User/").createDirectory().wasOk())
            {
                newDirUser = newDirUser.getChildFile ("User/");
            }
    
            // If both exist, moves the old dir's content to the new one
            if (oldDir.exists()  && newDir.exists()  &&
                oldDir != newDir &&  oldDir != newDirUser)
            {
                oldDir.moveFileTo (newDirUser);
    
                if (oldDir != newDirUser)
                {
                    if (!newDirUser.isAChildOf (oldDir))
                    {
                        oldDir.moveToTrash();
                    }

                    // If everything was succesful pdates userDir property and plumepr
                    userDirValue.setProperty(PLUME::treeId::value, newDirUser.getFullPathName(), nullptr);
                    savePresetDirectoryToFile();
                }
            }
        }
        else // Just updates userDir property and plumepr
        {
            userDirValue.setProperty(PLUME::treeId::value, newDir.getFullPathName(), nullptr);
            savePresetDirectoryToFile();
        }

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

PlumePreset PresetHandler::getCurrentPreset()
{
    return currentPreset;
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

const bool PresetHandler::currentPresetRequiresAuth()
{
    return (currentPreset.isValid() && currentPreset.presetType == PlumePreset::defaultPreset &&
            (currentPreset.matchesSettings (-1, "UVIWorkstationVSTx64", "") ||
             currentPreset.matchesSettings (-1, "FalconVSTx64", "") ||
             currentPreset.matchesSettings (-1, "UVIWorkstationVST", "") ||
             currentPreset.matchesSettings (-1, "UVIWorkstation", "")));
}

void PresetHandler::storePresets()
{
    const File userDir = getUserDirectory();

    Array<File> presetFoldersToSearch;
    presetFoldersToSearch.add (userDir);

    for (auto folder : defaultDirectories)
    {
        if (!(folder->isAChildOf(userDir)))
        {
            if (userDir.isAChildOf(*folder))
                presetFoldersToSearch.removeFirstMatchingValue (userDir);

            presetFoldersToSearch.addIfNotAlreadyThere(File(*folder));
        }
    }

    searchedPresets.clear();
    defaultPresets.clear();
    userPresets.clear();

    for (auto presetFolder : presetFoldersToSearch)
    {
        if (presetFolder.exists())
        {
            for (auto f : presetFolder.findChildFiles (File::findFiles + File::ignoreHiddenFiles,
                                                           true, "*.plume"))
            {
                PlumePreset tempPreset (f, PlumePreset::defaultPreset, "", true);

                if (tempPreset.presetType == PlumePreset::defaultPreset)
                {
                    defaultPresets.add (new PlumePreset (tempPreset));
                }
                else if (tempPreset.presetType == PlumePreset::userPreset)
                {
                    userPresets.add (new PlumePreset (tempPreset));
                }

            }
        }
    }

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

String PresetHandler::getDescriptionForPresetId (int id)
{
    if (id < 0 || id >= getNumSearchedPresets()) return "-";
    
    return searchedPresets[id]->getDescription();
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

std::unique_ptr<XmlElement> PresetHandler::getPresetXmlToLoad (int selectedPreset)
{
	if (selectedPreset < 0 || selectedPreset > getNumSearchedPresets()) return nullptr;

	currentPreset = getPresetForId (selectedPreset);
	
	XmlDocument doc (searchedPresets[selectedPreset]->getFile());
	    
	if (std::unique_ptr<XmlElement> elem = doc.getDocumentElement())
    {
        return elem;
    }
    else
    {
        DBG ("Failed to parse : " << doc.getLastParseError());
        return nullptr;
    }	
}

void PresetHandler::loadInfoFromTreeState (const ValueTree& treeState)
{
    if (treeState.isValid())
    {
        auto currentPresetTree = treeState.getChildWithName (PLUME::treeId::currentPreset);

        if (currentPresetTree.isValid())
        {
            using namespace PLUME::treeId;
            
            if (currentPresetTree.hasProperty (currentPresetName))
                currentPreset.setName (currentPresetTree.getProperty (currentPresetName).toString());
            
            if (currentPresetTree.hasProperty (currentPresetType))
                currentPreset.presetType = int (currentPresetTree.getProperty (currentPresetType));
            
            if (currentPresetTree.hasProperty (currentPresetFile))
                currentPreset.setFile (currentPresetTree.getProperty (currentPresetFile).toString());
        }
    }
}

void PresetHandler::saveInfoToTreeState (AudioProcessorValueTreeState& apvts)
{
    auto treeState = apvts.copyState();

    if (treeState.isValid())
    {
        auto currentPresetTree = treeState.getChildWithName (PLUME::treeId::general)
                                          .getChildWithName (PLUME::treeId::currentPreset);

        if (currentPresetTree.isValid() && currentPreset.isValid())
        {
            using namespace PLUME::treeId;
            
            currentPresetTree.setProperty (currentPresetName, currentPreset.getName(), nullptr);
            currentPresetTree.setProperty (currentPresetType, currentPreset.presetType, nullptr);
            currentPresetTree.setProperty (currentPresetFile, currentPreset.getFile().getFullPathName(), nullptr);

            apvts.replaceState (treeState);
        }
    }
}

bool PresetHandler::savePreset (XmlElement& presetXml, File fileToWriteTo)
{
    String fileString = File::createLegalFileName(presetXml.getChildByName("INFO")->getStringAttribute("name") + ".plume");

    if (!fileToWriteTo.exists())
    {
        fileToWriteTo = getUserDirectory().getChildFile (fileString);
    }

    // Tries to write the xml to the specified file
    if (fileToWriteTo.exists())
    {
        if (presetXml.writeToFile (fileToWriteTo, String()) &&
            PlumePreset (fileToWriteTo).isValid())
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
        DBG ("Couldn't find preset file : " << fileToWriteTo.getFullPathName());
        return false;
    }
}

bool PresetHandler::createNewOrOverwriteUserPreset (XmlElement& presetXml)
{
    if (XmlElement* info = presetXml.getChildByName ("INFO"))
    {
        const String presetName = info->getStringAttribute ("name");
        PLUME::log::writeToLog ("Creating new User preset : " + presetName, PLUME::log::presets);
        
        const String fileName = File::createLegalFileName (presetName);
        File newPresetFile;
        bool saveAs = false;
        int presetIdToSaveAs = -1;

        for (auto* prst : userPresets)
        {
            if (prst->getFile().getFileNameWithoutExtension() == fileName)
            {
                saveAs = true;
                newPresetFile = prst->getFile();
                presetIdToSaveAs = userPresets.indexOf (prst);
                break;
            }
        }

        if (!saveAs)
        {
            newPresetFile = getUserDirectory().getChildFile (fileName + ".plume");
            newPresetFile.create();
        }

        // If the file is succesfully created, changes current preset and saves
        if (newPresetFile.exists())
        {
            // Saves the current preset in case saving fails
            String formerPresetName = currentPreset.getName();
            int formerType = currentPreset.presetType;
    		File formerFile = currentPreset.getFile();
    		
            if (savePreset (presetXml, newPresetFile))
            {   
                if (saveAs)
                {
                    PlumePreset& presetThatWasSavedAs = *userPresets[presetIdToSaveAs];
                    presetThatWasSavedAs.setFile (newPresetFile); // sets the same file to update info
                    currentPreset = presetThatWasSavedAs;
                }
                else
                {
                    currentPreset = *userPresets.add (new PlumePreset (getUserDirectory().getChildFile (fileName).withFileExtension("plume")));
                }
                
                updateSearchedPresets();
                return true;
            }
            else
            {
                // restores to former values and deletes file
                currentPreset.setName (formerPresetName);
                currentPreset.presetType = formerType;
    			currentPreset.setFile (formerFile);
                return false;
            }
        }
    }
    
    return false;
}

bool PresetHandler::renamePreset (String newName, const int id)
{
    if (newName.isEmpty()) return false;
    
    String newFileName = File::createLegalFileName (newName);
    
    if (searchedPresets[id]->presetType == PlumePreset::userPreset && searchedPresets[id]->getFile().exists())
    {
        File f = searchedPresets[id]->getFile();
		std::unique_ptr<XmlElement> presetXml = XmlDocument::parse(f);

        if (presetXml != nullptr)
        {
            // Changes name of the preset for the main Xml tag
            if (XmlElement* info = presetXml->getChildByName ("INFO"))
            {
                info->setAttribute ("name", newName);
                
                // Writes the new xml to the old file
                if (presetXml->writeToFile (f, String()))
                {
                    // Renames the file
                    if (f.moveFileTo (f.getSiblingFile (newFileName).withFileExtension ("plume")))
                    {
                        presetXml->deleteAllChildElements();
                        
                        // Changes the file in the array..
                        userPresets.set (userPresets.indexOf (searchedPresets[id]),
                                         new PlumePreset (f.getSiblingFile(newFileName).withFileExtension("plume"), PlumePreset::userPreset));
                        updateSearchedPresets();
                        return true;
                    }
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

PlumePreset PresetHandler::getPresetForId (int id)
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
    //File defaultDirectories;
    defaultDirectories.add (new File (PLUME::file::defaultPresetDir));
    #if JUCE_WINDOWS
    defaultDirectories.add (new File (File::getSpecialLocation (File::userDocumentsDirectory)
                                                .getChildFile ("Enhancia/")
                                                .getChildFile ("Plume/")
                                                .getChildFile ("Presets/")));
    #endif

    loadPresetDirectoryFromFile();
    if (!getUserDirectory().exists())
    {
        #if JUCE_WINDOWS
        File f = File::getSpecialLocation (File::userDocumentsDirectory).getChildFile ("Enhancia/")
                                                                                 .getChildFile ("Plume/")
                                                                                 .getChildFile ("Presets/")
                                                                                 .getChildFile ("User/");
        #elif JUCE_MAC

        File f = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Audio/")
                                                                                       .getChildFile ("Presets/")
                                                                                       .getChildFile ("Enhancia/")
                                                                                       .getChildFile ("Plume/")
                                                                                       .getChildFile ("User/");
        #endif

        f.createDirectory();
        setUserDirectory (f, false);
    }

    storePresets();
}

void PresetHandler::setSearchSettings (int type, int filter, String pluginName, String pDescriptiveName, String name)
{
    if (settings.presetType         == type       &&
        settings.filterType         == filter     &&
        settings.plugin             == pluginName &&
        settings.descriptiveName    == pDescriptiveName &&
        settings.nameSearch         == name)
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

    if (settings.plugin != pluginName)
        settings.plugin = pluginName;
    if (settings.descriptiveName != pDescriptiveName)
        settings.descriptiveName = pDescriptiveName;
    if (settings.nameSearch != name)
        settings.nameSearch = name;

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

void PresetHandler::setPluginSearchSetting (String pDescriptiveName)
{
    if (settings.descriptiveName == pDescriptiveName)
    {
        return; //No update if nothing changed
    }
    
    // Sets the value and updates the searched presets list
    settings.descriptiveName = pDescriptiveName;
    
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

    PLUME::log::writeToLog ("Updating preset list",
                            PLUME::log::presets);
    
    if (settings.presetType == -1 || settings.presetType == PlumePreset::defaultPreset)
    {
        for (auto* preset : defaultPresets)
        {
            if (preset->matchesSettings (settings.filterType, settings.descriptiveName, settings.nameSearch))
            {
                searchedPresets.addSorted (settings, preset);
            }
        }
    }
    
    if (settings.presetType == -1 || settings.presetType == PlumePreset::userPreset)
    {
        for (auto* preset : userPresets)
        {
            if (preset->matchesSettings (settings.filterType, settings.descriptiveName, settings.nameSearch))
            {
                searchedPresets.addSorted (settings, preset);
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

void PresetHandler::deleteDirectoryArborescence (File& dir)
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
    using namespace PLUME::file;

    if (!presetDir.exists())
    {
        presetDir.create();
    }
    
	std::unique_ptr<XmlElement> presetDirXml = userDirValue.createXml();
    presetDirXml->writeToFile (presetDir, StringRef());
    presetDirXml->deleteAllChildElements();
}

void PresetHandler::loadPresetDirectoryFromFile()
{
    using namespace PLUME::file;
  
    if (presetDir.exists())
    {
		std::unique_ptr<XmlElement> presetDirXml = XmlDocument::parse (presetDir);
        
        if (presetDirXml != nullptr)
        {
            userDirValue = ValueTree::fromXml (*presetDirXml);
        }
        
        presetDirXml->deleteAllChildElements();
    }
}
