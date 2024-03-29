/*
  ==============================================================================

    UpdaterPanel.h
    Created: 12 Mar 2020 4:36:00pm
    Author:  aleva

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/PlumeCommon.h"
#include "../../../Updater/PlumeUpdater.h"

class UpdaterPanel    : public Component,
						public Timer,
                        private Button::Listener
{
public:
    //==============================================================================
	enum downloadProgress
	{
        noDownloadAvailable = 0,
		downloadAvailable,
		inProgress,
		downloadFinished,
        updateRequired
	};

    //==============================================================================
    UpdaterPanel (PlumeUpdater& updtr);
    ~UpdaterPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void timerCallback() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;
    bool keyPressed (const KeyPress& key) override;

    //==============================================================================
    void resetAndOpenPanel (bool updateIsRequired = false);
    void closeAndResetPanel();
    
private:
    //==============================================================================
    void createLabels();
    void createButtons();
	void updateComponentsForSpecificStep (downloadProgress downloadStep);

    //==============================================================================
    juce::Rectangle<int> panelArea;
    std::unique_ptr<Label> bodyText;
    std::unique_ptr<Label> titleLabel;
    std::unique_ptr<PlumeShapeButton> closeButton;
    std::unique_ptr<TextButton> bottomButton;
    std::unique_ptr<TextButton> viewNotesButton;

    //==============================================================================
    downloadProgress currentProgress = downloadAvailable;

    //==============================================================================
    PlumeUpdater& updater;
    float& progress;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpdaterPanel)
};