/*
  ==============================================================================

    SideBarComponent.cpp
    Created: 12 Dec 2018 4:52:31pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "SideBarComponent.h"

//==============================================================================
SideBarComponent::SideBarComponent (PlumeProcessor& proc, Component& optsPanel)
        : processor (proc), optionsPanel (optsPanel)
{
    setName ("Side Bar");
    setComponentID ("sideBar");
    
    // Option button
    addAndMakeVisible (optionsButton = new ShapeButton ("Options Button",
                                                        getPlumeColour (sideBarButtonFill),
		                                                getPlumeColour (sideBarButtonFill).withAlpha (0.7f),
		                                                getPlumeColour (sideBarButtonFill).interpolatedWith (Colour (0xff0000ff),
														                                                     0.5f)
																						  .withAlpha (0.7f)));
    optionsButton->setShape (PLUME::path::createOptionsPath(), false, true, false);
    optionsButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill), 1.5f);
    optionsButton->addMouseListener (this, false);
	optionsButton->addListener (this);

    // Info Panel
    addAndMakeVisible (infoPanel = new InfoPanel());
    addAndMakeVisible (hideInfoButton = new ShapeButton ("Hide Info Button",
                                                         Colour (0x00000000),
                                                         Colour (0x00000000),
                                                         Colour (0x00000000)));
    hideInfoButton->setOutline (Colour (0x50ffffff), 1.0f);
    hideInfoButton->setToggleState (infoHidden, dontSendNotification); // side bar visible at first
    hideInfoButton->setClickingTogglesState (true);
    createHideInfoButtonPath();
    hideInfoButton->addMouseListener (this, false);
    hideInfoButton->addListener (this);

    // Preset Component
    addAndMakeVisible (presetComponent = new PresetComponent (processor));
}

SideBarComponent::~SideBarComponent()
{
    presetComponent = nullptr;
    infoPanel = nullptr;
}

//==============================================================================
const String SideBarComponent::getInfoString()
{
    return "Hideable side bar :\n\n"
           "- Click on the arrow on the header to hide or show the side bar.\n"
           "- The top left hand corner has an option button to configure the plugin.";
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
    
    //Gradient for horizontal lines
    auto grad = ColourGradient::horizontal (Colour (0x10ffffff),
                                            float(MARGIN), 
                                            Colour (0x10ffffff),
                                            float(getWidth() - MARGIN));
    grad.addColour (0.5, Colour (0x10ffffff));

    
    auto area = getLocalBounds();
    
    // Plume. text
    g.setColour (currentTheme.getColour(PLUME::colour::sideBarMainText));
    g.setFont (PLUME::font::plumeFontBold.withHeight (25.0f));
    g.drawText ("Plume.", area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN),
                Justification::centredRight, true);

	g.setGradientFill (grad);
	g.drawHorizontalLine (area.getY(), float(MARGIN), float(getWidth() - MARGIN));
    
	area.removeFromTop (MARGIN/2); // Extra space

    /*            
    // Presets Panel
    g.drawHorizontalLine (area.getY() + MARGIN, MARGIN, getWidth() - MARGIN);
    
    g.setFont (Font (font, 15.0f, Font::plain));
	g.setColour(currentTheme.getColour(PLUME::colour::sideBarSubText));
    g.drawText ("Presets", area.reduced (2*MARGIN, MARGIN),
                Justification::topLeft, true);
    */
    
    if (hideInfoButton->getToggleState()) // Lines that represents hidden infoPanel
    {
        g.drawHorizontalLine (getHeight() - 2*MARGIN, float(MARGIN), getWidth()/2.0f - float(2*MARGIN));
        g.drawHorizontalLine (getHeight() - 2*MARGIN, getWidth()/2.0f + float(2*MARGIN), float(getWidth() - MARGIN));
    }

    // Version Text
    g.setColour (getPlumeColour (presetsBoxRowText));
    g.setFont (PLUME::font::plumeFont.withHeight (10.0f));
    g.drawText ("Plume " + String(JucePlugin_VersionString),
	            1, getHeight() - MARGIN,
	            100, MARGIN,
                Justification::centredLeft, true);
}

void SideBarComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();
    
    // Buttons
    auto buttonsArea = area.removeFromTop (HEADER_HEIGHT).removeFromLeft (SIDEBAR_WIDTH/3);
    
    optionsButton->setBounds (buttonsArea.removeFromLeft (buttonsArea.getWidth()/2).reduced (MARGIN));
    
    // Presets
	area.removeFromTop (MARGIN);
    if (!hideInfoButton->getToggleState())
    {
        infoPanel->setBounds (area.removeFromBottom (3*HEADER_HEIGHT).withTrimmedBottom (MARGIN)
                                                                     .reduced (MARGIN));
    }
    else 
    {
        infoPanel->setBounds (area.removeFromBottom (3*MARGIN).withTrimmedBottom (2*MARGIN)
                                                              .withTrimmedTop (MARGIN));
    }

    hideInfoButton->setBounds (getWidth()/2 - 2*MARGIN, infoPanel->getY() - MARGIN/2, 4*MARGIN, MARGIN);
    presetComponent->setBounds (area.reduced (2*MARGIN, MARGIN));
    //area.removeFromTop (area.getHeight()/2);
}

void SideBarComponent::buttonClicked (Button* bttn)
{
    if (bttn == optionsButton)
    {
		// Displays options
		optionsPanel.setVisible (true);
    }

    else if (bttn == hideInfoButton)
    {
        infoPanel->setVisible (!hideInfoButton->getToggleState());
        resized();

        createHideInfoButtonPath();
        hideInfoButton->setOutline (Colour (0x50ffffff), 1.0f);
        repaint();
    }
}

void SideBarComponent::mouseEnter (const MouseEvent &event)
{
    if (event.eventComponent == hideInfoButton)
    {
        hideInfoButton->setOutline (Colour (0x10ffffff),  1.0f);
    }

    else if (event.eventComponent == optionsButton)
    {
        optionsButton->setOutline (getPlumeColour (sideBarButtonFill).withAlpha (0.5f),
                                    1.5f);
    }
}

void SideBarComponent::mouseExit (const MouseEvent &event)
{
    if (event.eventComponent == hideInfoButton)
    {
        hideInfoButton->setOutline (Colour (0x50ffffff), 1.0f);
    }

    else if (event.eventComponent == optionsButton)
    {
        optionsButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::sideBarButtonFill),
                                    1.5f);
    }
}

void SideBarComponent::addInfoPanelAsMouseListener (Component* componentToListen)
{
    componentToListen->addMouseListener (infoPanel, true);
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