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
class PresetHandler
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
    String getCurrentPreset();
    
    /**
     * \brief Getter for the total number of presets.
     *
     * \returns Total number of presets, regardless of them being default or user presets.
     */
    int getNumPresets();
    
    /**
     * \brief Getter for the preset currently in use.
     *
     * \returns The name of the preset file which is currently used.
     */
    String getTextForPresetId (int id);
    
    /**
     * \brief Getter for the preset currently in use.
     *
     * \returns The name of the preset file which is currently used.
     */
    bool canSavePreset();
    
    //==============================================================================
    void storePresets();
    XmlElement* getPresetXmlToLoad (int selectedPreset);
    bool savePreset (XmlElement& presetXml);
    bool createNewUserPreset (String presetName, XmlElement& presetXml);
    bool renamePreset (const String& newName);
    void resetPreset();
    
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
};