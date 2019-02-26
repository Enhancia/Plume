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
    // Labels
    addAndMakeVisible (presetDirLabel = new Label ("Preset Directory Label", processor.getPresetHandler()
                                                                                      .getUserDirectory()
                                                                                      .getFullPathName()));
    presetDirLabel->setEditable (true, false, true);                                                                           
    presetDirLabel->addListener (this);
                                                                                      
    addAndMakeVisible (pluginDirLabel = new Label ("Plugin Directory Label", processor.getWrapper()
                                                                                      .getCustomDirectory (0)));
    pluginDirLabel->setEditable (true, false, true);
    pluginDirLabel->addListener (this);
    
    // Scanner
    addAndMakeVisible (scanner = new ScannerComponent (processor));
    
    // mail
    addAndMakeVisible (mailButton = new TextButton ("Mail Button"));
    mailButton->setButtonText ("Report");
    mailButton->addListener (this);
}

FileOptionsSubPanel::~FileOptionsSubPanel()
{
    scanButton = nullptr;
    scanner = nullptr;
    mailButton->removeListener (this);
    mailButton = nullptr;
}

void FileOptionsSubPanel::paint (Graphics& g)
{
    // Main Text
    auto area = getLocalBounds();
    
    g.setColour (UI::currentTheme.getColour (colour::topPanelMainText));
    
    auto labelArea = area.removeFromTop (jmax (getLocalBounds().getHeight()/4, 40 + UI::MARGIN)).reduced (4*UI::MARGIN, 0);
    labelArea.removeFromRight (labelArea.getWidth()*2/3);
    
    g.setFont (font::plumeFont.withHeight (14.0f));
    g.drawText ("User Presets Path :", 
                labelArea.removeFromTop (labelArea.getHeight()/2),
                Justification::centredLeft, true);
                
    g.drawText ("Plugins Path :", 
                labelArea,
                Justification::centredLeft, true);
    
    auto scanArea = area.removeFromTop (20 + 2*UI::MARGIN).reduced (4*UI::MARGIN, UI::MARGIN);
    
    g.drawText ("Scan Plugins :", 
                scanArea.withWidth (scanArea.getWidth()/3),
                Justification::centredLeft, true);
}

void FileOptionsSubPanel::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();
    
    auto labelArea = area.removeFromTop (jmax (getLocalBounds().getHeight()/4, 40 + UI::MARGIN)).reduced (4*UI::MARGIN, 0); // Custom Dirs
    labelArea.removeFromLeft (labelArea.getWidth()/3);
    
    presetDirLabel->setBounds (labelArea.withHeight (20).translated (0, labelArea.getHeight()/4 - 10));
    pluginDirLabel->setBounds (labelArea.withHeight (20).translated (0, labelArea.getHeight()*3/4 - 10));
                             
    auto scanArea = area.removeFromTop (20 + 2*UI::MARGIN).reduced (4*UI::MARGIN, UI::MARGIN);
    scanArea.removeFromLeft (scanArea.getWidth()/3); // "Scan Plugins" text
    
    scanner->setBounds (scanArea);
    
    mailButton->setBounds (area.removeFromBottom (30 + 2*MARGIN).reduced (4*MARGIN, MARGIN).withWidth (60));
}

void FileOptionsSubPanel::buttonClicked (Button* bttn)
{
    TRACE_IN;
    
    if (bttn == mailButton)
    {
        if (auto* plumeLogger = dynamic_cast<FileLogger*> (Logger::getCurrentLogger()))
        {
            String fullLog = plumeLogger->getLogFile().loadFileAsString().removeCharacters ("\n");
            
            /* Only keeps the last 3 entries of the log: the one that had an issue, 
               the one used to send the report, and one more to cover cases where the plugin
               has to be checked (For instance the plugin check when launching Ableton..)
               If too long, keeps the 6000 last characters.. */
            int startIndex = jmax (fullLog.upToLastOccurrenceOf("Plume Log", false, false)
                                          .upToLastOccurrenceOf("Plume Log", false, false)
								          .lastIndexOf("Plume Log"),
								          fullLog.length() - 6000);
			
		  #if JUCE_WINDOWS					          
			String mail_str ("mailto:damien.leboulaire@enhancia.co"
                             "?Subject=[Plume Report]"
			                 "&cc=alex.levacher@enhancia.co"
		                     "&body=" + fullLog.substring (startIndex));
		    LPCSTR mail_lpc = mail_str.toUTF8();

            ShellExecute (NULL, "open", mail_lpc,
		                  "", "", SW_SHOWNORMAL);
		  #elif JUCE_MAC					          
			String mail_str ("open mailto:damien.leboulaire@enhancia.co"
                             "?Subject=\"[Plume Report]\""
			                 "\\&cc=alex.levacher@enhancia.co"
		                     "\\&body=\"" + fullLog.substring (startIndex) + "\"");
		    
		    system (mail_str.toUTF8());
		  #endif
        }
    }
}

void FileOptionsSubPanel::labelTextChanged (Label* lbl)
{
    // Preset Directory Label
    if (lbl == presetDirLabel)
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
    if (lbl == pluginDirLabel)
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