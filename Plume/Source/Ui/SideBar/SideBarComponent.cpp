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
    //addAndMakeVisible (presetComponent = new PresetComponent (processor));
}

SideBarComponent::~SideBarComponent()
{
}

void SideBarComponent::paint (Graphics& g)
{
	using namespace PLUME::UI;
	
    g.fillAll (Colour (0xff393939));

    auto area = getLocalBounds();
    
    g.setColour (Colours::white);
    g.setFont (Font (font, 25.0f, Font::bold));
    g.drawText ("Plume.", area.removeFromTop (HEADER_HEIGHT).reduced (MARGIN),
                Justification::centredLeft, true);

	g.setColour(Colour(0x40ffffff));
	g.drawHorizontalLine(area.getY() + MARGIN, MARGIN, getWidth() - MARGIN);

	area.removeFromTop (HEADER_HEIGHT); // Extra space for menu icons

    g.setFont (Font (font, 15.0f, Font::plain));
    
    // Plugin Panel
    g.setColour (Colour (0x40ffffff));
    g.drawHorizontalLine (area.getY() + MARGIN, MARGIN, getWidth() - MARGIN);
    
    g.setColour (Colours::white);
    g.drawText ("Plugins", area.removeFromTop (area.getHeight()/2).reduced (2*MARGIN),
                Justification::topLeft, true);
                
    // Presets Panel
    g.setColour (Colour (0x40ffffff));
    g.drawHorizontalLine (area.getY() + MARGIN, MARGIN, getWidth() - MARGIN);
    
	g.setColour(Colours::white);
    g.drawText ("Presets", area.reduced (2*MARGIN),
                Justification::topLeft, true);
}

void SideBarComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();
    area.removeFromTop (2*HEADER_HEIGHT);
    area.removeFromTop (area.getHeight()/2);
    
    //presetComponent->setBounds (area.reduced (2*MARGIN, 4*MARGIN));
}
