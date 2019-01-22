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
    //==============================================================================
    /**
     * \brief Constructor.
     */
    PresetHandler();
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
    int getNumPresets();
    
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
    bool deletePresetForId (int id);
    void resetPreset();
    void showPresetInExplorer (int id);
    
private:
    //==============================================================================
    void initialiseDirectories();
    
    //==============================================================================
    OwnedArray<File> defaultPresets;
    OwnedArray<File> userPresets;
    File defaultDir;
    File userDir;
    String currentPresetName;
    bool currentIsDefault = false;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetHandler)
};