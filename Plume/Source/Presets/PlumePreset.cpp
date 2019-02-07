/*
  ==============================================================================

    PlumePreset.cpp
    Created: 4 Feb 2019 5:12:32pm
    Author:  Alex

  ==============================================================================
*/

#include "PlumePreset.h"


//==============================================================================
PlumePreset::PlumePreset (String presetName, File pathToPreset, PresetType pType,
                          FilterType category, String auth, String ver)
{
    name = presetName;
    author = auth;
    version = ver;
    presetType = (int) pType;
    filterType = (int) category;
    
    if (pathToPreset.exists() && !name.isEmpty())
    {
        presetFile = File (pathToPreset);
        valid = true;
        
        setPresetInfoToFile();
    }
}

PlumePreset::PlumePreset (File pathToPreset, PresetType pType)
{
    name = "";
    author = (pType == defaultPreset) ? "Enhancia" : "";
    version = "1.0";
    presetType = (int) pType;
    filterType = custom;
     
    
    if (pathToPreset.exists())
    {
        presetFile = File (pathToPreset);
        name = presetFile.getFileNameWithoutExtension();
        valid = true;
        
        if (hasInfoXml())
        {
            loadPresetInfoFromFile();
			
			if (presetType != (int) pType)
			{
				presetType = int(pType);
				setPresetInfoToFile();
			}
        }
        else
        {
            setPresetInfoToFile();
        }
    }
}

PlumePreset::PlumePreset()
{
    name = "";
    author = "";
    version = "1.0";
    presetType = (int) PresetType::userPreset;
    filterType = (int) FilterType::custom;
}

PlumePreset::~PlumePreset()
{
}

PlumePreset::PlumePreset (PlumePreset& other)
{
	name = "";
	author = "";
	version = "1.0";
	presetType = (int)PresetType::userPreset;
	filterType = (int)FilterType::custom;

    File& f = other.getFile();
    loadPresetFromFile (f);
}

PlumePreset& PlumePreset::operator= (PlumePreset& other) noexcept
{
	File& f = other.getFile();
    loadPresetFromFile (f);
	return *this;
}

//==============================================================================
bool PlumePreset::setFile (const File& newFile)
{
    presetFile = File (newFile);
    
    if (presetFile.exists())
    {
        valid = true;
        return true;
    }
    
    valid = false;
    return false;
}

File& PlumePreset::getFile()
{
    if (valid) return presetFile;
    
    return File();
}

String PlumePreset::getFilterTypeString (int filterTypeId)
{
    switch (filterTypeId)
    {
        case arp:        return "arp";
		case bass:       return "bass";
		case harsh:      return "harsh";
		case keys:       return "keys";
		case lead:       return "lead";
		case pad:        return "pad";
		case percussion: return "percussion";
		case sfx:        return "sfx";
		default:         return "custom";
    }
}

bool PlumePreset::isValid()
{
	return valid;
}

bool PlumePreset::matchesSettings (int filter, String pluginName, String nameSearch)
{
    return (filter == -1 || filter == filterType)        &&
           (pluginName == "All" || pluginName == plugin) &&
           (nameSearch.isEmpty() || name.removeCharacters (" -_").containsIgnoreCase(nameSearch));
}

void PlumePreset::setName (const String newName)
{
    name = newName;
    valid = !name.isEmpty();
}

const String PlumePreset::getName()
{
    return name;
}

bool PlumePreset::hasInfoXml()
{
    ScopedPointer<XmlElement> xml = XmlDocument::parse (presetFile);
    
    if (xml != nullptr)
    {
        if (xml->getChildByName ("INFO") != nullptr)
        {
            xml = nullptr;
            return true;
        }
    }
    
    xml = nullptr;
        
    return false;
}

void PlumePreset::loadPresetInfoFromFile()
{
    if (hasInfoXml())
    {
		ScopedPointer<XmlElement> xml = XmlDocument::parse (presetFile);
		
		XmlElement* info = xml->getChildByName("INFO");
        
        if (info != nullptr)
        {
            author = info->getStringAttribute ("author", "");
            version = info->getStringAttribute ("version", "1.0");
            presetType = info->getIntAttribute ("type", 1);
            filterType = info->getIntAttribute ("filter", PlumePreset::FilterType::custom);
        }
        
        xml = nullptr;
    }
}

void PlumePreset::loadPresetFromFile (File& file)
{
    if (file.exists())
    {
        setFile (file);
        name = presetFile.getFileNameWithoutExtension();
        
        if (hasInfoXml())
        {
            loadPresetInfoFromFile();
        }
    }
}

void PlumePreset::setPresetInfoToFile()
{
    ScopedPointer<XmlElement> xml = XmlDocument::parse (presetFile);
    
    if (xml != nullptr)
    {
        auto info= new XmlElement ("INFO");
        
        info->setAttribute ("author", author);
        info->setAttribute ("version", version);
        info->setAttribute ("type", presetType);
        info->setAttribute ("filter", filterType);
        
        // Attempts to replace the info child if it already exists.
        // If it doesn't, simply adds it.
        if (!xml->replaceChildElement (xml->getChildByName ("INFO"), info))
        {
            xml->insertChildElement (info, 0);
        }
        
		if (presetType != PresetType::userPreset) presetFile.getParentDirectory().setReadOnly (false);

        xml->writeToFile (presetFile, String());

		if (presetType != PresetType::userPreset) presetFile.getParentDirectory().setReadOnly (true);
    }
    
    xml = nullptr;
}
