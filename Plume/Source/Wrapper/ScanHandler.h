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

	ScanHandler();
	~ScanHandler();

    void timerCallback();
	void startScanProcess (const File& pluginList,
						   const File& deadsManPedalFIle,
						   const bool forceRescan,
						   const Array<File>& directoriesToScan);
	void cancelScan();
	void resetScanInfo();
	void handleScanCrashed();
	void handleScanRunning();
	void handleScanFinished();

	void setPluginFormats (bool useAUOnMac = false);

    String getScanInfo();

private:
	void startScanForFormat (const String& pluginFormat,
							 const File& pluginList,
							 const File& deadsManPedalFIle,
							 const bool forceRescan,
							 const Array<File>& directoriesToScan);

	std::unique_ptr <AudioPluginFormatManager> formatManager;
	Array<AudioPluginFormat*> formatsToScanQueue;
	int crashCount = 0;
	String pluginBeingScanned;
	float scanProgress = 0.0f;
	ChildProcess scannerProcess;

	struct LastScanInfo
	{
		String pluginFormat;
		File pluginList;
		File deadsManPedalFile;
		bool forceRescan;
		Array<File> directoriesToScan;
	};

	LastScanInfo lastScanInfo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScanHandler)
};