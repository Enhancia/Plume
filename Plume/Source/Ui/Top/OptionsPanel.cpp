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
    
    addAndMakeVisible (scanButton = new ShapeButton ("Scan Button",
                                                     PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill),
		                                             PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill)
		                                                                                .withAlpha (0.5f),
		                                             PLUME::UI::currentTheme.getColour(PLUME::colour::sideBarButtonFill)
		                                                                                .withAlpha (0.7f)));
		                                                                                
	scanButton->setShape (PLUME::path::magnifyingGlassPath, false, true, false);
	scanButton->addListener (this);
	
	addAndMakeVisible (bar = processor.getWrapper().getProgressBar());
}

OptionsPanel::~OptionsPanel()
{
    settings = nullptr;
    scanButton = nullptr;
    bar = nullptr;
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
                
    
    //g.drawRect (area.withHeight (optionsArea.getHeight()/4).reduced (4*UI::MARGIN, 2*UI::MARGIN)); // outline 1
    area.removeFromTop (optionsArea.getHeight()/4); // Custom Dirs
    
    //g.drawRect (area.withHeight (20).reduced (4*UI::MARGIN, 0)); // outline 2
    
    // outline 3
    //g.drawRect (area.withHeight (20).reduced (4*UI::MARGIN, 0)
    //                                .withWidth (optionsArea.getWidth()/3));
    
    
    auto scanArea = area.removeFromTop (20).reduced (4*UI::MARGIN, 0);
    
    g.setFont (Font (15.0f, Font::plain));
    g.drawText ("Scan Plugins :", 
                scanArea.withWidth (scanArea.getWidth()/3),
                Justification::centred, true);
}

void OptionsPanel::resized()
{
    using namespace PLUME::UI;
    
    optionsArea = getBounds().reduced (getWidth()/6, getHeight()/8);
    
    auto area = optionsArea;
    area.removeFromTop (HEADER_HEIGHT); // "Settings" text
    settings->setBounds (area.removeFromTop (optionsArea.getHeight()/4)
                             .reduced (4*MARGIN, 2*MARGIN));
                             
    auto scanArea = area.removeFromTop (20).reduced (4*MARGIN, 0);
    scanArea.removeFromLeft (scanArea.getWidth()/3); // "Scan Plugins" text
    
    scanButton->setBounds (scanArea.removeFromLeft (20));
    
    if (bar != nullptr) bar->setBounds (scanArea.reduced (4*MARGIN, 0));
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
    if (bttn == scanButton)
    {
        processor.getWrapper().scanAllPluginsInDirectories (true, true);
        
        if (auto* header = dynamic_cast<PlumeComponent*> (getParentComponent()->findChildWithID ("header")))
        {
            header->update();
        }
    }
}

void OptionsPanel::textPropertyComponentChanged (TextPropertyComponent* tpc)
{
    // Preset Directory Property
    if (tpc->getName()[8] == 'r')
    {
        if (File::isAbsolutePath (tpc->getText()) && File (tpc->getText()).exists())
        {
            processor.getPresetHandler().setUserDirectory (tpc->getText());
            processor.getPresetHandler().storePresets();
            
            if (auto* sideBar = dynamic_cast<PlumeComponent*> (getParentComponent()->findChildWithID ("sideBar")))
            {
                sideBar->update();
            }
        }
        else
        {
            tpc->setText (processor.getPresetHandler().getUserDirectory().getFullPathName());
        }
    }
    
    // Plugin Directory Property
    else if (tpc->getName()[8] == 'l')
    {
        if (File::isAbsolutePath (tpc->getText()) && File (tpc->getText()).exists())
        {
            processor.getWrapper().addCustomDirectory (tpc->getText());
        }
        else if (tpc->getText() == "")
        {
            processor.getWrapper().clearCustomDirectories();
        }
        else
        {
            processor.getWrapper().clearCustomDirectories();
            //tpc->setText (processor.getWrapper().getCustomDirectory (0));
        }
    }
}

//==============================================================================
void OptionsPanel::createAndAddProperties()
{
    Array<PropertyComponent*> props;
    props.add (new TextPropertyComponent (processor.getParameterTree().state
		                                           .getChildWithName (PLUME::treeId::general)
		                                           .getChildWithName (PLUME::treeId::presetDir)
                                                   .getPropertyAsValue (PLUME::treeId::value, nullptr),
                                          "Custom Preset Directory :", 100, false));
                                          
    props.add (new TextPropertyComponent (processor.getParameterTree().state
		                                           .getChildWithName (PLUME::treeId::general)
		                                           .getChildWithName (PLUME::treeId::pluginDirs)
		                                           .getChildWithName (PLUME::treeId::directory)
                                                   .getPropertyAsValue (PLUME::treeId::value, nullptr),
                                          "Custom Plugin Directory :", 100, false));
                                          
    for (auto* p : props)
    {
        if (auto* tpc = dynamic_cast<TextPropertyComponent*> (p))
        {
            p->setLookAndFeel (&getLookAndFeel());
            p->setColour (PropertyComponent::backgroundColourId, Colour (0x00000000));
            
            tpc->addListener (this);
        }
    }
    
    settings->addProperties (props);
}