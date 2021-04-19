/*
  ==============================================================================

    UpdaterSubPanel.cpp
    Created: 16 Apr 2021 10:33:13am
    Author:  alexl

  ==============================================================================
*/

#include "UpdaterSubPanel.h"

UpdaterSubPanel::UpdaterSubPanel (PlumeUpdater& updtr, UpdaterPanel& updtrPanel)
  : updater (updtr), updaterPanel (updtrPanel)
{
	updateButton.reset (new TextButton ("Update Button"));
	updateButton->setButtonText ("Update");
	updateButton->addListener (this);
	addAndMakeVisible (*updateButton);
}

UpdaterSubPanel::~UpdaterSubPanel()
{
    removeAllChildren();
}

void UpdaterSubPanel::paint (Graphics& g)
{
    paintSoftwareArea (g);
}

void UpdaterSubPanel::buttonClicked (Button* bttn)
{
    if (bttn == updateButton.get())
    {
        updaterPanel.resetAndOpenPanel (false);
    }
}

void UpdaterSubPanel::paintSoftwareArea (Graphics& g)
{
    auto softAreaTemp = softwareArea.reduced (PLUME::UI::MARGIN*2);

    const int notificationRadius = 10;
    g.setColour (Colour (0xff8090f0));

    if (updater.hasNewAvailableVersion())
    {
        g.fillEllipse (softAreaTemp.withTrimmedTop (softAreaTemp.getHeight()*2/3)
                                   .withSizeKeepingCentre (notificationRadius, notificationRadius)
                                   .withY (softAreaTemp.getY() + softAreaTemp.getHeight()*2/3 + 6 - notificationRadius/2).toFloat()
                                   .translated (PLUME::font::plumeFont.withHeight (20).getStringWidth ("v" + String (JucePlugin_VersionString))/2 + 5, -5));
    }

    g.setColour (getPlumeColour (topPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight (20));
    g.drawFittedText ("v" + String (JucePlugin_VersionString),
                      softAreaTemp.removeFromBottom (softAreaTemp.getHeight()/3),
                      Justification::centredTop, 1);

    Path plumePath = PLUME::path::createPath (PLUME::path::plumeLogo);

    plumePath.scaleToFit (softAreaTemp.reduced (softAreaTemp.getHeight()/4).getX(),
                          softAreaTemp.reduced (softAreaTemp.getHeight()/4).getY(),
                          softAreaTemp.reduced (softAreaTemp.getHeight()/4).getWidth(),
                          softAreaTemp.reduced (softAreaTemp.getHeight()/4).getHeight(),
                          true);

    g.strokePath (plumePath, {1.0f, PathStrokeType::curved});
}

void UpdaterSubPanel::resized()
{
    softwareArea = getLocalBounds().reduced(0, PLUME::UI::MARGIN);

    updateButton->setBounds (softwareArea.withSizeKeepingCentre (updateButton->getBestWidthForHeight (30), 30)
                                        .withBottomY (getLocalBounds().getBottom() - 2*PLUME::UI::MARGIN));
}