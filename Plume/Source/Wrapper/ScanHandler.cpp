/*
  ==============================================================================

    ScanHandler.cpp
    Created: 1 Apr 2021 1:41:17pm
    Author:  alexl

  ==============================================================================
*/

#include "ScanHandler.h"


ScanHandler::ScanHandler()
{
	setPluginFormats();
}

ScanHandler::~ScanHandler() {}

void ScanHandler::timerCallback()
{
	if (scannerProcess.isRunning())
	{
		DBG ("Scan Running\n");
		handleScanRunning();
	}
	else if (scannerProcess.getExitCode() == 0)
	{
		DBG ("Scan Fnished\n");
		handleScanFinished();
	}
	else
	{
		DBG ("Scan Crashed\n");
		handleScanCrashed();
	}
}

void ScanHandler::startScanProcess (const File& pluginList,
							 		const File& deadsManPedalFIle,
							 		const bool forceRescan,
							 		const Array<File>& directoriesToScan)
{
	startScanForFormat (formatsToScanQueue.getFirst()->getName(),
						pluginList,
						deadsManPedalFIle,
						forceRescan,
						directoriesToScan);
}

void ScanHandler::startScanForFormat (const String& pluginFormat,
							 		  const File& pluginList,
							 		  const File& deadsManPedalFIle,
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
		args.add (deadsManPedalFIle.getFullPathName());
		args.add ((forceRescan ? "0" : "1"));

		for (auto file : directoriesToScan) args.add (file.getFullPathName());

		if (scannerProcess.start (args))
		{
			DBG ("============= SCAN START ===========\n");

			lastScanInfo = {pluginFormat, pluginList, deadsManPedalFIle, forceRescan, directoriesToScan};

			startTimer (50);
		}
	}
}

void ScanHandler::cancelScan()
{
	if (scannerProcess.isRunning()) scannerProcess.kill();

	stopTimer();
}

void ScanHandler::resetScanInfo()
{
	crashCount = 0;
	scanProgress = 0.0f;
	pluginBeingScanned = "";
}

void ScanHandler::handleScanRunning()
{
	if (scannerProcess.isRunning())
	{
		int  bufferSize = 500;
		void* dataBuffer = malloc (bufferSize);

		int dataSize = scannerProcess.readProcessOutput (dataBuffer, bufferSize);

		if (dataSize > 0)
		{
			String message = String::createStringFromData (dataBuffer, dataSize);

			DBG ("Scanner message : " << message);

			if (message.isNotEmpty())
			{
				// get scan info from scanner
			}
		}
	}
}

void ScanHandler::handleScanCrashed()
{
	crashCount++;

	if (crashCount)
	{
		cancelScan();
	}
	else
	{
		stopTimer();

		//startScan ()
	}

}

void ScanHandler::handleScanFinished()
{
	stopTimer();
	resetScanInfo();
}

void ScanHandler::setPluginFormats (bool useAUOnMac)
{
	formatManager.reset (new AudioPluginFormatManager());
	formatsToScanQueue.clear();

  #if JUCE_MAC
	ignoreUnused (useAUOnMac);
  #endif

    formatManager->addFormat (new VSTPluginFormat());

  #if JUCE_MAC
    if (useAUOnMac) formatManager->addFormat (new AudioUnitPluginFormat());
  #endif

  #if JUCE_PLUGINHOST_VST3
    formatManager->addFormat (new VST3PluginFormat());
  #endif

    for (auto* format : formatManager->getFormats()) formatsToScanQueue.add (format);
}

String ScanHandler::getScanInfo()
{
	return String (int(scanProgress*100)) + "% | " + pluginBeingScanned;
}