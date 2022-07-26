/*
  ==============================================================================

    PlumeUpdater.h
    Created: 9 Mar 2020 10:36:32am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/PlumeCommon.h"
#include "GitAssetDownloader.h"

/**
    \class  PlumeUpdater PlumeUpdater.h
 
    \brief  Manages checking for updates and downloading the newest installer.
*/
class PlumeUpdater : public URL::DownloadTask::Listener
{
public:
    //==============================================================================
    enum downloadState
    {
        noDownload = 0,
        inProgress,
        downloadFinished
    };

	//==============================================================================
	PlumeUpdater();
	~PlumeUpdater();

    //==============================================================================
    void finished (URL::DownloadTask* task, bool success) override;

    void progress (URL::DownloadTask* task,
                   int64 bytesDownloaded,
                   int64 totalLength ) override;

	//==============================================================================
    void checkForNewAvailableVersion();
	void startDownloadProcess();
    bool hasNewAvailableVersion();

    //==============================================================================
    downloadState getDownloadState();
    bool wasSuccessful();
    float& getDownloadProgressReference();
    String getLatestVersionString();
    File getDownloadedFile();

    //==============================================================================
    void launchInstaller();
    
private:
    //==============================================================================
    void initializeFileToDownloadString();
    var fetchRepoJSON();
    bool fetchFileURL (DynamicObject& jsonRef);
    
    //==============================================================================
	const String currentVersion;
	String latestVersion = "";

	//==============================================================================
    const String AUTH_TOKEN = ""; // Replace with a an auth token allowed to read from the release repo.
    const URL REPO_URL = URL (""); // Replace with a github repo in which to add new versions.
    //==============================================================================
    String fileToDownloadString = "";
    String fileToDownloadURL = "";
    File downloadedFile;
    int fileSize = 0;

    //==============================================================================
    bool availableVersion = false;
    downloadState state = noDownload;
    bool successful = false;
    float downloadProgress = 0.0f;

    //==============================================================================
    std::unique_ptr<URL::DownloadTask> downloadTask;

    //==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlumeUpdater)
};
