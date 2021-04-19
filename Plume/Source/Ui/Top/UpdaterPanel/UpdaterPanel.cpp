/*
  ==============================================================================

    UpdaterPanel.cpp
    Created: 12 Mar 2020 4:36:00pm
    Author:  aleva

  ==============================================================================
*/

#include "UpdaterPanel.h"

//==============================================================================
UpdaterPanel::UpdaterPanel (PlumeUpdater& updtr)
	: updater (updtr), progress (updtr.getDownloadProgressReference())
{
    createLabels();
    createButtons();

    updateComponentsForSpecificStep (noDownloadAvailable);
}

UpdaterPanel::~UpdaterPanel()
{
}

void UpdaterPanel::paint (Graphics& g)
{
    // updater panel area
    g.setColour (getPlumeColour (topPanelBackground));
    g.fillRoundedRectangle (panelArea.reduced (1).toFloat(), 10.0f);
}

void UpdaterPanel::resized()
{
	using namespace PLUME::UI;

    panelArea = getLocalBounds().reduced (getWidth()/5, getHeight()/10);
    
    // Close Button
    #if JUCE_WINDOWS
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withRightX (panelArea.getRight() - MARGIN_SMALL)
                                                         .withY (panelArea.getY() + MARGIN_SMALL));
    #elif JUCE_MAC
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withPosition (panelArea.getTopLeft()
    																			 .translated (MARGIN_SMALL,
    																			   			  MARGIN_SMALL)));
    #endif

    auto area = panelArea.reduced (PLUME::UI::MARGIN);

    titleLabel->setBounds (area.removeFromTop (area.getHeight()/5));

    auto buttonArea = area.removeFromBottom (jmin (area.getHeight()/5, 40));
    bottomButton->setBounds (buttonArea.withSizeKeepingCentre (bottomButton->getBestWidthForHeight (buttonArea.getHeight()),
                                     						   buttonArea.getHeight()));

    bodyText->setBounds (area.reduced (PLUME::UI::MARGIN));
}

void UpdaterPanel::timerCallback()
{
	jassert (currentProgress == inProgress);

	if (updater.getDownloadState() == PlumeUpdater::downloadFinished)
	{
		stopTimer();

		updateComponentsForSpecificStep (downloadFinished);
		
		return;
	}

	bodyText->setText ("Progress :\n\n" + String (int (progress*100)) + " %", dontSendNotification);
}


void UpdaterPanel::buttonClicked (Button* bttn)
{
	if (bttn == closeButton.get())
	{
		closeAndResetPanel();
	}

	else if (bttn == bottomButton.get())
	{
		switch (currentProgress)
		{
			case noDownloadAvailable:
				closeAndResetPanel();
				break;
			case downloadAvailable:
				updateComponentsForSpecificStep (inProgress);
				startTimerHz (2);
				updater.startDownloadProcess();
				break;

			case updateRequired:
				updateComponentsForSpecificStep (inProgress);
				startTimerHz (2);
				updater.startDownloadProcess();
				break;

			case inProgress:
				/* Uh, hitting this should not be possible, since the button should not be visible during the download... */
				jassertfalse;
				break;
			case downloadFinished:
				if (updater.wasSuccessful())
				{
					updater.launchInstaller();
				}
				closeAndResetPanel();
				break;
			default:
				break;
		}
	}
}
void UpdaterPanel::resetAndOpenPanel (bool updateIsRequired)
{
	if (currentProgress != inProgress)
	{
		if (isTimerRunning ()) stopTimer();
		updateComponentsForSpecificStep (updateIsRequired ? updateRequired
														  : updater.hasNewAvailableVersion() ? downloadAvailable
														  									 : noDownloadAvailable);

		setVisible (true);
	}
}

void UpdaterPanel::closeAndResetPanel()
{
	if (currentProgress != inProgress)
	{
		if (isTimerRunning ()) stopTimer();

		setVisible (false);
		updateComponentsForSpecificStep (noDownloadAvailable);

    	// TODO Update notifications
	}
}

void UpdaterPanel::createLabels()
{
    titleLabel.reset (new Label ("Title Label", ""));
    addAndMakeVisible (*titleLabel);
    titleLabel->setFont (PLUME::font::plumeFontBold.withHeight (25.0f));
    titleLabel->setJustificationType (Justification::centred);

    bodyText.reset (new Label ("Body Text", ""));
    addAndMakeVisible (*bodyText);
    bodyText->setJustificationType (Justification::centred);
}

void UpdaterPanel::createButtons()
{
	// Bottom button
    bottomButton.reset (new TextButton ("Bottom Button"));
    addAndMakeVisible (*bottomButton);

    bottomButton->setButtonText ("");
    bottomButton->addListener (this);

	// Close button
    closeButton = std::make_unique <PlumeShapeButton> ("Close Options Button",
                                                       Colour(0),
                                                       getPlumeColour(topPanelMainText));
    addAndMakeVisible (*closeButton);

    Path p;
    p.startNewSubPath (0, 0);
    p.lineTo (3*PLUME::UI::MARGIN, 3*PLUME::UI::MARGIN);
    p.startNewSubPath (0, 3*PLUME::UI::MARGIN);
    p.lineTo (3*PLUME::UI::MARGIN, 0);

    closeButton->setShape (p, false, true, false);
	closeButton->addListener (this);
}

void UpdaterPanel::updateComponentsForSpecificStep (downloadProgress downloadStep)
{
	currentProgress = downloadStep;

	switch (currentProgress)
	{
			case noDownloadAvailable:
				closeButton->setVisible (true);
				bottomButton->setVisible (true);
				bottomButton->setButtonText ("Ok");

				titleLabel->setText ("Plume up to date", dontSendNotification);

				bodyText->setText ("No new Plume version was found.",
					               dontSendNotification);
				break;

			case downloadAvailable:
				closeButton->setVisible (true);
				bottomButton->setVisible (true);
				bottomButton->setButtonText ("Download");

				titleLabel->setText ("Plume Update", dontSendNotification);

				bodyText->setText ("A new Plume version is available!\n\n\n"
								   "Current : " + String (JucePlugin_VersionString)
												+ "\n\nNew : " + updater.getLatestVersionString(),
					               dontSendNotification);
				break;


			case updateRequired:
				closeButton->setVisible (true);
				bottomButton->setVisible (updater.hasNewAvailableVersion());
				bottomButton->setButtonText ("Download");

				titleLabel->setText ("Plume Update (Required)", dontSendNotification);
				
				if (updater.hasNewAvailableVersion())
				{
					bodyText->setText ("Your Plume is outdated! Please update your Plume to use it with your Neova product.\n\n\n"
									   "Current : " + String (JucePlugin_VersionString)
													+ "\n\nNew : " + updater.getLatestVersionString(),
						               dontSendNotification);
				}
				else
				{
					bodyText->setText ("No up-to-date version found.\n\nMake sure you are connected to internet and try again.",
						               dontSendNotification);
				}
				break;

			case inProgress:
				closeButton->setVisible (false);
				bottomButton->setVisible (false);

				titleLabel->setText ("Downloading  . . .", dontSendNotification);
				bodyText->setText ("Preparing download", dontSendNotification);
				break;

			case downloadFinished:
				closeButton->setVisible (true);
				bottomButton->setVisible (true);
				titleLabel->setText ("Download Finished", dontSendNotification);

				if (updater.wasSuccessful())
				{
					bottomButton->setButtonText ("Start Installer");
					bodyText->setText ("Successfully downloaded installer!", dontSendNotification);
				}
				else
				{
					bottomButton->setButtonText ("Close");
					bodyText->setText ("Failed to download installer...", dontSendNotification);
				}
				break;
			default:
				break;
	}

	resized();
}