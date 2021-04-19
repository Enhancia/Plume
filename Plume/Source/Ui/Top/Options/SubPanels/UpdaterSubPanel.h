/*
  ==============================================================================

    UpdaterSubPanel.h
    Created: 16 Apr 2021 10:33:13am
    Author:  alexl

  ==============================================================================
*/

#pragma once

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../../Common/PlumeCommon.h"
#include "../../../../Updater/PlumeUpdater.h"
#include "../../UpdaterPanel/UpdaterPanel.h"

class UpdaterSubPanel: public Component,
					   private Button::Listener
{
public:
    //==============================================================================
    UpdaterSubPanel (PlumeUpdater& updtr, UpdaterPanel& updtrPanel);
    ~UpdaterSubPanel();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    void paintSoftwareArea (Graphics& g);

    //==============================================================================
    PlumeUpdater& updater;
    UpdaterPanel& updaterPanel;

    //==============================================================================
    std::unique_ptr<TextButton> updateButton;
    juce::Rectangle<int> softwareArea;
    Image plumeIcon = ImageFileFormat::loadFrom (PlumeData::plumeLogo_png, PlumeData::plumeLogo_pngSize);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UpdaterSubPanel)
};