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
    optionsArea = getBounds().reduced (getWidth()/4, getHeight()/6);
    
    // Labels
    addAndMakeVisible (presetDirLabel = new Label ("Preset Directory Label", processor.getPresetHandler()
                                                                                      .getUserDirectory()
                                                                                      .getFullPathName()));
    presetDirLabel->setEditable (true, false, true);                                                                           
    presetDirLabel->addListener (this);
                                                                                      
    addAndMakeVisible (pluginDirLabel = new Label ("Plugin Directory Label", processor.getWrapper()
                                                                                      .getCustomDirectory (0)));
    pluginDirLabel->setEditable (true, false, true);
    pluginDirLabel->addListener (this);
    
    // Scanner
    addAndMakeVisible (scanner = new ScannerComponent (processor));
}

OptionsPanel::~OptionsPanel()
{
    scanButton = nullptr;
    scanner = nullptr;
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
    auto area = optionsArea;
    
    g.setColour (UI::currentTheme.getColour (colour::topPanelMainText));
    g.setFont (Font (UI::font, 20.0f, Font::plain));
    g.drawText ("Settings :", 
                area.removeFromTop (UI::HEADER_HEIGHT).reduced (2*UI::MARGIN),
                Justification::topLeft, true);
    
    auto labelArea = area.removeFromTop (jmax (optionsArea.getHeight()/4, 40 + UI::MARGIN)).reduced (4*UI::MARGIN, 0);
    labelArea.removeFromRight (labelArea.getWidth()*2/3);
    
    g.setFont (Font (UI::font, 15.0f, Font::plain));
    g.drawText ("User Presets Path :", 
                labelArea.removeFromTop (labelArea.getHeight()/2),
                Justification::centredLeft, true);
                
    g.drawText ("Plugins Path :", 
                labelArea,
                Justification::centredLeft, true);
    
    auto scanArea = area.removeFromTop (20 + 2*UI::MARGIN).reduced (4*UI::MARGIN, UI::MARGIN);
    
    g.drawText ("Scan Plugins :", 
                scanArea.withWidth (scanArea.getWidth()/3),
                Justification::centredLeft, true);
}

void OptionsPanel::resized()
{
    using namespace PLUME::UI;
    
    optionsArea = getBounds().reduced (getWidth()/6, getHeight()/8);
    
    auto area = optionsArea;
    area.removeFromTop (HEADER_HEIGHT); // "Settings" text
    
    auto labelArea = area.removeFromTop (jmax (optionsArea.getHeight()/4, 40 + UI::MARGIN)).reduced (4*UI::MARGIN, 0); // Custom Dirs
    labelArea.removeFromLeft (labelArea.getWidth()/3);
    
    presetDirLabel->setBounds (labelArea.withHeight (20).translated (0, labelArea.getHeight()/4 - 10));
    pluginDirLabel->setBounds (labelArea.withHeight (20).translated (0, labelArea.getHeight()*3/4 - 10));
                             
    auto scanArea = area.removeFromTop (20 + 2*UI::MARGIN).reduced (4*UI::MARGIN, UI::MARGIN);
    scanArea.removeFromLeft (scanArea.getWidth()/3); // "Scan Plugins" text
    
    scanner->setBounds (scanArea);
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

void OptionsPanel::buttonClicked (Button* bttn)
{
}

void OptionsPanel::labelTextChanged (Label* lbl)
{
    // Preset Directory Label
    if (lbl == presetDirLabel)
    {
        if (File::isAbsolutePath (lbl->getText()) && File (lbl->getText()).exists())
        {
            processor.getPresetHandler().setUserDirectory (lbl->getText());
            processor.getPresetHandler().storePresets();
            
            if (auto* sideBar = dynamic_cast<PlumeComponent*> (getParentComponent()->findChildWithID ("sideBar")))
            {
                sideBar->update();
            }
        }
        else
        {
            lbl->setText (processor.getPresetHandler().getUserDirectory().getFullPathName(), dontSendNotification);
        }
    }
    
    // Plugin Directory Label
    if (lbl == pluginDirLabel)
    {
        if (File::isAbsolutePath (lbl->getText()) && File (lbl->getText()).exists())
        {
            processor.getWrapper().addCustomDirectory (lbl->getText());
        }
        else if (lbl->getText() == "")
        {
            processor.getWrapper().clearCustomDirectories();
        }
        else
        {
            lbl->setText (processor.getWrapper().getCustomDirectory (0), dontSendNotification);
        }
    }
}