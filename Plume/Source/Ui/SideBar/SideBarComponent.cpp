/*
  ==============================================================================

    SideBarComponent.cpp
    Created: 12 Dec 2018 4:52:31pm
    Author:  Alex

  ==============================================================================
*/

#include "SideBarComponent.h"

//==============================================================================
SideBarComponent::SideBarComponent (PlumeProcessor& proc, Component& optsPanel)
        : processor (proc), optionsPanel (optsPanel)
{
    setName ("Side Bar");
    setComponentID ("sideBar");
    
    // Option button
    addAndMakeVisible (*(optionsButton = std::make_unique<ShapeButton> ("Options Button",
                                                        getPlumeColour (sideBarButtonFill),
		                                                getPlumeColour (sideBarButtonFill).withAlpha (0.7f),
		                                                getPlumeColour (sideBarButtonFill).interpolatedWith (Colour (0xff0000ff),
														                                                     0.5f)
																						  .withAlpha (0.7f))));
    optionsButton->setShape (PLUME::path::createPath (PLUME::path::options), false, true, false);
    optionsButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill), 1.5f);
    optionsButton->addMouseListener (this, false);
	optionsButton->addListener (this);

    // Info Panel
    addAndMakeVisible (*(hideInfoButton = std::make_unique<ShapeButton> ("Hide Info Button",
                                                         Colour (0x00000000),
                                                         Colour (0x00000000),
                                                         Colour (0x00000000))));
    hideInfoButton->setOutline (Colour (0x50ffffff), 1.0f);
    hideInfoButton->setToggleState (infoHidden, dontSendNotification); // side bar visible at first
    hideInfoButton->setClickingTogglesState (true);
    hideInfoButton->setBorderSize (BorderSize<int> (2));
    createHideInfoButtonPath();
    hideInfoButton->addMouseListener (this, false);
    hideInfoButton->addListener (this);

    addAndMakeVisible (*(infoPanel = std::make_unique<InfoPanel> (*hideInfoButton)));
    infoPanel->toBehind (hideInfoButton.get());

    // Preset Component
    addAndMakeVisible (*(presetComponent = std::make_unique<PresetComponent> (processor)));
}

SideBarComponent::~SideBarComponent()
{
    presetComponent = nullptr;
    infoPanel = nullptr;
}

//==============================================================================
const String SideBarComponent::getInfoString()
{
    const String bullet = " " + String::charToString (juce_wchar(0x2022));
    
    return "Hideable side bar :\n\n"
           + bullet + " Click on the arrow on the header to hide or show the side bar.\n"
           + bullet + " The top left hand corner has an option button to configure the plugin.";
}

void SideBarComponent::update()
{
    presetComponent->update();
}

//==============================================================================
void SideBarComponent::paint (Graphics& g)
{
	using namespace PLUME::UI;
	
    g.setColour (getPlumeColour (sideBarBackground));
    g.fillRoundedRectangle (getLocalBounds().reduced (0, MARGIN_SMALL)
                                            .withTrimmedLeft (MARGIN_SMALL)
                                            .toFloat(),
                            10.0f);
    
    auto area = getLocalBounds().removeFromTop (HEADER_HEIGHT).reduced (MARGIN);
    
    // ALPHA text
    g.setColour (currentTheme.getColour(PLUME::colour::sideBarSubText));
    g.setFont (PLUME::font::plumeFontLight.withHeight (14.0f));
    g.drawText ("ALPHA", area.removeFromRight (PLUME::font::plumeFontLight.withHeight (14.0f).getStringWidth ("ALPHA") + 2*MARGIN_SMALL)
                             .reduced (MARGIN_SMALL),
                Justification::topLeft, true);
    
    // Plume text
    g.setColour (currentTheme.getColour(PLUME::colour::sideBarMainText));
    g.setFont (PLUME::font::plumeFont.withHeight (24.0f));
    g.drawText ("Plume", area, Justification::centredRight, true);
}

void SideBarComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds().withTrimmedLeft (MARGIN_SMALL);
    
    // Buttons
    auto buttonsArea = area.removeFromTop (HEADER_HEIGHT).removeFromLeft (SIDEBAR_WIDTH/3);
    optionsButton->setBounds (buttonsArea.removeFromLeft (buttonsArea.getWidth()/2).reduced (MARGIN));
    
    // Presets
    if (!hideInfoButton->getToggleState())
    {
        infoPanel->setBounds (area.removeFromBottom (120)
                                  .withTrimmedBottom (MARGIN)
                                  .reduced (MARGIN, MARGIN_SMALL));
        hideInfoButton->setBounds (infoPanel->getBounds().withTrimmedTop (2*MARGIN)
                                                         .withSize (10, 10)
                                                         .withX (infoPanel->getBounds().getCentreX() - 5));
    }
    else 
    {
        infoPanel->setBounds (area.removeFromBottom (30 + 2*MARGIN)
                                  .withTrimmedBottom (MARGIN)
                                  .reduced (MARGIN, MARGIN_SMALL));
        hideInfoButton->setBounds (infoPanel->getBounds().withTrimmedTop (MARGIN)
                                                         .withSize (10, 10)
                                                         .withX (infoPanel->getBounds().getCentreX() - 5));
    }

    presetComponent->setBounds (area.reduced (MARGIN, 0));
}

void SideBarComponent::buttonClicked (Button* bttn)
{
    if (bttn == optionsButton.get())
    {
		// Displays options
		optionsPanel.setVisible (true);
    }

    else if (bttn == hideInfoButton.get())
    {
        createHideInfoButtonPath();
        hideInfoButton->setOutline (Colour (0x50ffffff), 1.0f);

        resized();
    }
}

void SideBarComponent::mouseEnter (const MouseEvent &event)
{
    if (event.eventComponent == hideInfoButton.get())
    {
        hideInfoButton->setOutline (Colour (0x10ffffff),  1.0f);
    }

    else if (event.eventComponent == optionsButton.get())
    {
        optionsButton->setOutline (getPlumeColour (sideBarButtonFill).withAlpha (0.5f),
                                    1.5f);
    }
}

void SideBarComponent::mouseExit (const MouseEvent &event)
{
    if (event.eventComponent == hideInfoButton.get())
    {
        hideInfoButton->setOutline (Colour (0x50ffffff), 1.0f);
    }

    else if (event.eventComponent == optionsButton.get())
    {
        optionsButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::sideBarButtonFill),
                                    1.5f);
    }
}

void SideBarComponent::addInfoPanelAsMouseListener (Component* componentToListen)
{
    componentToListen->addMouseListener (infoPanel.get(), true);
}

void SideBarComponent::createHideInfoButtonPath()
{
    using namespace PLUME::UI;

    Path p;
    
    if (hideInfoButton->getToggleState())
    {
        p.startNewSubPath (0, HEADER_HEIGHT);
        p.lineTo (HEADER_HEIGHT/2, 0);
        p.lineTo (HEADER_HEIGHT, HEADER_HEIGHT);
    }
    else
    {
        p.startNewSubPath (0,0);
        p.lineTo (HEADER_HEIGHT/2, HEADER_HEIGHT);
        p.lineTo (HEADER_HEIGHT, 0);
    }
    
    hideInfoButton->setShape (p, false, true, false);
}