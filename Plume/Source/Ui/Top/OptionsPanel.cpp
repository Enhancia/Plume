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
    // Area
    optionsArea = getBounds().reduced (getWidth()/5, getHeight()/8);

    addAndMakeVisible (tabbedOptions = new TabbedPanelComponent (processor));

    tabbedOptions->addTab (new GeneralOptionsSubPanel (processor), "General");
    tabbedOptions->addTab (new FileOptionsSubPanel (processor), "File");
}

OptionsPanel::~OptionsPanel()
{
    tabbedOptions = nullptr;
}

//==============================================================================
void OptionsPanel::paint (Graphics& g)
{
    using namespace PLUME;
    
    ColourGradient gradFill (UI::currentTheme.getColour (colour::topPanelBackground)
                                         .overlaidWith (Colour (0x10000000)),
                             float (optionsArea.getCentreX()),
                             float (optionsArea.getBottom()),
                             UI::currentTheme.getColour (colour::topPanelBackground),
                             float (optionsArea.getCentreX()),
                             float (optionsArea.getY()),
                             true);
    gradFill.addColour (0.7, UI::currentTheme.getColour (colour::topPanelBackground)
                                         .overlaidWith (Colour (0x10000000)));

    // transparent area
    g.setColour (UI::currentTheme.getColour (colour::topPanelTransparentArea));
    g.fillRect (getBounds());
    
    // options panel area
    g.setGradientFill (gradFill);
    //g.setColour (UI::currentTheme.getColour (colour::topPanelBackground));
    g.fillRect (optionsArea);
    
    // options panel outline
    auto gradOut = ColourGradient::horizontal (UI::currentTheme.getColour(colour::sideBarSeparatorOut),
                                               optionsArea.getX(), 
                                               UI::currentTheme.getColour(colour::sideBarSeparatorOut),
                                               optionsArea.getRight());
    gradOut.addColour (0.5, UI::currentTheme.getColour(colour::sideBarSeparatorIn));

    g.setGradientFill (gradOut);
    g.drawRect (optionsArea);
}

void OptionsPanel::resized()
{
    using namespace PLUME::UI;
    
    optionsArea = getBounds().reduced (getWidth()/5, getHeight()/8);

    tabbedOptions->setBounds (optionsArea.reduced (2*MARGIN, MARGIN));
}

//==============================================================================
void OptionsPanel::mouseUp (const MouseEvent& event)
{
    if (!optionsArea.contains (event.getPosition()))
    {
        setVisible (false);
    }
}

void OptionsPanel::visibilityChanged()
{
}