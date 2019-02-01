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

	setComponentsVisible();
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
    cancelButton->setBounds (area.removeFromRight(100).reduced (PLUME::UI::MARGIN, 0));
    bar->setBounds (area.reduced (PLUME::UI::MARGIN, 0));
}

void ScannerComponent::buttonClicked (Button* bttn)
{
    if (bttn == scanButton)
    {
        formatToScan = 0;
		scanPlugins();
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
        
        scanFinished();
    }
}

void ScannerComponent::scanPlugins()
{
    enterModalState (false, nullptr, false);
    DBG ("Scan Start");
    
    dirScanner = processor.getWrapper().getDirectoryScannerForFormat (formatToScan);
    scanning = true;
    setComponentsVisible();
    
    threadPool.reset (new ThreadPool (numThreads));
    for (int i =0; i < numThreads; i++)
    {
		threadPool->addJob (new ScanJob(*this), true);
    }
    
    setComponentsVisible();
    startTimer (20);
}

bool ScannerComponent::doNextScan()
{
    if (dirScanner->scanNextFile (true, pluginBeingScanned))
    {
		scanProgress = (dirScanner->getProgress() + float (formatToScan))/PluginWrapper::Formats::numFormats;
        pluginBeingScanned = pluginBeingScanned.fromLastOccurrenceOf ("\\", false, false);
        return true;
    }
    
	scanProgress = (dirScanner->getProgress() + float (formatToScan))/PluginWrapper::Formats::numFormats;
    scanning = false;
    return false;
}

void ScannerComponent::cancelScan()
{
    if (threadPool->removeAllJobs (true, 5000))
    {
        stopTimer();
        scanning = false;
        scanProgress = 0.0f;
        pluginBeingScanned = "";
        bar->repaint();
        dirScanner.reset();
        setComponentsVisible(); 
        if (auto* header = dynamic_cast<PlumeComponent*> (getParentComponent()
                                                        ->getParentComponent()
                                                        ->findChildWithID ("header")))
        {
            header->update();
        }
    
        exitModalState (0);
    }
}

void ScannerComponent::scanFinished()
{
    formatToScan++;
    if (formatToScan < PluginWrapper::Formats::numFormats)
    {
        // Scans next format (if there is one)
        scanPlugins();
        return;
    }
    
    // Else reverts to non scan mode and saves plugins to external list
    setComponentsVisible();
    bar->repaint();
    
    dirScanner.reset();
    processor.getWrapper().savePluginListToFile();
    
    if (auto* header = dynamic_cast<PlumeComponent*> (getParentComponent()
                                                        ->getParentComponent()
                                                        ->findChildWithID ("header")))
    {
        header->update();
    }
        
    exitModalState (1);
}

void ScannerComponent::setComponentsVisible()
{
    cancelButton->setVisible (scanning);
}