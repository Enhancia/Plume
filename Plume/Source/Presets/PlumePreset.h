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
		brassWinds,
		strings,
		keys,
		lead,
		pad,
		percussive,
		sfx,
		other,
		
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
	PlumePreset (String name, File pathToPreset, PresetType pType =userPreset, FilterType category =other,
	             String auth ="", String ver ="1.0", String plug = "");
	PlumePreset (File pathToPreset, PresetType pType=userPreset, String pName = "", bool prioritizeFileInfo = false);
	PlumePreset();
	PlumePreset (const PlumePreset& other);
	
	~PlumePreset();

	PlumePreset& operator= (const PlumePreset& other) noexcept;
	bool operator== (const PlumePreset& other) noexcept;
    static int compareElements (PlumePreset& preset1, PlumePreset& preset2);
    static int compareElements (PlumePreset* preset1, PlumePreset* preset2);
    
	//==============================================================================
	static String getFilterTypeString (int filterTypeId);
	static void addPresetInfoXml (XmlElement& presetXml, String name, String author, String version,
                                  String plugin, int presetType, int filterType);
	
	//==============================================================================
	bool setFile (const File& newFile);
	File getFile() const;

	//==============================================================================
	bool isValid();
	void setName (const String newName);
	const String getName() const;
	const String getAuthor() const;
	const String getVersion() const;
	const int getFilter() const;
	const String getFilterString() const;
	const String getDescription() const;
	bool matchesSettings (int filter, String pluginName, String nameSearch);

	int presetType;

private:
	//==============================================================================
	bool hasInfoXml (File filePathToLookAt = File());
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
