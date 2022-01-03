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
    struct FileToScan
    {
        FileToScan (String fileOrId, int formatId)
            : fileOrIdentifier (fileOrId), format (formatId) {}
        
        String fileOrIdentifier; // File for VST or ID for AU
        int format; // int corresponding to the id in current format manager
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
    void createFilesToScanArray (const FileSearchPath& directoriesToSearchIn);

    //==============================================================================
    String getScanInfo();
    bool isScanRunning();
    bool hasScanFinished();

    //==============================================================================
    std::atomic<float>& getProgressRef();
    String& getProgressStringRef();

    //==============================================================================
    void setLastCrash (const String& lastCrashedPluginId);
    const bool hasLastScanCrashed();
    const String getLastCrashedPluginId();

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
            numFilesToScan = filesToScan.size();
            currentFileNum = 0;
            TemporaryFile resultFile ("rst");
            resultFile.getFile().create();
            TemporaryFile descriptionFile ("dsc");
            descriptionFile.getFile().create();

            startLogEntry();

            // For all candidate plugin files
            for (int fileNum = 0; (fileNum < numFilesToScan && !threadShouldExit()); fileNum++)
            {
                const String fileOrIdentifier = filesToScan[fileNum]->fileOrIdentifier;
                const int formatNum = filesToScan[fileNum]->format;

                if (!pluginList.isListingUpToDate (fileOrIdentifier, *formatManager->getFormat (formatNum))
                    && !isObviouslyNotAnInstrument (fileOrIdentifier, *formatManager->getFormat (formatNum))
                    && isNotBlacklisted (fileOrIdentifier))
                {
                    if (launchScannerProgram (formatManager->getFormat(formatNum)->getName(), fileOrIdentifier,
                                              descriptionFile.getFile(), resultFile.getFile()))
                    {
                        int count = 0;
                        
                        sleep (50);
                        
                        while (scannerProcess.isRunning() && count < timerLimit
                                                          && !threadShouldExit())
                        {
                            count++;
                            sleep (10);
                        }

                        if (!scannerProcess.isRunning())
                        {
                            const int exitCode = resultFile.getFile().loadFileAsString().isNotEmpty()
                                                    ? resultFile.getFile().loadFileAsString().getIntValue()
                                                    : scannerProcess.getExitCode(); // If no file or no result, uses program return value;

                            if (!readDmp().isEmpty())
                            {
                                PLUME::log::writeToLog ("Adding plugin to blacklist, it crashed during scan : " + fileOrIdentifier, PLUME::log::pluginScan);
                                pluginList.addToBlacklist (fileOrIdentifier);
                                clearDmp();
                            }
                            else if (exitCode == 0) // Successful scan
                            {
                                PLUME::log::writeToLog ("Scanner returned 0 (COUNT " + String (count) + "): Attempting to get description", PLUME::log::pluginScan);
                                
                                // If a description xml was shared by the scanner:
                                // creates and adds one or several PluginDescription object(s) from it. 
                                if (descriptionFile.getFile().loadFileAsString().isNotEmpty())
                                {
                                    if (auto descriptionsXml = XmlDocument::parse (descriptionFile.getFile()))
                                    {
                                        for (auto* descXml : descriptionsXml->getChildIterator())
                                        {
                                            DBG ("Scanned desc :" << descXml->toString());
                                            PluginDescription desc;

                                            if (desc.loadFromXml (*descXml))
                                            {
                                                PLUME::log::writeToLog ("Finished Scanning! Adding plugin to list : " + desc.name +
                                                                        "(" + desc.version + ")", PLUME::log::pluginScan);
                                                pluginList.addType (desc);
                                            } 
                                        }
                                    }
                                }
                                // If no description in file: tries to load descriptions from Plume.
                                // 'findAllTypesForFile()' might trigger a crash.
                                else
                                {
                                    PLUME::log::writeToLog ("Scanner returned 0 (COUNT " + String (count) + "): No result file: Attempting to create description", PLUME::log::pluginScan);
                                    OwnedArray<PluginDescription> found;
                                    
                                    addFileToDmp (fileOrIdentifier);
                                    formatManager->getFormat(formatNum)->findAllTypesForFile (found, fileOrIdentifier); // Might crash..
                                    clearDmp();

                                    for (auto* desc : found)
                                    {
                                        if (desc->name != "Plume" && desc->name != "Plume Tests")
                                        {
                                            PLUME::log::writeToLog ("Finished Scanning! Adding plugin to list : " + desc->name +
                                                                    "(" + desc->version + ")", PLUME::log::pluginScan);
                                            pluginList.addType (*desc);
                                        }
                                    }
                                }
                                
                            }
                            else // Scanner returned something other than 0
                            {
                                PLUME::log::writeToLog ("Failed to scan plugin (code " + String (exitCode) + ") : " + fileOrIdentifier, PLUME::log::pluginScan, PLUME::log::error);
                            }
                        }
                        else // Scanner process still running..
                        {
                            PLUME::log::writeToLog ("Failed to scan plugin (timeout) : " + fileOrIdentifier, PLUME::log::pluginScan, PLUME::log::error);
                            jassert (scannerProcess.kill()); // Force kill with dbg alert
                        }
                    }
                }

                updateProgress (fileOrIdentifier);
            }

            progress = 1.0f;
            endLogEntry();
        }

        bool launchScannerProgram (const String& formatString, const String& fileToScan,
                                   const File& descFile, const File& resFile)
        {
            if (PLUME::file::scannerExe.existsAsFile() && PLUME::file::deadMansPedal.existsAsFile())
            {
                StringArray args;

                args.add (PLUME::file::scannerExe.getFullPathName());
                args.add (formatString);
                args.add (fileToScan);
                args.add (PLUME::file::deadMansPedal.getFullPathName());
                args.add (descFile.getFullPathName());
                args.add (resFile.getFullPathName());

                PLUME::log::writeToLog ("Launching scan for plugin/format : " + fileToScan +
                                        " / " + formatString + " Args : " +
                                        args.joinIntoString (" | "), PLUME::log::pluginScan);
                
                return (scannerProcess.start (args));
            }

            jassert (false); // scanner or dmp file did not exist somehow...
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

              #if JUCE_PLUGINHOST_VST3
                else if (format->getName() == VST3PluginFormat::getFormatName())
                    formatManager->addFormat (new VST3PluginFormat());
              #endif
            }
        }

        void copyFilesToScan (const OwnedArray<FileToScan>& fileArrayToCopy)
        {
            filesToScan.clear();

            for (auto* fileToScanToCopy : fileArrayToCopy)
                filesToScan.add (new FileToScan (fileToScanToCopy->fileOrIdentifier,
                                                 fileToScanToCopy->format));
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

        void updateProgress (const String& currentPluginName)
        {
            progress = ((float) currentFileNum / (float) numFilesToScan);
            progressMessage = currentPluginName.fromLastOccurrenceOf ("\\", false, false)
                                               .upToLastOccurrenceOf (".", false, false) +
                              " (" + String (currentFileNum) + "/" + String (numFilesToScan) + ")";
            currentFileNum++;
        }

        bool isObviouslyNotAnInstrument (const String& fileOrIdentifier, AudioPluginFormat& format)
        {
	    #if JUCE_MAC
            if (format.getName() == AudioUnitPluginFormat::getFormatName())
            {
                return !fileOrIdentifier.contains ("Synths/aumu");
            }
	    #endif
	    
            return false;
        }

        bool isNotBlacklisted (const String& fileOrIdentifier)
        {
            for (auto blacklistedPlugin : pluginList.getBlacklistedFiles())
            {
                if (blacklistedPlugin.compare (fileOrIdentifier) == 0) return false;
            }

            return true;
        }
        
        String readDmp()
        {
            if (PLUME::file::deadMansPedal.existsAsFile())
                return PLUME::file::deadMansPedal.loadFileAsString();
            return String();
        }
        
        bool clearDmp()
        {
            if (PLUME::file::deadMansPedal.existsAsFile())
                return (PLUME::file::deadMansPedal.replaceWithText (String()));
            return false;
        }

        bool addFileToDmp (const String& filePathString)
        {
            if (PLUME::file::deadMansPedal.existsAsFile())
                return PLUME::file::deadMansPedal.replaceWithText (filePathString);
            return false;
        }


        void startLogEntry()
        {
            if (PLUME::file::scanLog.existsAsFile() || PLUME::file::scanLog.create().wasOk())
            {
                Time time (Time::getCurrentTime());

                PLUME::file::scanLog.appendText ("\n\r\n\r======= STARTING NEW PLUME PLUGIN SCAN : "
                                                     + String (time.getYear()) + "/"
                                                     + String (time.getMonth()) + "/"
                                                     + String (time.getDayOfMonth()) + " - "
                                                     + String (time.getHours()) + ":"
                                                     + String (time.getMinutes()) + ":"
                                                     + String (time.getSeconds()) + " =======\n\r");
            }
        }

        void endLogEntry()
        {
            if (PLUME::file::scanLog.existsAsFile())
            {
                Time time (Time::getCurrentTime());

                PLUME::file::scanLog.appendText ("======= SCAN FINISHED : "
                                                     + String (time.getYear()) + "/"
                                                     + String (time.getMonth()) + "/"
                                                     + String (time.getDayOfMonth()) + " - "
                                                     + String (time.getHours()) + ":"
                                                     + String (time.getMinutes()) + ":"
                                                     + String (time.getSeconds()) + " =======\n\r");
            }
        }

        std::atomic<float>& progress;
        std::unique_ptr<AudioPluginFormatManager> formatManager;
        OwnedArray<FileToScan> filesToScan;
        FileSearchPath& directoriesToSearch;
        ChildProcess scannerProcess;
        KnownPluginList& pluginList;
        String& progressMessage;
        int numFilesToScan = 0;
        int currentFileNum = 0;

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
    OwnedArray<FileToScan> filesToScan;
    KnownPluginList& pluginList;
    FileSearchPath fsp;
    bool finished = false;
    String progressMessage;
    std::atomic<float> scannerProgress {0.0f};
    String lastCrashedPlugin = "";

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScanHandler)
};
