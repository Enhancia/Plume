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
ScannerComponent::ScannerComponent (PlumeProcessor& proc, int buttonWidth)   : processor (proc),
                                                                               buttonW (buttonWidth),
                                                                               scanButton (new TextButton ("Scan Button"))
{
    // Scan Button
    addAndMakeVisible (scanButton);
    scanButton->setButtonText ("Scan");
    scanButton->addListener (this);
	
	addAndMakeVisible (bar = new PlumeProgressBar (scanProgress, pluginBeingScanned, "Scanning :"));
}

ScannerComponent::~ScannerComponent()
{
	scanButton = nullptr;
	bar = nullptr;
}

void ScannerComponent::paint (Graphics&)
{
}

void ScannerComponent::resized()
{
    if (getWidth() != 0 && (buttonW < 10 || buttonW > getWidth()*2/3)) buttonW = jmax (10, getWidth()/5);

    auto area = getLocalBounds();

    scanButton->setBounds (area.removeFromLeft (buttonW));
    bar->setBounds (area.reduced (PLUME::UI::MARGIN, 0));
}

void ScannerComponent::buttonClicked (Button* bttn)
{
    if (bttn == scanButton)
    {
        if (scanning)
        {
            cancelScan();
        }

        else
        {
            formatToScan = 0;
            scanPlugins();
        }
        
    }
}

void ScannerComponent::timerCallback()
{
    if (scanning)
    {
		bar->repaint();
		
		if (threadPool->getNumJobs() == 0) // Late job removal
        {
            cancelScan();
        }
    }
    else
    {
        bar->repaint();
        stopTimer();
        
        scanFinished();
    }
}

void ScannerComponent::scanPlugins (bool clearList)
{
    if (!isCurrentlyModal()) enterModalState (false, nullptr, false);
    DBG ("Scan Start");
    
    if (clearList) processor.getWrapper().getList().clear();

    dirScanner = processor.getWrapper().getDirectoryScannerForFormat (formatToScan);
    scanning = true;
    scanButton->setButtonText ("Cancel");
    
    threadPool.reset (new ThreadPool (numThreads));
    for (int i =0; i < numThreads; i++)
    {
		threadPool->addJob (new ScanJob(*this), true);
    }
    
    startTimer (20);
}

bool ScannerComponent::shouldScanNextFile()
{
    DBG ("Next File : " << dirScanner->getNextPluginFileThatWillBeScanned() << "\n\n");
	String nextPlugin = dirScanner->getNextPluginFileThatWillBeScanned();

    // Checks if plugin is PLUME.
    {
        String name;

        if (File::isAbsolutePath (nextPlugin)) name = File (nextPlugin).getFileNameWithoutExtension();
        else                                   name = nextPlugin;

        if (name == "Plume") return false;
    }

  #if JUCE_MAC
    // AudioUnit specific case.
    // Gets absolute path from the plugin's name.
    if (formatToScan == int (PluginWrapper::AU))
    {
        const File localAUDir = File::getSpecialLocation (File::commonApplicationDataDirectory)
                                        .getChildFile ("Audio/")
                                        .getChildFile ("Plug-Ins/")
                                        .getChildFile ("Components/");

        const File userAUDir = File::getSpecialLocation (File::userApplicationDataDirectory)
                                        .getChildFile ("Audio/")
                                        .getChildFile ("Plug-Ins/")
                                        .getChildFile ("Components/");

        if (localAUDir.getChildFile (nextPlugin).existsAsFile())
        {
            nextPlugin = localAUDir.getChildFile (nextPlugin).getFullPathName();
        }

        else if (userAUDir.getChildFile (nextPlugin).existsAsFile())
        {
            nextPlugin = userAUDir.getChildFile (nextPlugin).getFullPathName();
        }
        
    }
  #endif

    // Gets description and run checks
    if (File::isAbsolutePath (nextPlugin))
    {
        OwnedArray<PluginDescription> descArray;

		processor.getWrapper().getPluginFormat (nextPlugin)->findAllTypesForFile (descArray, nextPlugin);

        if (!descArray.isEmpty())
        {
			if (PluginDescription* nextDesc = descArray.getFirst())
			{
                // Checks if instrument
				if (!descArray.getFirst()->isInstrument)
				{
					return false;
				}

                // Checks if description was already scanned
                if (isDescriptionAlreadyInList (*nextDesc))
                {
                    return false;
                }
			}
        }
    }

    return true;
}

bool ScannerComponent::isDescriptionAlreadyInList (const PluginDescription& descriptionToCheck)
{
    for (int i=0; i<processor.getWrapper().getList().getNumTypes(); i++)
    {
        const PluginDescription& desc = *processor.getWrapper().getList().getType (i);

        if (descriptionToCheck.uid == desc.uid)
        {
            if (descriptionToCheck.pluginFormatName == desc.pluginFormatName &&
                descriptionToCheck.name == desc.name)
            {
                return true;
            }
        }
    }

    return false;
}

bool ScannerComponent::doNextScan()
{
    if (!shouldScanNextFile())
    {
        dirScanner->skipNextFile();
        return true;
    }

    if (dirScanner->scanNextFile (true, pluginBeingScanned))
    {
		scanProgress = (dirScanner->getProgress() + float (formatToScan))/PluginWrapper::Formats::numFormats;
        pluginBeingScanned = pluginBeingScanned.fromLastOccurrenceOf ("\\", false, false)
                                               .fromLastOccurrenceOf ("/", false, false);
        return true;
    }
    
	scanProgress = (dirScanner->getProgress() + float (formatToScan))/PluginWrapper::Formats::numFormats;
    scanning = false;
    return false;
}

void ScannerComponent::cancelScan()
{
    if (threadPool->getNumJobs() == 0 || threadPool->removeAllJobs (true, 6000))
    {
        scanning = false;
        
        stopTimer();
        scanProgress = 0.0f;
        pluginBeingScanned = "";
        bar->repaint();
        dirScanner.reset();
        scanButton->setButtonText ("Scan"); 
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
        // Scans next format
        scanPlugins (false);
        return;
    }
    
    // Else reverts to non scan mode and saves plugins to external list
    scanButton->setButtonText ("Scan");
    bar->repaint();
    
    dirScanner.reset();
    processor.getWrapper().savePluginListToFile();
    
    if (auto* header = dynamic_cast<PlumeComponent*> (getParentComponent() // FileSubPanel
                                                        ->getParentComponent() // TabbedPanel
                                                        ->getParentComponent() // OptionsPanel
                                                        ->getParentComponent() // Editor
                                                        ->findChildWithID ("header")))
    {
        header->update();
    }
        
    exitModalState (1);
}