/*
  ==============================================================================

    ScanHandler.h
    Created: 1 Apr 2021 1:41:17pm
    Author:  alexl

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/PlumeCommon.h"


/**
 *  \class ScanHandler ScanHandler.h
 *
 *  \brief Class that manages scanning plugins for Plume.
 *
 *  This class launches the PlumePluginScanner, and handles interactions with it.
 */

class ScanHandler : public Timer
{
public:
    //==============================================================================
    enum Formats
    {
        VST =0,
      #if JUCE_MAC
        AU,
      #endif
      #if JUCE_PLUGINHOST_VST3
        VST3,
      #endif
        numFormats
    };

    //==============================================================================
    ScanHandler (KnownPluginList& pluginsRef);
    ~ScanHandler();

    //==============================================================================
    void timerCallback();

    //==============================================================================
    void startScanProcess (const bool forceRescan,
                           const Array<File>& directoriesToScan);
    void cancelScan();
    void resetScanInfo (const bool resetProgress);
    void handleScanCrashed();
    void handleScanRunning();
    void handleScanFinished();

    //==============================================================================
    void setPluginFormats (bool useVST = true, bool useVST3 = true, bool useAUOnMac = true);

    //==============================================================================
    String getScanInfo();
    bool isScanRunning();
    bool hasScanFinished();

    //==============================================================================
    std::atomic<float>& getProgressRef();
    String& getProgressStringRef();

private:
    //==============================================================================
    class ScanThread : public Thread
    {
    public:
        explicit ScanThread (std::atomic<float>& progressRef,
                             String& pluginStringRef,
                             FileSearchPath& directoriesRef,
                             KnownPluginList& pluginsRef)
            : Thread ("Scan Thread"),
              progress (progressRef),
              progressMessage (pluginStringRef),
              directoriesToSearch (directoriesRef),
              pluginList (pluginsRef)
        {}

        ~ScanThread() {}

        void run() override
        {
            numFilesToScan = countNumFilesToScan();

            // For each format
            for (int formatNum = 0; (formatNum < formatManager->getNumFormats() && !threadShouldExit()); formatNum++)
            {
                DBG (" Scanning format : " << formatManager->getFormat (formatNum)->getName() << "\n");
                
                StringArray fileOrIds = formatManager->getFormat (formatNum)->searchPathsForPlugins (directoriesToSearch, true, true);

                // For all candidate plugin files for this format
                for (int fileNum = 0; (fileNum < fileOrIds.size() && !threadShouldExit()); fileNum++)
                {
                    String fileOrIdentifier = fileOrIds[fileNum];
                    DBG ("   - Scanning file : " << fileOrIdentifier << " ( Progress : " << progress << " )");

                    if (launchScannerProgram (formatManager->getFormat(formatNum)->getName(), fileOrIdentifier))
                    {
                        int count = 0;
                        while (scannerProcess.isRunning() && count < timerLimit
                                                          && !threadShouldExit())
                        {
                            count++;
                            wait (10);
                        }

                        const int exitCode = scannerProcess.getExitCode();

                        if (!scannerProcess.isRunning()/* || exitCode == 0
                                                        || exitCode == 1
                                                        || exitCode == 2*/)
                        {
                            const int exitCode = scannerProcess.getExitCode();

                            if (exitCode == 0)
                            {
                                if (!pluginList.isListingUpToDate (fileOrIdentifier, *formatManager->getFormat (formatNum)))
                                {
                                    OwnedArray<PluginDescription> found;
                                    formatManager->getFormat(formatNum)->findAllTypesForFile (found, fileOrIdentifier);

                                    for (auto* desc : found)
                                    {
                                        bool success = pluginList.addType (*desc); // Adds type with dbg alert
                                        if (success) DBG ("     Successful!");
                                        else DBG("     Failed to add to KPL...!");
                                    }
                                }
                            }
                            else
                            {
                                pluginList.addToBlacklist (fileOrIdentifier);
                                DBG ("     Failed... Added to blacklist");
                            }
                        }
                        else // Scanner process still running..
                        {
                            jassert (scannerProcess.kill()); // Force kill with dbg alert
                        }
                    }

                    updateProgress (fileNum, fileOrIdentifier);
                }
            }

            progress = 1.0f;
            DBG ("Thread just exited...");
        }

        bool launchScannerProgram (const String& formatString, const String& fileToScan)
        {
          #if JUCE_WINDOWS
            File scannerExe (File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("Enhancia/utilities/PluginScanner.exe"));
          #elif JUCE_MAC
            File scannerExe (File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Application Support/Enhancia/PlumePluginScanner"));
          #endif

            if (scannerExe.existsAsFile())
            {
                StringArray args;

                args.add (scannerExe.getFullPathName());
                args.add (formatString);
                args.add (fileToScan);

                return (scannerProcess.start (args));
            }

            return false;
        }

        void copyFormats (const AudioPluginFormatManager& managerToUse)
        {
            formatManager.reset (new AudioPluginFormatManager());

            for (auto* format : managerToUse.getFormats())
            {
                if (format->getName() == VSTPluginFormat::getFormatName())
                    formatManager->addFormat (new VSTPluginFormat());

              #if JUCE_MAC
                else if (format->getName() == AudioUnitPluginFormat::getFormatName())
                    formatManager->addFormat (new AudioUnitPluginFormat());
              #endif

                else if (format->getName() == VST3PluginFormat::getFormatName())
                    formatManager->addFormat (new VST3PluginFormat());
            }
        }

        int countNumFilesToScan()
        {
            int numFilesTotal = 0;

            for (int formatNum = 0; formatNum < formatManager->getNumFormats(); formatNum++)
            {
                StringArray fileOrIds = formatManager->getFormat (formatNum)->searchPathsForPlugins (directoriesToSearch, true, true);

                numFilesTotal += fileOrIds.size();
            }

            return numFilesTotal;
        }

        void updateProgress (const int currentFileNum, const String& currentPluginName)
        {
            progress = ((float) currentFileNum / (float) numFilesToScan);
            progressMessage = currentPluginName.fromLastOccurrenceOf ("\\", false, false)
                                               .upToLastOccurrenceOf (".", false, false) +
                              " (" + String (currentFileNum) + "/" + String (numFilesToScan) + ")";
        }

        std::atomic<float>& progress;
        std::unique_ptr<AudioPluginFormatManager> formatManager;
        FileSearchPath& directoriesToSearch;
        ChildProcess scannerProcess;
        KnownPluginList& pluginList;
        String& progressMessage;
        int numFilesToScan = 0;

        const int timerLimit = 3000;
    };

    ScanThread scanThread;

    //==============================================================================
    void startScanForFormat (const String& pluginFormat,
                             const File& pluginList,
                             const File& deadsManPedalFIle,
                             const bool forceRescan,
                             const Array<File>& directoriesToScan);

    //==============================================================================
    std::unique_ptr <AudioPluginFormatManager> formatManager;
    KnownPluginList& pluginList;
    FileSearchPath fsp;
    bool finished = false;
    String progressMessage;
    std::atomic<float> scannerProgress {0.0f};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScanHandler)
};
