/*
  ==============================================================================

    PresetHandler.h
    Created: 9 Jan 2019 12:15:28pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"

#include "PlumePreset.h"

/**
 *  \class PresetHandler PresetHandler.h
 *
 *  \brief Plume's preset handler object.
 *
 *  This object handles most of the preset operations. It is used in conjunction with
 *  the PresetComponent from the editor to allow saving and creating preset files.
 *
 */
class PresetHandler : private ChangeBroadcaster
{
public:

    struct presetSearchSettings
    {
        presetSearchSettings (int type = -1, int filter = 7, String pluginName = "All", String name = "")
        {
            presetType = type;
            filterType = filter;
            plugin = pluginName;
            nameSearch = name;
        }
        int presetType;
        int filterType;
        String plugin = "All";
        String nameSearch = "";
    };
    
    //==============================================================================
    /**
     * \brief Constructor.
     */
    PresetHandler (ValueTree presetDirValue);
    /**
     * \brief Destructor.
     */
    ~PresetHandler();
    
    //==============================================================================
    /**
     * \brief Setter for the user directory.
     *
     * Call this method to change the directory that should be used for user created
     * presets.
     *
     * \param newDir the directory that should be used.
     */  
    void setUserDirectory (const File& newDir);
    
    /**
     * \brief Getter for the user directory.
     *
     * \returns The user preset directory.
     */  
    File getUserDirectory();
    
    /**
     * \brief Getter for the preset currently in use.
     *
     * \returns The name of the preset file which is currently used.
     */
    String getCurrentPresetName();
    
    /**
     * \brief Getter for the total number of presets.
     *
     * \returns Total number of presets, regardless of them being default or user presets.
     */
    int getNumPresets ();
    
    /**
     * \brief Getter for the number of presets that match the search.
     *
     * \returns Number of presets that matches the request. Used fot displaying the preset list.
     */
    int getNumSearchedPresets ();
    
    /**
     * \brief Getter for the name of a specified preset number.
     *
     * \returns The name of the preset file corresponding to the id.
     */
    String getTextForPresetId (int id);
    
    /**
     * \brief Helper method that tells is the specified preset is from the user.
     *
     * \returns True is the preset is user made, and therefore can be renamed and deleted.
     */
    bool isUserPreset (int id);
    
    /**
     * \brief Helper method that tells is the current preset is can be saved by the user.
     *
     * \returns True is the user is allowed to save the current preset.
     */
    bool canSavePreset();
    
    //==============================================================================
    void storePresets();
    XmlElement* getPresetXmlToLoad (int selectedPreset);
    bool savePreset (XmlElement& presetXml);
    bool createNewUserPreset (String presetName, XmlElement& presetXml);
    bool renamePreset (String newName, const int id);
	PlumePreset getPresetForId (const int id);
    bool deletePresetForId (int id);
    void resetPreset();
    void showPresetInExplorer (int id);
    
    //==============================================================================
    void setSearchSettings (int type, int filter, String pluginName, String name);
    void setTypeSearchSetting (PlumePreset::PresetType type);
    void setFilterSearchSetting (PlumePreset::FilterType filter);
    void setPluginSearchSetting (String pluginName);
    void setNameSearchSetting (String name);
    
private:
    //==============================================================================
    void initialiseDirectories();
    //void createDirectoryArborescence (File& dir);
    //void createAllSubdirectories();
    
    //==============================================================================
    void updateSearchedPresets();
    
    //==============================================================================
    OwnedArray<PlumePreset> defaultPresets;
    OwnedArray<PlumePreset> userPresets;
    
    File defaultDir;
    ValueTree userDirValue;
    
    PlumePreset currentPreset;
    bool currentIsDefault = false;
    
    //==============================================================================
    Array<PlumePreset*> searchedPresets;
    presetSearchSettings settings;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetHandler)
};