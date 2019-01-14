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

class PresetHandler
{
public:
    //==============================================================================
    PresetHandler();
    ~PresetHandler();
    
    //==============================================================================
    void setUserDirectory (const File& newDir);
    String getCurrentPreset();
    int getNumPresets();
    String getTextForPresetId (int id);
    bool canSavePreset();
    
    //==============================================================================
    void storePresets();
    XmlElement* getPresetXmlToLoad (int selectedPreset);
    void savePreset (XmlElement& presetXml);
    void createNewUserPreset (String presetName);
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