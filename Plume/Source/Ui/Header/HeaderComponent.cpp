/*
  ==============================================================================

    HeaderComponent.cpp
    Created: 12 Dec 2018 4:53:18pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "HeaderComponent.h"

//==============================================================================
HeaderComponent::HeaderComponent()
{
}

HeaderComponent::~HeaderComponent()
{
}

void HeaderComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xfff0f0f0));

    g.setColour (Colours::black);
    g.setFont (14.0f);
    g.drawText ("HeaderComponent", getLocalBounds(),
                Justification::centred, true);   // draw some placeholder text
}

void HeaderComponent::resized()
{
}
