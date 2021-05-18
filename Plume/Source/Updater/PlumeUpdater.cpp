/*
  ==============================================================================

    PlumeUpdater.cpp
    Created: 9 Mar 2020 10:36:32am
    Author:  Alex

  ==============================================================================
*/

#include "PlumeUpdater.h"

PlumeUpdater::PlumeUpdater() : currentVersion (JucePlugin_VersionString)
{
    checkForNewAvailableVersion();
}

PlumeUpdater::~PlumeUpdater()
{
}

void PlumeUpdater::checkForNewAvailableVersion()
{
    auto json (fetchRepoJSON());
    auto* repo (json.getDynamicObject());

    if (repo != nullptr)
    {
        latestVersion = repo->getProperty ("tag_name").toString();

        if (latestVersion.containsOnly (".0123456789"))
        {
            String currentVerTemp = currentVersion;
            String latestVerTemp = latestVersion;

            for (int subdivision=0; subdivision < 3; subdivision++)
            {
                if (currentVerTemp.upToFirstOccurrenceOf (".", false, false).getIntValue()
                        < latestVerTemp.upToFirstOccurrenceOf (".", false, false).getIntValue())
                {
                    availableVersion = fetchFileURL (*repo);
                    return;
                }
                else if (currentVerTemp.upToFirstOccurrenceOf (".", false, false).getIntValue()
                          > latestVerTemp.upToFirstOccurrenceOf (".", false, false).getIntValue())
                {
                    return;
                }

                currentVerTemp = currentVerTemp.fromFirstOccurrenceOf (".", false, false);
                latestVerTemp = latestVerTemp.fromFirstOccurrenceOf (".", false, false);
            }
        }
    }
}

void PlumeUpdater::finished (URL::DownloadTask* task, bool success)
{
    state = downloadFinished;
    successful = success;

    if (!success)
    {
        downloadedFile.deleteFile();
    }

	DBG ("Download finished " << (successful ? "Successfully" : "Unsuccessfully"));
}

void PlumeUpdater::progress (URL::DownloadTask* task,
                            int64 bytesDownloaded,
                            int64 totalLength )
{
    DBG ("Downloading ... " << float (bytesDownloaded)/1000 << " / "
                            << float (totalLength)/1000 << " Ko\n");

    if (totalLength != 0) downloadProgress = float (bytesDownloaded)/totalLength;
}

void PlumeUpdater::startDownloadProcess()
{
    if (fileToDownloadURL.isEmpty())
    {
        state = downloadFinished;
        successful = false;
        return;
    }

    URL assetURL (fileToDownloadURL);
    DBG ("Attempting to download file : " << assetURL.getFileName());

    state = inProgress;
    downloadProgress = 0.0f;
    
    #if JUCE_WINDOWS
    downloadedFile = File::getSpecialLocation (File::userApplicationDataDirectory ).getChildFile ("Enhancia")
                                                                                   .getChildFile ("Plume")
                                                                                   .getChildFile ("Installer");
    #elif JUCE_MAC
    downloadedFile = File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Application Support")
                                                                                 .getChildFile ("Enhancia")
                                                                                 .getChildFile ("Installers")
                                                                                 .getChildFile ("Plume");
    #endif
    
    if (downloadedFile.exists()) downloadedFile.deleteRecursively();
    downloadedFile.createDirectory();

    downloadedFile = downloadedFile.getNonexistentChildFile(fileToDownloadString.upToFirstOccurrenceOf (".", false, true),
                                                            fileToDownloadString.fromFirstOccurrenceOf (".", true, true));
    
    //downloadTask.reset (assetURL.downloadToFile (downloadedFile, "\r\nAccept: application/octet-stream\r\n", this));
    downloadTask = GitAssetDownloader::downloadAsset (fileToDownloadURL, downloadedFile, this);
}

bool PlumeUpdater::hasNewAvailableVersion()
{
    return availableVersion;
}

PlumeUpdater::downloadState PlumeUpdater::getDownloadState()
{
    return state;
}

bool PlumeUpdater::wasSuccessful()
{
    return successful;
}

float& PlumeUpdater::getDownloadProgressReference()
{
    return downloadProgress;
}

String PlumeUpdater::getLatestVersionString()
{
    return latestVersion;
}

File PlumeUpdater::getDownloadedFile()
{
    if (successful) return downloadedFile;

    return File();
}

void PlumeUpdater::launchInstaller()
{
    if (wasSuccessful())
    {
        #if JUCE_WINDOWS

        if (!getDownloadedFile().startAsProcess())
        {
            getDownloadedFile().revealToUser();
        }

        #elif JUCE_MAC

        if (getDownloadedFile().startAsProcess())
        {
            auto plumeDisk = File("/Volumes").findChildFiles(1, false, "Plume*");
       
            if (!plumeDisk.isEmpty()) plumeDisk[0].getChildFile ("Plume_Installer.pkg")
                                                  .startAsProcess();
        }
		else
		{
			getDownloadedFile().revealToUser();
		}
        #endif
    }
}

void PlumeUpdater::initializeFileToDownloadString()
{
    #if JUCE_WINDOWS
	fileToDownloadString = "Plume-Setup.msi";
    #elif JUCE_MAC
    fileToDownloadString = "Plume-Setup.dmg";
    #endif
}

var PlumeUpdater::fetchRepoJSON()
{
    // Creating input stream to get file link
    int status;
    std::unique_ptr<InputStream> urlInStream (REPO_URL.createInputStream (false, nullptr, nullptr,
                                                                         "Authorization: token " + PLUME::auth::MACHINE_TOKEN,
                                                                         1000, nullptr, &status));

    if (urlInStream == nullptr || status != 200)
    {
        DBG ("Failed to create input stream...\nHTTP status code : " << String (status));
        return var();
    }

    // Converting into JSON to get properties
    String streamString = urlInStream->readEntireStreamAsString();
    return JSON::parse (streamString);
}

bool PlumeUpdater::fetchFileURL (DynamicObject& jsonRef)
{
    String fileExtension;

    #if JUCE_MAC
    fileExtension = "dmg";
    #elif JUCE_WINDOWS
    fileExtension = "msi";
    #endif

    // Getting assets
    auto* assets = jsonRef.getProperty ("assets").getArray();

    if (assets != nullptr)
    {
        // Finding url for the File to download
        for (auto asset : *assets)
        {
            if (asset.getProperty ("name", var()).toString().endsWith (fileExtension))
            {
                fileToDownloadString = asset.getProperty ("name", var()).toString();
                fileToDownloadURL = asset.getProperty ("url", var()).toString();
                fileSize = asset.getProperty ("size", var(0));

                return true;
            }
        }
    }

    DBG ("Failed to find assets or file URL...");

    return false;
}
