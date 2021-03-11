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

    addRow ("Rescan Plugins", new ScannerComponent (processor, 4*PLUME::UI::SUBPANEL_ROW_HEIGHT), 20);
    addToggleRow ("Use System Plugin Folder", "sysT", true);
    addToggleRow ("Use Custom Plugin Folder", "cusT", !processor.getWrapper().getCustomDirectory (0).isEmpty());

  #if JUCE_MAC
    addToggleRow ("Use Audio Units", "auT", true);
  #endif

    addScannerRow ("Custom Plugin Folder", "pluginDir", TRANS("Select your custom plugin directory:"),
                   File::getSpecialLocation (File::userHomeDirectory), String(),
                   processor.getWrapper().getCustomDirectory (0), true);



    // Preset section
    addSeparatorRow ("Preset Sources");

    addButtonRow ("Rescan Presets", "prscanB", "Scan", "Scan Finished !");
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
        processor.getWrapper().setAuUsage (bttn->getToggleState());
    }
}
/*
void FileOptionsSubPanel::labelTextChanged (Label* lbl)
{
    // Preset Directory Label
    if (lbl->getComponentID() == "presetDir")
    {
        if (File::isAbsolutePath (lbl->getText()) && File (lbl->getText()).exists())
        {
            processor.getPresetHandler().setUserDirectory (lbl->getText());
            processor.getPresetHandler().storePresets();
            
            if (auto* sideBar = dynamic_cast<PlumeComponent*> (getParentComponent()->getParentComponent()
                                                                                   ->findChildWithID ("sideBar")))
            {
                sideBar->update();
            }
        }
        else
        {
            lbl->setText (processor.getPresetHandler().getUserDirectory().getFullPathName(), dontSendNotification);
        }
    }
    
    // Plugin Directory Label
    if (lbl->getComponentID() == "pluginDir")
    {
        if (File::isAbsolutePath (lbl->getText()) && File (lbl->getText()).exists())
        {
            processor.getWrapper().addCustomDirectory (lbl->getText());
        }
        else if (lbl->getText() == "")
        {
            processor.getWrapper().clearCustomDirectories();
        }
        else
        {
            lbl->setText (processor.getWrapper().getCustomDirectory (0), dontSendNotification);
        }
    }
}
*/