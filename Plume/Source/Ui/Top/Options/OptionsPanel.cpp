/*
  ==============================================================================

    OptionsPanel.cpp
    Created: 24 Jan 2019 5:23:26pm
    Author:  Alex

  ==============================================================================
*/

#include "OptionsPanel.h"

//==============================================================================
OptionsPanel::OptionsPanel (PlumeProcessor& proc)   : processor (proc)
{
    addAndMakeVisible (tabbedOptions = new TabbedPanelComponent());

    tabbedOptions->addTab (new FileOptionsSubPanel (processor), "File");
    tabbedOptions->addTab (new AboutPanel(), "About");
    tabbedOptions->addTab (new Component(), "Update");
    
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
    closeButton->setBorderSize (BorderSize<int> (6));
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
    g.fillRoundedRectangle (optionsArea.toFloat(), 10.0f);
    
    // options panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               float(optionsArea.getX()), 
                                               Colour (0x10ffffff),
                                               float(optionsArea.getRight()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (optionsArea.toFloat(), 10.0f, 1.0f);
    
    auto area = optionsArea.reduced (PLUME::UI::MARGIN*2);

    paintProductInformations (g, area.removeFromTop (area.getHeight()/4).reduced (PLUME::UI::MARGIN));
}

void OptionsPanel::resized()
{
    using namespace PLUME::UI;
    
    optionsArea = getBounds().reduced (getWidth()/5, getHeight()/8);

    #if JUCE_WINDOWS
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withRightX (optionsArea.getRight() - MARGIN_SMALL)
                                                                     .withY (optionsArea.getY() + MARGIN_SMALL));
    #elif JUCE_MAC
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withPosition (optionsArea.getTopLeft()
                                                                                   .translated (MARGIN_SMALL,
                                                                                                MARGIN_SMALL)));
    #endif

    auto area = optionsArea.reduced (PLUME::UI::MARGIN*2);
    area.removeFromTop (area.getHeight()/4);

    area.reduce (PLUME::UI::MARGIN, PLUME::UI::MARGIN);

    tabbedOptions->setBounds (area);
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

void OptionsPanel::paintProductInformations(Graphics& g, juce::Rectangle<int> area)
{
    using namespace PLUME::UI;

    // Enhancia Logo
    auto enhanciaArea = area.removeFromLeft (area.getWidth()/2);
    g.drawImage (enhanciaLogo, enhanciaArea.toFloat(), RectanglePlacement::fillDestination);

    // Plume Text
    auto plumeTextArea = area.reduced (MARGIN*2, area.getHeight()/6);

    g.setColour (currentTheme.getColour(PLUME::colour::topPanelMainText));
    g.setFont(PLUME::font::plumeFontBook.withHeight (26.0f).withExtraKerningFactor (0.3f));
    g.drawText ("PLUME", plumeTextArea.removeFromTop (plumeTextArea.getHeight()*4/5),
                                   Justification::centred);

    g.setColour (currentTheme.getColour(PLUME::colour::topPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight (12.0f));
    g.drawText (String ("v " + JUCEApplication::getInstance()->getApplicationVersion()),
                plumeTextArea, Justification::centredBottom);
}