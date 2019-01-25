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
OptionsPanel::OptionsPanel (PlumeProcessor& proc)   : processor (proc)
{
    optionsArea = getBounds().reduced (getWidth()/4, getHeight()/6);
    addAndMakeVisible (settings = new PropertyPanel ("Settings Panel"));
    createAndAddProperties();
}

OptionsPanel::~OptionsPanel()
{
    settings = nullptr;
}

//==============================================================================
void OptionsPanel::paint (Graphics& g)
{
    using namespace PLUME;
    
    // transparent area
    g.setColour (UI::currentTheme.getColour (colour::topPanelTransparentArea));
    g.fillRect (getBounds());
    
    // options panel area
    g.setColour (UI::currentTheme.getColour (colour::topPanelBackground));
    g.fillRect (optionsArea);
    
    // options panel outline
    auto gradOut = ColourGradient::horizontal (UI::currentTheme.getColour(colour::sideBarSeparatorOut),
                                               optionsArea.getX(), 
                                               UI::currentTheme.getColour(colour::sideBarSeparatorOut),
                                               optionsArea.getRight());
    gradOut.addColour (0.5, UI::currentTheme.getColour(colour::sideBarSeparatorIn));

    g.setGradientFill (gradOut);
    g.drawRect (optionsArea);
    
    // Main Text
    g.setColour (UI::currentTheme.getColour (colour::topPanelMainText));
    g.setFont (Font (UI::font, 20.0f, Font::plain));
    g.drawText ("Settings :", 
                optionsArea.getX() + 2*UI::MARGIN,
                optionsArea.getY() + UI::MARGIN,
                optionsArea.getWidth(),
                UI::HEADER_HEIGHT,
                Justification::topLeft, true);
}

void OptionsPanel::resized()
{
    optionsArea = getBounds().reduced (getWidth()/6, getHeight()/8);
    settings->setBounds (optionsArea.reduced (2*PLUME::UI::MARGIN, 20 + 2*PLUME::UI::MARGIN));
}

//==============================================================================
void OptionsPanel::mouseUp (const MouseEvent& event)
{
    if (!optionsArea.contains (event.getPosition()))
    {
        setVisible (false);
    }
}

void OptionsPanel::createAndAddProperties()
{
    Array<PropertyComponent*> props;
    props.add (new TextPropertyComponent (processor.getParameterTree()
                                                   .state.getChild (0)
                                                   .getPropertyAsValue (PLUME::treeId::presetDir, nullptr),
                                          "Custom presets :", 50, false));
    settings->addProperties (props);
}