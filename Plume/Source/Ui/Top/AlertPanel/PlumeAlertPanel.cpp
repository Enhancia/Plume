/*
  ==============================================================================

    PlumeAlertPanel.cpp
    Created: 25 Sep 2019 4:35:26pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "PlumeAlertPanel.h"

//==============================================================================
PlumeAlertPanel::PlumeAlertPanel (const String& title, const String& message,
                                                     int returnValue,
                                                     const bool hasCloseButton,
                                                     const String& buttonText)
{
    modalReturnValue = returnValue;

    createAndAddTextEditor (message);
    createAndAddLabel (title);
    createAndAddButtons (buttonText, hasCloseButton);
}

PlumeAlertPanel::~PlumeAlertPanel()
{
}

void PlumeAlertPanel::paint (Graphics& g)
{
    // transparent area
    g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelTransparentArea));
    g.fillRect (getBounds());

    // options panel area
    g.setColour (getPlumeColour(topPanelBackground));
    g.fillRoundedRectangle (panelArea.toFloat(), 10.0f);
    
    // options panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               float (panelArea.getX()), 
                                               Colour (0x10ffffff),
                                               float(panelArea.getRight()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (panelArea.reduced (1).toFloat(), 10.0f, 1.0f);
}

void PlumeAlertPanel::resized()
{
    panelArea = getLocalBounds().withSizeKeepingCentre (getLocalBounds().getWidth()/2, getLocalBounds().getHeight()/2);
    
    auto area = panelArea.reduced (PLUME::UI::MARGIN);

    titleLabel->setBounds (area.removeFromTop (area.getHeight()/5));

    if (showButton)
    {
        int buttonHeight = area.getHeight()/8;

        okButton->setBounds (area.removeFromBottom (buttonHeight)
                                    .withSizeKeepingCentre
                                        (okButton->getBestWidthForHeight (buttonHeight),
                                         buttonHeight));
    }

    #if JUCE_WINDOWS
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withRightX (panelArea.getRight() - PLUME::UI::MARGIN_SMALL)
                                                                     .withY (panelArea.getY() + PLUME::UI::MARGIN_SMALL));
    #elif JUCE_MAC
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withPosition (panelArea.getTopLeft()
                                                                                   .translated (PLUME::UI::MARGIN_SMALL,
                                                                                                PLUME::UI::MARGIN_SMALL)));
    #endif

    bodyText->setBounds (area.reduced (area.getWidth()/6, area.getHeight()/6));
}

void PlumeAlertPanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton.get())
    {
        exitModalState (0);
    }

    if (bttn == okButton.get())
    {
        exitModalState (modalReturnValue);
    }
}

void PlumeAlertPanel::createAndAddLabel (const String& textToSet)
{
    titleLabel.reset (new Label ("Title Label", textToSet));
    addAndMakeVisible (*titleLabel);

    titleLabel->setJustificationType (Justification::centred);
    titleLabel->setFont (PLUME::font::plumeFontBold.withHeight (20.0f).withExtraKerningFactor (0.03f));
}

void PlumeAlertPanel::createAndAddTextEditor (const String& textToSet)
{
    bodyText.reset (new Label ("Body Text", textToSet));
    addAndMakeVisible (*bodyText);

    bodyText->setJustificationType (Justification::centred);
}

void PlumeAlertPanel::createAndAddButtons (const String& buttonText, const bool addCloseButton)
{
    if (!buttonText.isEmpty()) showButton = true;

    okButton.reset (new TextButton ("Close Button"));
    addAndMakeVisible (*okButton);

    okButton->setButtonText (buttonText);
    okButton->addListener (this);

    // Close button
    closeButton = std::make_unique <PlumeShapeButton> ("Close Options Button",
                                                       Colour(0),
                                                       getPlumeColour(topPanelMainText));
    addAndMakeVisible (*closeButton);

    Path p;
    p.startNewSubPath (0, 0);
    p.lineTo (3*PLUME::UI::MARGIN, 3*PLUME::UI::MARGIN);
    p.startNewSubPath (0, 3*PLUME::UI::MARGIN);
    p.lineTo (3*PLUME::UI::MARGIN, 0);

    closeButton->setShape (p, false, true, false);
    closeButton->addListener (this);

    closeButton->setVisible (addCloseButton);
}

PlumeAlertPanel* PlumeAlertPanel::createSpecificAlertPanel (SpecificReturnValue panelType)
{
    switch (panelType)
    {
        case missingPlugin:
            return new PlumeAlertPanel ("Missing plugin",
                                       "The preset you just loaded uses a plugin that is not in your plugin list.\n"
                                       "If you own this plugin, rescan your plugins so that Plume can load it with the preset."
                                       "Only the gestures were loaded.",
                                       int (panelType),
                                       true,
                                       "Ok");
        case missingScript:
            return new PlumeAlertPanel ("Missing DAW script",
                                       "Your DAW supports using a custom script to improve your Plume experience.",
                                       int (panelType),
                                       true,
                                       "Ok");
        case scanRequired:
            return new PlumeAlertPanel ("Plugin Scan required.",
                                       "You don't have plugins to use yet!"
                                       "Please perform a plugin scan to use them with Plume.",
                                       int (panelType),
                                       true,
                                       "Ok");
        case mappingOverwrite:
            return new PlumeAlertPanel ("Already mapped",
                                       "This plugin parameter was already assigned to another gesture!"
                                       "Please clear the mapping to assign this parameter to this gesture.",
                                       int (panelType),
                                       true,
                                       "Ok");
        default:
            return new PlumeAlertPanel ("Something went wrong ..",
                                       "Please contact Enhancia about your issue!",
                                       0,
                                       false,
                                       "Ok");
            return nullptr;
    }
}