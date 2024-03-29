/*
  ==============================================================================

    OptionsPanel.cpp
    Created: 24 Jan 2019 5:23:26pm
    Author:  Alex

  ==============================================================================
*/

#include "OptionsPanel.h"

//==============================================================================
OptionsPanel::OptionsPanel (PlumeProcessor& proc, UpdaterPanel& updtrPanel)
    : processor (proc)
{

    tabbedOptions.reset (new TabbedPanelComponent());
    addAndMakeVisible (*tabbedOptions);
    
    tabbedOptions->addTab (new FileOptionsSubPanel (processor), "File");
    tabbedOptions->addTab (new AboutPanel(), "About");
    tabbedOptions->addTab (new UpdaterSubPanel (processor.getUpdater(), updtrPanel), "Update");
    tabbedOptions->setTabAlert (2, processor.getUpdater().hasNewAvailableVersion());

    // Close button
    closeButton.reset (new ShapeButton ("Close Options Button",
                                                       Colour(0),
                                                       Colour(0),
                                                       Colour(0)));
    addAndMakeVisible (*closeButton);

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
    
    optionsArea = getBounds().reduced (getWidth()/5, getHeight()/10);

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
    if (bttn == closeButton.get())
    {
        setVisible (false);
    }
}

bool OptionsPanel::keyPressed (const KeyPress &key)
{
    if (key == PLUME::keyboard_shortcut::closeWindow)
    {
        setVisible (false);
    }

	return true;
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
    if (event.eventComponent == closeButton.get())
    {
        closeButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::topPanelMainText)
                                                      .withAlpha (0.4f), 1.0f);
    }
}

void OptionsPanel::mouseExit (const MouseEvent &event)
{
    if (event.eventComponent == closeButton.get())
    {
        closeButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::topPanelMainText), 1.0f);
    }
}

void OptionsPanel::visibilityChanged()
{
}

TabbedPanelComponent& OptionsPanel::getOptions()
{
    return *tabbedOptions;
}

void OptionsPanel::paintProductInformations(Graphics& g, juce::Rectangle<int> area)
{
    using namespace PLUME::UI;

    // Enhancia Logo
    auto enhanciaArea = area.removeFromLeft (area.getWidth()/2);
    g.drawImage (enhanciaLogo, enhanciaArea.toFloat(), RectanglePlacement::fillDestination);

    // Plume Text
    auto plumeTextArea = area.reduced (MARGIN*2, area.getHeight()/6);
    auto logoArea = plumeTextArea.removeFromTop (plumeTextArea.getHeight()*4/5)
                                 .withSizeKeepingCentre (PLUME::font::plumeFontMedium.withHeight (36.0f)
                                                                               .getStringWidth ("Plume") + MARGIN_SMALL,
                                                         PLUME::UI::HEADER_HEIGHT);
    
    // Plume text
    g.setColour (currentTheme.getColour(PLUME::colour::sideBarMainText));

    g.setFont (PLUME::font::plumeFontMedium.withHeight (36.0f));
    g.drawText ("Plume", logoArea, Justification::centredLeft, true);

    g.setColour (currentTheme.getColour(PLUME::colour::topPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight (12.0f));
    g.drawText (String ("v " + String (JucePlugin_VersionString)),
                plumeTextArea, Justification::centred);
}