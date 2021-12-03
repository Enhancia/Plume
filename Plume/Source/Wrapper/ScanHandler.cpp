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

void ScanHandler::startScanForFormat (const String& pluginFormat,
                                      const File& pluginList,
                                      const File& deadsManPedalFile,
                                      const bool forceRescan,
                                      const Array<File>& directoriesToScan)
{
    /*
  #if JUCE_WINDOWS
    File scannerExe (File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("Enhancia/utilities/PluginScanner.exe"));
  #elif JUCE_MAC
    File scannerExe (File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Application Support/Enhancia/PlumePluginScanner"));
  #endif
    if (scannerExe.existsAsFile())
    {
        StringArray args;

        args.add (scannerExe.getFullPathName());
        args.add (pluginFormat);
        args.add (pluginList.getFullPathName());
        args.add (deadsManPedalFile.getFullPathName());
        args.add ((forceRescan ? "0" : "1"));

        for (auto file : directoriesToScan) args.add (file.getFullPathName());
        
        DBG ("Pending messages : " <<  scannerProcess.readAllProcessOutput());

        if (scannerProcess.isRunning()) scannerProcess.kill();
        DBG ("Starting scan with args :\n" <<  args.joinIntoString ("\n"));

        if (scannerProcess.start (args))
        {
            DBG ("============= SCAN START ===========\n");
            scanThread.startThread();

            finished = false;
            lastScanInfo = {pluginFormat, pluginList, deadsManPedalFile, forceRescan, directoriesToScan};

            startTimer (1000);
        }
    }
    else
    {
        DBG ("Unable to scan plugins : missing scanner executable.");
    }*/
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
        //readProcessOutput();
    }
}

void ScanHandler::handleScanCrashed()
{
    resetScanInfo (true);
    
    /*
    crashCount++;

    if (crashCount > 9)
    {
        DBG ("Too many crashes.. Ending plugin scan here.");

        cancelScan();
        resetScanInfo (true);
    }
    else
    {
        stopTimer();

        startScanForFormat (lastScanInfo.pluginFormat,
                            lastScanInfo.pluginList,
                            lastScanInfo.deadsManPedalFile,
                            0,
                            lastScanInfo.directoriesToScan);
    }
    */
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