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
SideBarComponent::SideBarComponent (PlumeProcessor& proc)   : processor (proc)
{
    addAndMakeVisible (optionsButton = new ShapeButton ("Options Button",
                                                        PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill),
		                                                PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFillHighlighted),
		                                                PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFillClicked)));
	optionsButton->setShape (PLUME::path::gearPath, false, true, false);
	optionsButton->addListener (this);
    //addAndMakeVisible (presetComponent = new PresetComponent (processor));
}

SideBarComponent::~SideBarComponent()
{
}

void SideBarComponent::paint (Graphics& g)
{
	using namespace PLUME::UI;
	
    g.fillAll (currentTheme.getColour(PLUME::colour::sideBarBackground));
    
    //Gradient for horizontal lines
    auto grad = ColourGradient::horizontal (currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                            MARGIN, 
                                            currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                            getWidth() - MARGIN);
    grad.addColour (0.5, currentTheme.getColour(PLUME::colour::sideBarSeparatorIn));

    
    auto area = getLocalBounds();
    
    // Plume. text
    g.setColour (currentTheme.getColour(PLUME::colour::sideBarMainText));
    g.setFont (Font (font, 25.0f, Font::bold));
    g.drawText ("Plume.", area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN),
                Justification::centredLeft, true);

	g.setGradientFill (grad);
	g.drawHorizontalLine (area.getY(), MARGIN, getWidth() - MARGIN);
    
	area.removeFromTop (2*MARGIN); // Extra space

                
    // Presets Panel
    g.drawHorizontalLine (area.getY() + MARGIN, MARGIN, getWidth() - MARGIN);
    
    g.setFont (Font (font, 15.0f, Font::plain));
	g.setColour(currentTheme.getColour(PLUME::colour::sideBarSubText));
    g.drawText ("Presets", area.reduced (2*MARGIN),
                Justification::topLeft, true);
}

void SideBarComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();
    
    // Buttons
    auto buttonsArea = area.removeFromTop (HEADER_HEIGHT).removeFromRight (SIDEBAR_WIDTH/3);
    
    optionsButton->setBounds (buttonsArea.removeFromRight (buttonsArea.getWidth()/2).reduced (MARGIN));
    
    // Presets
	area.removeFromTop (HEADER_HEIGHT);
    //presetComponent->setBounds (area.reduced (2*MARGIN, 4*MARGIN));
    area.removeFromTop (area.getHeight()/2);
}

void SideBarComponent::buttonClicked (Button* bttn)
{
    if (bttn == optionsButton)
    {
        // WIP -> Whould trigger a message to open the options panel...
        // For now only scans the plugins in the defaultDirs
        processor.getWrapper().scanAllPluginsInDirectories (false, true);
		processor.sendActionMessage (PLUME::commands::updateInterface);
    }
}
