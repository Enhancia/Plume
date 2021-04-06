/*
  ==============================================================================

    ScanHandler.cpp
    Created: 1 Apr 2021 1:41:17pm
    Author:  alexl

  ==============================================================================
*/

#include "ScanHandler.h"


ScanHandler::ScanHandler() : scanInfoFetchThread (formatId,
                                                  pluginId,
                                                  scanProgress,
                                                  scannerProcess)
{
    setPluginFormats();
}

ScanHandler::~ScanHandler() {}

void ScanHandler::timerCallback()
{
    const int exitCode = scannerProcess.getExitCode();

    if (!scannerProcess.isRunning() ||
        !scanInfoFetchThread.isThreadRunning() ||
        exitCode == 0 || exitCode == 1)
    {
        if (exitCode == 0)
        {
            DBG("Scan Finished\n");
            handleScanFinished();
        }
        else if (exitCode == 1)
        {
            DBG("Scan Crashed\n");
            handleScanCrashed();
        }
    }
    else
    {
        DBG("Scan Running (code " << exitCode << ")");
        DBG("Process running : " <<  (scannerProcess.isRunning() ? "1" : "0"));
        DBG("Thread running : " << (scanInfoFetchThread.isThreadRunning() ? "1" : "0") << "\n");
        //handleScanRunning();
    }
}

void ScanHandler::startScanProcess (const File& pluginList,
                                    const File& deadsManPedalFile,
                                    const bool forceRescan,
                                    const Array<File>& directoriesToScan)
{
    if (formatsToScanQueue.isEmpty())
    {
        DBG ("Unable to scan plugins : no formats selected...");
        return;
    }

    for (auto& file : directoriesToScan)
    {
        if (file.isDirectory())
        {
            fsp.add (file);
        }
    }
      
    String formats;
    for (const auto* format : formatsToScanQueue) formats += format->getName() + " ";

    DBG ("Starting scan process. Formats : " << formats);
    pluginsToScan = formatsToScanQueue.getFirst()->searchPathsForPlugins (fsp, true, true);

    startScanForFormat (formatsToScanQueue.getFirst()->getName(),
                        pluginList,
                        deadsManPedalFile,
                        forceRescan,
                        directoriesToScan);
}

void ScanHandler::startScanForFormat (const String& pluginFormat,
                                      const File& pluginList,
                                      const File& deadsManPedalFile,
                                      const bool forceRescan,
                                      const Array<File>& directoriesToScan)
{
    File scannerExe (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/Plume/Utilities/PluginScanner.exe"));

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
            scanInfoFetchThread.startThread();

            finished = false;
            lastScanInfo = {pluginFormat, pluginList, deadsManPedalFile, forceRescan, directoriesToScan};

            startTimer (1000);
        }
    }
    else
    {
        DBG ("Unable to scan plugins : missing scanner executable.");
    }
}

void ScanHandler::cancelScan()
{
    scanInfoFetchThread.stopThread (10000);
    scannerProcess.kill();

    stopTimer();
}

void ScanHandler::resetScanInfo()
{
    crashCount = 0;
    formatId = 0;
    pluginId = 0;
    scanProgress = 0.0f;
    pluginBeingScanned = "";
    fsp = FileSearchPath();
    resetFormatQueue();
}

void ScanHandler::handleScanRunning()
{
    if (scannerProcess.isRunning())
    {
        DBG (getScanInfo());
        //readProcessOutput();
    }
}

void ScanHandler::handleScanCrashed()
{
    crashCount++;

    if (crashCount > 9)
    {
        DBG ("Too many crashes.. Ending plugin scan here.");

        cancelScan();
        resetScanInfo();
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

}

void ScanHandler::handleScanFinished()
{
    stopTimer();
    DBG ("Attempt to kill process..");
    if (scannerProcess.isRunning()) scannerProcess.kill();
    DBG ("Attempt to stop thread.. (30s time out)");
    if (scanInfoFetchThread.isThreadRunning()) scanInfoFetchThread.stopThread (30000);

    formatsToScanQueue.remove (0);

    if (formatsToScanQueue.isEmpty())
    {
        // Scan fully finished
        resetScanInfo();
        finished = true;
    }
    else // starts scan with next format
    {
        startScanForFormat(formatsToScanQueue.getFirst()->getName(),
                           lastScanInfo.pluginList,
                           lastScanInfo.deadsManPedalFile,
                           0,
                           lastScanInfo.directoriesToScan);
    }
}

void ScanHandler::setPluginFormats (bool useVST, bool useVST3, bool useAUOnMac)
{
    formatManager.reset (new AudioPluginFormatManager());

  #if !JUCE_MAC
    ignoreUnused (useAUOnMac);
  #endif

    if (useVST) formatManager->addFormat (new VSTPluginFormat());


  #if JUCE_MAC
    if (useAUOnMac) formatManager->addFormat (new AudioUnitPluginFormat());
  #endif

  #if JUCE_PLUGINHOST_VST3
    if (useVST3) formatManager->addFormat (new VST3PluginFormat());
  #endif

    resetFormatQueue();
}

void ScanHandler::resetFormatQueue()
{
    formatsToScanQueue.clear();
    for (auto* format : formatManager->getFormats()) formatsToScanQueue.add (format);
}

String ScanHandler::getScanInfo()
{
    return String (int(scanProgress*100)) + "% | " + pluginBeingScanned;
}

bool ScanHandler::isScanRunning()
{
    return scannerProcess.isRunning();
}

bool ScanHandler::hasScanFinished()
{
    return finished;
}

std::atomic<float>& ScanHandler::getProgressRef()
{
    return scanProgress;
}

String& ScanHandler::getPluginStringRef()
{
    return pluginBeingScanned;
}

void ScanHandler::readProcessOutput (const int bufferSize)
{
    void* dataBuffer = malloc (bufferSize);

    readProcessOutput (dataBuffer, bufferSize);

    free (dataBuffer);
}

bool ScanHandler::readProcessOutput (void* dataBuffer, const int bufferSize)
{
    int dataSize = scannerProcess.readProcessOutput (dataBuffer, bufferSize);

    if (dataSize > 0)
    {
        String message = String::createStringFromData (dataBuffer, dataSize);

        DBG ("Scanner message ("<< dataSize << "): " << message << "\n");

        if (message.isNotEmpty())
        {
            const String lastMessage = message.fromLastOccurrenceOf ("\n", false, false).upToLastOccurrenceOf ("\n", false, false);
            pluginBeingScanned = lastMessage.upToFirstOccurrenceOf (";", false, false);
            scanProgress = lastMessage.fromFirstOccurrenceOf (";", false, false).getFloatValue();

            return true;
        }
    }

    return false;
}

void ScanHandler::flushAvailableMessages (const int bufferSize)
{
    void* dataBuffer = malloc (bufferSize);
    
    do { DBG ("Flushing ...\n"); } while (readProcessOutput (dataBuffer, bufferSize));
        
    free (dataBuffer);
}