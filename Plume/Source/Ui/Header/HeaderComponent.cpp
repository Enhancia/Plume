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
HeaderComponent::HeaderComponent (PlumeProcessor& proc) : processor (proc)
{
    addAndMakeVisible (pluginNameLabel = new Label ("Plugin Name Label", processor.getWrapper().getWrappedPluginInfoString()));
    pluginNameLabel->setFont (Font (PLUME::UI::font, 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    pluginNameLabel->setJustificationType (Justification::centred);
    pluginNameLabel->setEditable (false, false, false);
    pluginNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    pluginNameLabel->setColour (Label::textColourId, Colour (0xff393939));
    pluginNameLabel->addMouseListener (this, false);
    
    addAndMakeVisible (presetNameLabel = new Label ("Preset Name Label", "No preset"));
    presetNameLabel->setFont (Font (PLUME::UI::font, 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    presetNameLabel->setJustificationType (Justification::centred);
    presetNameLabel->setEditable (false, false, false);
    presetNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    presetNameLabel->setColour (Label::textColourId, Colour (0xff393939));
}

HeaderComponent::~HeaderComponent()
{
}

void HeaderComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xfff8f8f8));
    
    g.setColour (Colour (0xff707070));
    
    //separator path
    Path p;
    p.startNewSubPath (getWidth()/3, 2*PLUME::UI::MARGIN);
    p.lineTo (getWidth()/3, getHeight() - 2*PLUME::UI::MARGIN);
    g.strokePath (p , PathStrokeType (1.0f, PathStrokeType::curved, PathStrokeType::butt));
}

void HeaderComponent::resized()
{
    auto area = getLocalBounds();
    
    presetNameLabel->setBounds (area.removeFromLeft (getWidth()/3).reduced (PLUME::UI::MARGIN));
    pluginNameLabel->setBounds (area.reduced (PLUME::UI::MARGIN));
}

void HeaderComponent::update()
{
    pluginNameLabel->setText (processor.getWrapper().getWrappedPluginInfoString(), dontSendNotification);
}