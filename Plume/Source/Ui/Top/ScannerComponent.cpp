/*
  ==============================================================================

    ScannerComponent.cpp
    Created: 31 Jan 2019 11:42:32am
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "ScannerComponent.h"

//==============================================================================
ScannerComponent::ScannerComponent (PlumeProcessor& proc)   : processor (proc)
{
    // Scan Button
    addAndMakeVisible (scanButton = new ShapeButton ("Scan Button",
                                                     PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill),
		                                             PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill)
		                                                                                .withAlpha (0.5f),
		                                             PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill)
		                                                                                .withAlpha (0.7f)));
		                                                                                
	scanButton->setShape (PLUME::path::magnifyingGlassPath, false, true, false);
	scanButton->addListener (this);
	
	addAndMakeVisible (cancelButton = new TextButton ("Cancel Button"));
	cancelButton->setButtonText ("Cancel");
	cancelButton->addListener (this);
	
	addAndMakeVisible (bar = new PlumeProgressBar (scanProgress, pluginBeingScanned, "Scanning :"));

	//setComponentsVisible();
}

ScannerComponent::~ScannerComponent()
{
	scanButton = nullptr;
	cancelButton = nullptr;
	bar = nullptr;
}

void ScannerComponent::paint (Graphics& g)
{
}

void ScannerComponent::resized()
{
    auto area = getLocalBounds();
    scanButton->setBounds (area.removeFromLeft (20));
    //cancelButton->setBounds (area.removeFromRight(100).reduced (PLUME::UI::MARGIN, 0));
    bar->setBounds (area.reduced (PLUME::UI::MARGIN, 0));
}

void ScannerComponent::buttonClicked (Button* bttn)
{
    if (bttn == scanButton)
    {
        //processor.getWrapper().scanAllPluginsInDirectories (true, true);
		scanPlugins();
        
        if (auto* header = dynamic_cast<PlumeComponent*> (getParentComponent()->findChildWithID ("header")))
        {
            header->update();
        }
    }
    
    else if (bttn == cancelButton)
    {
        cancelScan();
    }
}

void ScannerComponent::timerCallback()
{
    if (scanning)
    {
		bar->repaint();
    }
    else
    {
        bar->repaint();
        stopTimer();
    }
}

void ScannerComponent::scanPlugins()
{
    enterModalState (false, nullptr, false);
    DBG ("Scan Start");
    
    scanning = true;
    //setComponentsVisible();
    startTimer (20);
    
    int i = 0;
    for (int i =0; i < PluginWrapper::Formats::numFormats; i++)
    {
        ScopedPointer<PluginDirectoryScanner> dirScanner = processor.getWrapper().getDirectoryScannerForFormat (i);
        
        // Rescans until scanNextFile returns false
        while (dirScanner->scanNextFile (true, pluginBeingScanned))
        {
            scanProgress = dirScanner->getProgress();
            pluginBeingScanned = pluginBeingScanned.fromLastOccurrenceOf ("\\", false, false);
            DBG ("Scanning : " << pluginBeingScanned << " | Progress : " << scanProgress);
            
            if (scanning == false) break; 
        }
        
        if (scanning == false) break;
        
        scanProgress = dirScanner->getProgress();
    }
    
    scanFinished();
}

void ScannerComponent::cancelScan()
{
    scanning = false;
    //setComponentsVisible();
    
    exitModalState (0);
}

void ScannerComponent::scanFinished()
{
    scanning = false;
    //setComponentsVisible();
    
    bar->repaint();
    processor.getWrapper().savePluginListToFile();
    exitModalState (1);
}

void ScannerComponent::setComponentsVisible()
{
    bar->setVisible (scanning);
    cancelButton->setVisible (scanning);
}
