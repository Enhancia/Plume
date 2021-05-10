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
                          FilterType category, String auth, String ver, String plug)
                            : name (presetName), plugin (plug), author (auth), version (ver)
{
    presetType = (int) pType;
    filterType = (int) category;
    
    if (pathToPreset.exists() && !name.isEmpty())
    {
        presetFile = File (pathToPreset);
        valid = true;
        
        setPresetInfoToFile();
    }
}

PlumePreset::PlumePreset (File pathToPreset, PresetType pType, String pName)
                            : name (pName), author ((pType == defaultPreset) ? "Enhancia" : ""),
                                         plugin (""),
                                         version ("1.0")
{
    presetType = (int) pType;
    filterType = custom;
    
    if (pathToPreset.exists())
    {
        presetFile = File (pathToPreset);
        valid = true;
        
        if (hasInfoXml())
        {
            loadPresetInfoFromFile();
			
            bool overrideInfo = false;

			if (presetType != (int) pType)
			{
                presetType = int(pType);
                overrideInfo = true;
			}

            if (pName.isNotEmpty() && pName != name)
            {
				name = pName;
                overrideInfo = true;
            }

			if (overrideInfo) setPresetInfoToFile();
        }
        else
        {
            getPluginFromFile (pathToPreset);
            setPresetInfoToFile();
        }
    }
}

PlumePreset::PlumePreset() : name (""), author (""), plugin (""), version ("1.0")
{
    presetType = (int) PresetType::userPreset;
    filterType = (int) FilterType::custom;
}

PlumePreset::~PlumePreset()
{
}

PlumePreset::PlumePreset (const PlumePreset& other) : name (""), author (""), plugin (""), version ("1.0")

{
	presetType = (int)PresetType::userPreset;
	filterType = (int)FilterType::custom;

    File f = other.getFile();
    loadPresetFromFile (f);
}

PlumePreset& PlumePreset::operator= (const PlumePreset& other) noexcept
{
	File f = other.getFile();
    loadPresetFromFile (f);
	return *this;
}

bool PlumePreset::operator== (const PlumePreset& other) noexcept
{
    return (getFile() == other.getFile());
}

int PlumePreset::compareElements (PlumePreset& preset1, PlumePreset& preset2)
{
    return preset1.getName().compareNatural (preset2.getName());
}

int PlumePreset::compareElements (PlumePreset* preset1, PlumePreset* preset2)
{
    return preset1->getName().compareNatural (preset2->getName());
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

File PlumePreset::getFile() const
{
    if (valid) return presetFile;
    
    return File();
}

String PlumePreset::getFilterTypeString (int filterTypeId)
{
    switch (filterTypeId)
    {
        case arp:        return "Arp";
		case bass:       return "Bass";
		case harsh:      return "Harsh";
		case keys:       return "Keys";
		case lead:       return "Lead";
		case pad:        return "Pad";
		case percussion: return "Percussion";
		case sfx:        return "Sfx";
		default:         return "Custom";
    }
}

bool PlumePreset::isValid()
{
	return valid;
}

bool PlumePreset::matchesSettings (int filter, String pluginName, String nameSearch)
{
    return (filter == -1 || filter == filterType)         &&
           (pluginName.isEmpty() || pluginName == plugin) &&
           (nameSearch.isEmpty() || name.removeCharacters (" -_").containsIgnoreCase(nameSearch));
}

void PlumePreset::setName (const String newName)
{
    name = newName;
    setPresetInfoToFile();
    valid = presetFile.exists() && name.isNotEmpty();
}

const String PlumePreset::getName() const
{
    return name;
}

const String PlumePreset::getAuthor() const
{
    return author;
}

const String PlumePreset::getVersion() const
{
    return version;
}

const int PlumePreset::getFilter() const
{
    return filterType;
}

const String PlumePreset::getDescription() const
{
    return name + " - " + author + " (v" + version + ") :\n\n"
         + "  Plugin   : " + (plugin.isEmpty() ? "-" : plugin) + "\n"
         + "  Category : " + getFilterTypeString (filterType);
}

const String PlumePreset::getFilterString() const
{
    return getFilterTypeString (filterType);
}

bool PlumePreset::hasInfoXml (File fileToLookAt)
{
    if (fileToLookAt.getFullPathName().isEmpty()) fileToLookAt = presetFile;

    std::unique_ptr<XmlElement> xml = XmlDocument::parse (fileToLookAt);
    
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
		std::unique_ptr<XmlElement> xml = XmlDocument::parse (presetFile);
		
		XmlElement* info = xml->getChildByName("INFO");
        
        if (info != nullptr)
        {
            name = info->getStringAttribute ("name", "");
            author = info->getStringAttribute ("author", "");
            version = info->getStringAttribute ("version", "1.0");
            plugin = info->getStringAttribute ("plugin", "");
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
        if (hasInfoXml (file))
        {
            setFile (file);
            loadPresetInfoFromFile();
        }
    }
}

void PlumePreset::getPluginFromFile (File& file)
{
	std::unique_ptr<XmlElement> xml = XmlDocument::parse (file);
	if (xml == nullptr) return;

	XmlElement* wrap = xml->getChildByName ("WRAPPED_PLUGIN");
	
    if (wrap != nullptr)
    {
        if (wrap->getBoolAttribute ("hasWrappedPlugin", false))
        {
            if (wrap->getChildByName ("PLUGIN") != nullptr)
            {
                plugin = wrap->getChildByName ("PLUGIN")->getStringAttribute ("descriptiveName", "");
            }
        }
    }   
    
    xml = nullptr;
}

void PlumePreset::setPresetInfoToFile()
{
    std::unique_ptr<XmlElement> xml = XmlDocument::parse (presetFile);
    
    if (xml != nullptr)
    {
        auto info= new XmlElement ("INFO");
        
        info->setAttribute ("name", name);
        info->setAttribute ("author", author);
        info->setAttribute ("version", version);
        info->setAttribute ("plugin", plugin);
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

void PlumePreset::addPresetInfoXml (XmlElement& presetXml, String name,
                                                           String author,
                                                           String version,
                                                           String plugin,
                                                           int presetType,
                                                           int filterType)
{
    auto* info= new XmlElement ("INFO");
        
    info->setAttribute ("name", name);
    info->setAttribute ("author", author);
    info->setAttribute ("version", version);
    info->setAttribute ("plugin", plugin);
    info->setAttribute ("type", presetType);
    info->setAttribute ("filter", filterType);

    presetXml.insertChildElement (info, 0);
}
