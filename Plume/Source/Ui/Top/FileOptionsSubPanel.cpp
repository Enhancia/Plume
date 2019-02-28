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

    addLabelRow ("User plugin path", "pluginDir", processor.getWrapper()
                                                           .getCustomDirectory (0));
    addRow ("Scan Plugins", new ScannerComponent (processor), 20);



    // Preset section
    addSeparatorRow ("Preset Sources");

    addLabelRow ("User presets path", "presetDir", processor.getPresetHandler()
                                                            .getUserDirectory()
                                                            .getFullPathName());
    
}

FileOptionsSubPanel::~FileOptionsSubPanel()
{
}

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