/*
  ==============================================================================

    ScanHandler.cpp
    Created: 1 Apr 2021 1:41:17pm
    Author:  alexl

  ==============================================================================
*/

#include "ScanHandler.h"


ScanHandler::ScanHandler (KnownPluginList& pluginsRef)
    : pluginList (pluginsRef), scanThread (scannerProgress, progressMessage, fsp, pluginsRef)
{
    setPluginFormats();
}

ScanHandler::~ScanHandler() {}

void ScanHandler::timerCallback()
{
    if (!scanThread.isThreadRunning())
    {
        if (scannerProgress == 1.0f)
        {
            DBG("Scan Finished\n");
            finished = true;
            handleScanFinished();
        }
        else
        {
            DBG("Scan Crashed\n");
            //handleScanCrashed();
        }
    }
    else
    {
        //DBG("Scan running\n");
        //handleScanRunning();
    }
}

void ScanHandler::startScanProcess (const bool forceRescan,
                                    const Array<File>& directoriesToScan)
{
    if (formatManager->getNumFormats() == 0)
    {
        DBG ("Unable to scan plugins : no formats selected...");
        return;
    }

    resetScanInfo (true);

    scanThread.copyFormats (*formatManager);

    for (auto& file : directoriesToScan)
    {
        if (file.isDirectory())
        {
            fsp.addIfNotAlreadyThere (file);
        }
    }

    fsp.removeRedundantPaths();
    createFilesToScanArray (fsp);

    scanThread.copyFilesToScan (filesToScan);

    scanThread.startThread();

    startTimer (10);
}

void ScanHandler::cancelScan()
{
    scanThread.stopThread (10000);

    stopTimer();
}

void ScanHandler::resetScanInfo (const bool resetProgress)
{
    if (resetProgress)
    {
        scannerProgress = 0.0f;
        progressMessage = "";
    }

    fsp = FileSearchPath();
    finished = false;
}

void ScanHandler::handleScanRunning()
{
    if (scanThread.isThreadRunning())
    {
        DBG (getScanInfo());
    }
}

void ScanHandler::handleScanCrashed()
{
    resetScanInfo (true);
}

void ScanHandler::handleScanFinished()
{
    stopTimer();
    finished = true;
}

void ScanHandler::setPluginFormats (bool useVST, bool useVST3, bool useAUOnMac)
{
    formatManager.reset (new AudioPluginFormatManager());

  #if !JUCE_MAC
    ignoreUnused (useAUOnMac);
  #endif

    if (useVST) formatManager->addFormat (new VSTPluginFormat());

  #if JUCE_MAC
    //if (useAUOnMac) formatManager->addFormat (new AudioUnitPluginFormat()); // TO UNCOMMENT when implementing AU
  #endif

  #if JUCE_PLUGINHOST_VST3
    if (useVST3) formatManager->addFormat (new VST3PluginFormat());
  #endif
}

void ScanHandler::createFilesToScanArray (const FileSearchPath& directoriesToSearchIn)
{
    filesToScan.clear();

    for (int formatNum = 0; formatNum < formatManager->getNumFormats(); formatNum++)
    {
        // Add all files or Ids (check to see if type didn't exist already)
        auto filesOrIdsForGivenFormat = formatManager->getFormat (formatNum)->searchPathsForPlugins (fsp, true, true);

        for (auto fileOrId : filesOrIdsForGivenFormat)
        {
            bool shouldAdd = true;

            for (auto* fileToScanToCompareTo : filesToScan)
                if ((fileOrId == fileToScanToCompareTo->fileOrIdentifier ||
                    fileOrId.fromLastOccurrenceOf ("\\", false, false)
                            .upToLastOccurrenceOf (".", false, false)
                     == fileToScanToCompareTo->fileOrIdentifier.fromLastOccurrenceOf ("\\", false, false)
                                                              .upToLastOccurrenceOf (".", false, false)) &&
                    fileToScanToCompareTo->format == formatNum)
                {
                    shouldAdd = false;
                }
            
                else if (fileOrId.contains ("Plume.") ||
                    fileOrId.contains ("Plume Tests.") ||
                    fileOrId.contains ("Plum,Enhn"))
                {
                    shouldAdd = false;
                }

            if (shouldAdd) filesToScan.add (new FileToScan (fileOrId, formatNum));
        }
    }
}

String ScanHandler::getScanInfo()
{
    return progressMessage;
}

bool ScanHandler::isScanRunning()
{
    return scanThread.isThreadRunning();
}

bool ScanHandler::hasScanFinished()
{
    return finished;
}

std::atomic<float>& ScanHandler::getProgressRef()
{
    return scannerProgress;
}

String& ScanHandler::getProgressStringRef()
{
    return progressMessage;
}

void ScanHandler::setLastCrash (const String& lastCrashedPluginId)
{
    lastCrashedPlugin = lastCrashedPluginId;
}

const String ScanHandler::getLastCrashedPluginId()
{
    return lastCrashedPlugin;
}

const bool ScanHandler::hasLastScanCrashed()
{
    return lastCrashedPlugin.isNotEmpty();
}