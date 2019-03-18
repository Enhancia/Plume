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
    addAndMakeVisible (tabbedOptions = new TabbedPanelComponent (processor));

    tabbedOptions->addTab (new GeneralOptionsSubPanel (processor), "General");
    tabbedOptions->addTab (new FileOptionsSubPanel (processor), "File");
    
    // Close button
    addAndMakeVisible (closeButton = new ShapeButton ("Close Options Button",
                                                       Colour(0x00000000),
                                                       Colour(0x00000000),
                                                       Colour(0x00000000)));

    Path p;
    p.startNewSubPath (0, 0);
    p.lineTo (3*PLUME::UI::MARGIN, 3*PLUME::UI::MARGIN);
    p.startNewSubPath (0, 3*PLUME::UI::MARGIN);
    p.lineTo (3*PLUME::UI::MARGIN, 0);

    closeButton->setShape (p, false, true, false);
	  closeButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::topPanelMainText), 1.0f);
    closeButton->addMouseListener (this, false);
	  closeButton->addListener (this);
}

OptionsPanel::~OptionsPanel()
{
    tabbedOptions = nullptr;
    closeButton = nullptr;
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
                                               float(optionsArea.getX()), 
                                               UI::currentTheme.getColour(colour::sideBarSeparatorOut),
                                               float(optionsArea.getRight()));
    gradOut.addColour (0.5, UI::currentTheme.getColour(colour::sideBarSeparatorIn));

    g.setGradientFill (gradOut);
    g.drawRect (optionsArea);
}

void OptionsPanel::resized()
{
    using namespace PLUME::UI;
    
    optionsArea = getBounds().reduced (getWidth()/5, getHeight()/8);
    
    tabbedOptions->setBounds (optionsArea.reduced (2*MARGIN, MARGIN));
  #if JUCE_WINDOWS
    closeButton->setBounds (juce::Rectangle<int> (3*MARGIN, 3*MARGIN).withRightX (optionsArea.getRight())
                                                                     .withY (optionsArea.getY())
                                                                     .reduced (5));
  #elif JUCE_MAC
    closeButton->setBounds (juce::Rectangle<int> (3*MARGIN, 3*MARGIN).withPosition (optionsArea.getTopLeft())
                                                                     .reduced (5));
  #endif
}

//==============================================================================
void OptionsPanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton)
    {
        setVisible (false);
    }
}

void OptionsPanel::mouseUp (const MouseEvent& event)
{
    if (!optionsArea.contains (event.getPosition()))
    {
        setVisible (false);
    }
}


void OptionsPanel::mouseEnter (const MouseEvent &event)
{
    if (event.eventComponent == closeButton)
	{
	    closeButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::topPanelMainText)
                                                      .withAlpha (0.4f), 1.0f);
	}
}

void OptionsPanel::mouseExit (const MouseEvent &event)
{
    if (event.eventComponent == closeButton)
	{
	    closeButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::topPanelMainText), 1.0f);
	}
}

void OptionsPanel::visibilityChanged()
{
}