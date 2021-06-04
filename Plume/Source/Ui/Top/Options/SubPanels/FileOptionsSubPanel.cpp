/*
  ==============================================================================

    FileOptionsSubPanel.cpp
    Created: 25 Feb 2019 11:31:12am
    Author:  Alex

  ==============================================================================
*/

#include "FileOptionsSubPanel.h"

//==============================================================================
FileOptionsSubPanel::FileOptionsSubPanel (PlumeProcessor& proc)   : processor (proc)
{
    // Plugin section
    addSeparatorRow ("Plugin Sources");
    
    #if JUCE_WINDOWS
    addRow ("Scan Plugins (hold alt to force rescan)", new ScannerComponent (processor, 4*PLUME::UI::SUBPANEL_ROW_HEIGHT), 20);
    #elif JUCE_MAC
    addRow ("Scan Plugins (hold ctrl to force rescan)", new ScannerComponent (processor, 4*PLUME::UI::SUBPANEL_ROW_HEIGHT), 20);
    #endif

    addToggleRow ("Use System Plugin Folder", "sysT", processor.getWrapper().usesDefaultPaths());
    addToggleRow ("Use Custom Plugin Folder", "cusT", processor.getWrapper().usesCustomPaths());

    addScannerRow ("Custom Plugin Folder", "pluginDir", TRANS("Select your custom plugin directory:"),
                   File::getSpecialLocation (File::userHomeDirectory), String(),
                   processor.getWrapper().getCustomDirectory (0), true);

  #if JUCE_MAC
    addToggleRow ("Use Audio Units", "auT", processor.getWrapper().usesAudioUnits());
  #endif


    // Preset section
    addSeparatorRow ("Preset Sources");

    addButtonRow ("Scan Presets", "prscanB", "Scan", "Scan Finished !");
    addScannerRow ("User Presets Folder", "presetDir", TRANS("Select your custom preset directory:"),
                   File::getSpecialLocation (File::userHomeDirectory), String(),
                   processor.getPresetHandler().getUserDirectory().getFullPathName(),
                   true);
}

FileOptionsSubPanel::~FileOptionsSubPanel()
{
}

void FileOptionsSubPanel::fileScanned (const String& scannerID, const File& fileThatWasScanned)
{
    if (scannerID == "pluginDir")
    {
        if (fileThatWasScanned.isDirectory())
        {
            processor.getWrapper().addCustomDirectory (fileThatWasScanned);
        }
    }
    else if (scannerID == "presetDir")
    {
        if (fileThatWasScanned.isDirectory())
        {
            processor.getPresetHandler().setUserDirectory (fileThatWasScanned);
        }
    } 
}

void FileOptionsSubPanel::buttonClicked (Button* bttn)
{
    if (bttn->getComponentID() == "prscanB")
    {
        processor.getPresetHandler().storePresets();
            
        if (auto* presetComp = dynamic_cast<PlumeComponent*> (getParentComponent() // TabbedPanel
                                                             ->getParentComponent() // Options Panel
                                                             ->getParentComponent() // Editor
                                                             ->findChildWithID ("sideBar")
                                                             ->findChildWithID ("presetComponent")))
        {
            presetComp->update();
        }
    }

    else if (bttn->getComponentID() == "sysT")
    {
        processor.getWrapper().setDefaultPathUsage (bttn->getToggleState());
    }

    else if (bttn->getComponentID() == "cusT")
    {
        processor.getWrapper().setCustomPathUsage (bttn->getToggleState());
    }

    else if (bttn->getComponentID() == "auT")
    {
        processor.getWrapper().getScanner().setPluginFormats (true, true, bttn->getToggleState());
    }
}