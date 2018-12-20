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
                                                        Colour(0xffffffff),
		                                                Colour(0xaaffffff),
		                                                Colour(0xaaaaaaff)));
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
	
    g.fillAll (Colour (0xff393939));
    
    //Gradient for horizontal lines
    auto grad = ColourGradient::horizontal (Colour (0x05ffffff), MARGIN, 
                                            Colour (0x05ffffff), getWidth() - MARGIN);
    grad.addColour (0.5, Colour (0x40ffffff));

    
    auto area = getLocalBounds();
    
    // Plume. text
    g.setColour (Colours::white);
    g.setFont (Font (font, 25.0f, Font::bold));
    g.drawText ("Plume.", area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN),
                Justification::centredLeft, true);

	g.setGradientFill (grad);
	g.drawHorizontalLine (area.getY(), MARGIN, getWidth() - MARGIN);
    
	area.removeFromTop (2*MARGIN); // Extra space

                
    // Presets Panel
    g.drawHorizontalLine (area.getY() + MARGIN, MARGIN, getWidth() - MARGIN);
    
    g.setFont (Font (font, 15.0f, Font::plain));
	g.setColour(Colours::white);
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
