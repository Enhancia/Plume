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
		arp = 0,
		bass,
		harsh,
		keys,
		lead,
		pad,
		percussion,
		sfx,
		custom,
		
		numFilters
	};

	enum PresetType
	{
		defaultPreset = 0,
		userPreset,
		communityPreset
	};

	//==============================================================================
	PlumePreset (String name, File pathToPreset, PresetType pType =userPreset, FilterType category =custom,
	             String auth ="", String ver ="1.0");
	PlumePreset (File pathToPreset, PresetType pType=userPreset);
	PlumePreset();
	PlumePreset (PlumePreset& other);
	
	~PlumePreset();

	PlumePreset& operator= (PlumePreset& other) noexcept;
	//==============================================================================
	bool setFile(const File& newFile);
	File& getFile();
	String getFilterTypeString (int filterTypeId);

	//==============================================================================
	bool isValid();
	void setName (const String newName);
	const String getName();
	bool matchesSettings (int filter, String pluginName, String nameSearch);

	int presetType;

private:
	//==============================================================================
	bool hasInfoXml();
	void loadPresetInfoFromFile();
	void loadPresetFromFile (File& file);
	void setPresetInfoToFile();
	
	//==============================================================================
	File presetFile;
	int filterType;
	String name, author, plugin, version;
	bool valid = false;

	//==============================================================================
	JUCE_LEAK_DETECTOR (PlumePreset)
};