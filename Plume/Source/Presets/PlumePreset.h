/*
  ==============================================================================

    PlumePreset.h
    Created: 4 Feb 2019 5:12:32pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"

class PlumePreset
{
public:
    //==============================================================================
    enum FilterType
    {
        arp =0,
        bass,
        harsh,
        keys,
        lead,
        pad,
        percussion,
        sfx
    };
    
    enum PresetType
    {
        defaultPreset=0,
        userPreset,
        communityPreset
    };
    
    //==============================================================================
    PlumePreset (String name, File pathToPreset, PresetType pType, FilterType category);
    ~PlumePreset();
    
    //==============================================================================
    bool setFile();
    
    //==============================================================================
    bool isValid();
    
private:
    //==============================================================================
    File presetFile;
    int presetType, filterType;
    string name, plugin, author;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumePreset)
}