/*
  ==============================================================================

    OptionsPanel.cpp
    Created: 24 Jan 2019 5:23:26pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "OptionsPanel.h"

//==============================================================================
OptionsPanel::OptionsPanel()
{
}

OptionsPanel::~OptionsPanel()
{
}

//==============================================================================
void OptionsPanel::paint (Graphics& g)
{
    using namespace PLUME;
    
    g.fillAll (UI::currentTheme.getColour (colour::topPanelTransparentArea));
    
    auto optionsArea = getLocalBounds().reduced (5*UI::MARGIN);
    
    g.setColour (UI::currentTheme.getColour (colour::topPanelBackground));
    
    g.fillRect (optionsArea);
    
    g.setColour (UI::currentTheme.getColour (colour::topPanelMainText));
    g.setFont (Font (font, 25.0f, Font::bold));
    g.drawText ("Settings :", optionsArea.removeFromTop (HEADER_HEIGHT).reduced (MARGIN),
                Justification::topLeft, true);
}

void OptionsPanel::resized()
{
}