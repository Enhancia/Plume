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
SideBarComponent::SideBarComponent()
{
}

SideBarComponent::~SideBarComponent()
{
}

void SideBarComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xff202020));

    g.setColour (Colours::white);
    g.setFont (14.0f);
    g.drawText ("SideBarComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void SideBarComponent::resized()
{
}
