/*
  ==============================================================================

    FileOptionsSubPanel.cpp
    Created: 25 Feb 2019 11:31:12am
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "FileOptionsSubPanel.h"

//==============================================================================
FileOptionsSubPanel::FileOptionsSubPanel (PlumeProcessor& proc)   : processor (proc)
{
    // Plugin section
    addSeparatorRow ("Plugin Sources");

    addScannerRow ("User plugin path", "pluginDir", TRANS("Select your custom plugin directory:"),
                   File::getSpecialLocation (File::userHomeDirectory), String(),
                   processor.getWrapper().getCustomDirectory (0), true);

    addRow ("Scan Plugins", new ScannerComponent (processor), 20);



    // Preset section
    addSeparatorRow ("Preset Sources");

    addScannerRow ("User preset path", "presetDir", TRANS("Select your custom preset directory:"),
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
            processor.getPresetHandler().storePresets();
            
            if (auto* sideBar = dynamic_cast<PlumeComponent*> (getParentComponent() // TabbedPanel
                                                                 ->getParentComponent() // Options Panel
                                                                 ->getParentComponent() // Editor
                                                                 ->findChildWithID ("sideBar")))
            {
                sideBar->update();
            }
        }
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