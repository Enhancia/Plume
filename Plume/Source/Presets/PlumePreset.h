/*
  ==============================================================================

    PlumePreset.h
    Created: 4 Feb 2019 5:12:32pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/PlumeCommon.h"

class PlumePreset
{
public:
	//==============================================================================
	enum FilterType
	{
		arp = 0,
		bass,
		custom,
		harsh,
		keys,
		lead,
		pad,
		percussion,
		sfx,
		
		numFilters
	};

	enum PresetType
	{
		defaultPreset = 0,
		userPreset,
		communityPreset,

		numTypes
	};

	//==============================================================================
	PlumePreset (String name, File pathToPreset, PresetType pType =userPreset, FilterType category =custom,
	             String auth ="", String ver ="1.0", String plug = "");
	PlumePreset (File pathToPreset, PresetType pType=userPreset);
	PlumePreset();
	PlumePreset (const PlumePreset& other);
	
	~PlumePreset();

	PlumePreset& operator= (const PlumePreset& other) noexcept;
	bool operator== (const PlumePreset& other) noexcept;
	//==============================================================================
	static String getFilterTypeString (int filterTypeId);
	static void addPresetInfoXml (XmlElement& presetXml, String author, String version,
                                  String plugin, int presetType, int filterType);
	
	//==============================================================================
	bool setFile (const File& newFile);
	File getFile() const;

	//==============================================================================
	bool isValid();
	void setName (const String newName);
	const String getName();
	const String getFilterString();
	bool matchesSettings (int filter, String pluginName, String nameSearch);

	int presetType;

private:
	//==============================================================================
	bool hasInfoXml();
	void loadPresetInfoFromFile();
	void loadPresetFromFile (File& file);
	void getPluginFromFile (File& file);
	void setPresetInfoToFile();
	
	//==============================================================================
	File presetFile;
	int filterType;
	String name, author, plugin, version;
	bool valid = false;

	//==============================================================================
	JUCE_LEAK_DETECTOR (PlumePreset)
};