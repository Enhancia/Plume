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
	ScanHandler();
	~ScanHandler();

    //==============================================================================
    void timerCallback();

    //==============================================================================

	void startScanProcess (const File& pluginList,
						   const File& deadsManPedalFIle,
						   const bool forceRescan,
						   const Array<File>& directoriesToScan);
	void cancelScan();
	void resetScanInfo (const bool resetProgress);
	void handleScanCrashed();
	void handleScanRunning();
	void handleScanFinished();

    //==============================================================================
	void setPluginFormats (bool useVST = true, bool useVST3 = true, bool useAUOnMac = true);
	void resetFormatQueue(); 

    //==============================================================================
    String getScanInfo();
    bool isScanRunning();
    bool hasScanFinished();

    //==============================================================================
    std::atomic<float>& getProgressRef();
    String& getPluginStringRef();
    void updateTotalProgress();

private:
    //==============================================================================
	class ScanInfoFetchThread : public Thread
    {
    public:
        ScanInfoFetchThread (std::atomic<int>& formatIdRef,
        					 std::atomic<int>& pluginIdRef,
        					 std::atomic<float>& progressRef,
        					 ChildProcess& scannerProcessRef,
        					 const int readBufferSize = 50)
        	: Thread ("Scan Info Fetch Thread"),
        	  formatId (formatIdRef),
        	  pluginId (pluginIdRef),
        	  progress (progressRef),
        	  scannerProcess (scannerProcessRef),
        	  bufferSize (readBufferSize)
        {
        	dataBuffer = malloc (bufferSize);
        }

        ~ScanInfoFetchThread()
        {
        	free (dataBuffer);
        }

        void run() override
        {
            while (! threadShouldExit())
            {
            	if (scannerProcess.isRunning())
            	{
                    DBG("Process Running");
                	fetchData();
            	}
            	else
            	{
                    DBG("Process Finished");
                    flushAvailableData();
            		signalThreadShouldExit();
            	}

                wait (10);
            }

            DBG ("Thread just exited...");
        }

		void fetchData()
		{
		    int dataSize = scannerProcess.readProcessOutput (dataBuffer, bufferSize);
            DBG("Datasize " << dataSize);

		    if (dataSize > 0)
		    {
		        String message = String::createStringFromData (dataBuffer, dataSize);

		        parseData (message);
		    }
		}

		void flushAvailableData()
		{
			parseData (scannerProcess.readAllProcessOutput());
		}

		void parseData (const String& message)
		{
			if (message.isNotEmpty())
	        {
	            const String lastMessage = message.fromLastOccurrenceOf ("\n", false, false)
	            								   .upToLastOccurrenceOf ("\n", false, false);

	            StringArray values = StringArray::fromTokens (lastMessage, ";", String());

                DBG("Message : " << lastMessage << " | Values : " << values.joinIntoString (" - "));

	            if (values.size() == 3)
	            {
                    const int newFormat = values[0].getIntValue();
                    const int newPlugin = values[1].getIntValue();
                    const float newProgress = values[2].getFloatValue();

	            	if (newPlugin > pluginId || newFormat > formatId) pluginId.store (newPlugin);
                    if (newProgress > progress || newFormat > formatId) progress.store (newProgress);
                    if (newFormat > formatId) formatId.store (newFormat);

                    DBG("[Values] format " << formatId << " | plugin " << pluginId << " | progress " << progress << "\n");
	            }
	        }
		}

        std::atomic<int>& formatId;
        std::atomic<int>& pluginId;
        std::atomic<float>& progress;
        ChildProcess& scannerProcess;
        const int bufferSize;
        void* dataBuffer;
    };

    ScanInfoFetchThread scanInfoFetchThread;

    //==============================================================================
	void startScanForFormat (const String& pluginFormat,
							 const File& pluginList,
							 const File& deadsManPedalFIle,
							 const bool forceRescan,
							 const Array<File>& directoriesToScan);

	void flushAvailableMessages (const int bufferSize = 150);
	void readProcessOutput (const int bufferSize = 150);
	bool readProcessOutput (void* dataBuffer, const int bufferSize = 150);

    //==============================================================================
	std::unique_ptr <AudioPluginFormatManager> formatManager;
	FileSearchPath fsp;
	StringArray pluginsToScan;

	Array<AudioPluginFormat*> formatsToScanQueue;
	bool finished = false;
	int crashCount = 0;
	String pluginBeingScanned;

    int formatCount = 0;
    int numFormatsToScan = 0;
    std::atomic<int> formatId  {0};
    std::atomic<int> pluginId  {0};
    std::atomic<float> scannerProgress {0.0f};
    std::atomic<float> totalScanProgress {0.0f};

	ChildProcess scannerProcess;

    //==============================================================================
	struct LastScanInfo
	{
		String pluginFormat;
		File pluginList;
		File deadsManPedalFile;
		bool forceRescan;
		Array<File> directoriesToScan;
	};

	LastScanInfo lastScanInfo;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScanHandler)
};
