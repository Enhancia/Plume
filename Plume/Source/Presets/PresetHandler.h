/*
  ==============================================================================

    PresetHandler.h
    Created: 9 Jan 2019 12:15:28pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/PlumeCommon.h"

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

    struct PresetSearchSettings
    {
        enum SortMethod
        {
            alphabetical =0,
            alphabeticalReversed
        };

		PresetSearchSettings(int type = -1, int filter = -1, String pluginName = String(), String name = String())
			: presetType (type), filterType (filter), plugin (pluginName), nameSearch (name)
        {
        }
        
	    PresetSearchSettings& operator= (const PresetSearchSettings& other) noexcept
        {
            this->presetType = other.presetType;
            this->filterType = other.filterType;
            this->plugin     = other.plugin;
            this->nameSearch = other.nameSearch;
            
            return *this;
        }
        
        const int compareElements (PlumePreset* preset1, PlumePreset* preset2) const
        {
            switch (sortMethod)
            {
                case alphabetical:
                    return preset1->getName().compareNatural (preset2->getName());
                case alphabeticalReversed:
                    return preset2->getName().compareNatural (preset1->getName());
            }

            return 0;
        }
        
        int presetType;
        int filterType;
        String plugin;
        String nameSearch;
        SortMethod sortMethod = alphabetical;
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
    void setUserDirectory (const File& newDir, bool moveFiles = false);
    
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

    PlumePreset getCurrentPreset();
    const bool currentPresetRequiresAuth();

    /**
     * \brief Getter for the id of the preset currently in use.
     *
     * \returns The id for the preset which is currently used, relative to the list of searched presets.
     */
    int getCurrentPresetIdInSearchList();
    
    /**
     * \brief Getter for the total number of presets.
     *
     * \returns Total number of presets, regardless of them being default or user presets.
     */
    int getNumPresets();
    
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
     * \brief Getter for the description of a specified preset number.
     *
     * \returns The name of the preset file corresponding to the id.
     */
    String getDescriptionForPresetId (int id);
    
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
    
    String getFilterTextForPresetId (const int id);
    void loadInfoFromTreeState (const ValueTree& treeState);
    //void saveInfoToTreeState (ValueTree& treeState);
    void saveInfoToTreeState (AudioProcessorValueTreeState& apvts);

    //==============================================================================
    void storePresets();
    std::unique_ptr<XmlElement> getPresetXmlToLoad (int selectedPreset);
    bool savePreset (XmlElement& presetXml, File fileToWriteTo = File());
    bool createNewOrOverwriteUserPreset (XmlElement& presetXml);
    bool renamePreset (String newName, const int id);
	PlumePreset getPresetForId (const int id);
    bool deletePresetForId (int id);
    void resetPreset();
    void showPresetInExplorer (int id);
    void savePresetDirectoryToFile();
    void loadPresetDirectoryFromFile();

    //==============================================================================
    void setSearchSettings (int type, int filter, String pluginName, String name);
    void setTypeSearchSetting (int type);
    void setFilterSearchSetting (int filter);
    void setPluginSearchSetting (String pluginName);
    void setNameSearchSetting (String name);
    
    const PresetSearchSettings getCurrentSettings();
    
private:
    //==============================================================================
    void initialiseDirectories();
    void createDirectoryArborescence (File& dir);
    void deleteDirectoryArborescence (File& dir);
    //void createAllSubdirectories();
    
    //==============================================================================
    void updateSearchedPresets();
    
    //==============================================================================
    OwnedArray<PlumePreset> defaultPresets;
    OwnedArray<PlumePreset> userPresets;
    
    OwnedArray<File> defaultDirectories;
    ValueTree userDirValue;
    
    PlumePreset currentPreset;
    bool currentIsDefault = false;
    
    //==============================================================================
    Array<PlumePreset*> searchedPresets;
    PresetSearchSettings settings;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetHandler)
};