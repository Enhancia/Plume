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
    // Plugin Name
    addAndMakeVisible (pluginNameLabel = new Label ("Plugin Name Label", processor.getWrapper().getWrappedPluginInfoString()));
    pluginNameLabel->setFont (Font (PLUME::UI::font, 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    pluginNameLabel->setJustificationType (Justification::centred);
    pluginNameLabel->setEditable (false, false, false);
    pluginNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    pluginNameLabel->setColour (Label::textColourId, Colour (0xff393939));
    pluginNameLabel->addMouseListener (this, false);
    
    // Preset Name
    addAndMakeVisible (presetNameLabel = new Label ("Preset Name Label", "No preset"));
    presetNameLabel->setFont (Font (PLUME::UI::font, 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    presetNameLabel->setJustificationType (Justification::centred);
    presetNameLabel->setEditable (false, false, false);
    presetNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    presetNameLabel->setColour (Label::textColourId, Colour (0xff393939));
    
    // Plugin List Button
    Path arrow;
    arrow.startNewSubPath (PLUME::UI::MARGIN, 2*PLUME::UI::MARGIN);
    arrow.lineTo (PLUME::UI::HEADER_HEIGHT/2, PLUME::UI::HEADER_HEIGHT - PLUME::UI::MARGIN);
    arrow.lineTo (PLUME::UI::HEADER_HEIGHT - PLUME::UI::MARGIN, 2*PLUME::UI::MARGIN);
    
    addAndMakeVisible (pluginListButton = new ShapeButton ("Plugin List Button",
                                                           Colour(0x00000000),
		                                                   Colour(0x00000000),
		                                                   Colour(0x00000000)));
	pluginListButton->setShape (arrow, false, true, false);
	pluginListButton->setOutline (Colour (0xff393939), 2.0f);
	pluginListButton->addListener (this);
    
	// Plugin List menu
	pluginListMenu.setLookAndFeel (&getTopLevelComponent()->getLookAndFeel());
	pluginListMenu.addSectionHeader ("Plugins:");
    createPluginMenu();
	
    // drawPianoAndFolderPath (pianoPath, folderPath);
    
}

HeaderComponent::~HeaderComponent()
{
}

//==============================================================================
void HeaderComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    
    g.fillAll (Colour (0xfff8f8f8));
    
    //Gradient for horizontal lines
    auto grad = ColourGradient::vertical (Colour (0xffa5a5a5), MARGIN, 
                                          Colour (0xffe1e1e1), getHeight() - MARGIN);
    grad.addColour (0.8, Colour (0xffe1e1e1));
    g.setGradientFill (grad);
    
    auto area = getLocalBounds().withLeft (getHeight()); 
    
    // Separator
    g.drawVerticalLine (area.getX(),
                        MARGIN,
                        getHeight() - MARGIN);
    
    // Preset folder drawing                   
    Path p (PLUME::path::folderPath);
	p.scaleToFit (area.getX() + 3*MARGIN/2, 0, getHeight()-2*MARGIN, getHeight(), true);
    g.fillPath (p);
    
    // Preset label Outline
    g.drawRoundedRectangle (area.removeFromLeft (area.getWidth()/3).withLeft (area.getX() + getHeight()).reduced (MARGIN).toFloat(), 3, 1);
    
    // Separator
    area.removeFromLeft (MARGIN); // Space before separator
    g.drawVerticalLine (area.getX(),
                        MARGIN,
                        getHeight() - MARGIN);
                        
    area.removeFromRight (getHeight() - MARGIN); // space for the plugin list arrow
    
    // Plugin piano drawing
	p = PLUME::path::pianoPath;
	p.scaleToFit (area.getX() + MARGIN, 0, getHeight()-MARGIN, getHeight(), true);
    g.fillPath (p);
    
    // Plugin label Outline
    g.drawRoundedRectangle (area.withLeft (area.getX() + getHeight()).reduced (MARGIN).toFloat(), 3, 1);           
}

void HeaderComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds().withLeft (getHeight());
                          
    presetNameLabel->setBounds (area.removeFromLeft (area.getWidth()/3).withLeft (area.getX() + getHeight()).reduced (MARGIN));
                                                                                 
    area.removeFromLeft (MARGIN); // Space before separator
    pluginListButton->setBounds (area.removeFromRight (getHeight() - MARGIN).reduced (MARGIN));
    
    pluginNameLabel->setBounds (area.withLeft (area.getX() + getHeight()).reduced (MARGIN));
}

//==============================================================================
void HeaderComponent::mouseUp (const MouseEvent &event)
{
	if (event.eventComponent == pluginNameLabel)
	{
		if (processor.getWrapper().isWrapping())
		{
			processor.getWrapper().createWrapperEditor();
		}
	}
}

void HeaderComponent::buttonClicked (Button* bttn) 
{
    if (bttn == pluginListButton)
    {
        pluginListMenu.showMenuAsync (PopupMenu::Options().withMinimumWidth (100)
                                                          .withMaximumNumColumns (3)
                                                          .withPreferredPopupDirection (PopupMenu::Options::PopupDirection::downwards)
                                                          .withTargetComponent (pluginListButton),
                                      ModalCallbackFunction::forComponent (pluginMenuCallback, this));
    }
}

void HeaderComponent::pluginMenuCallback (int result, HeaderComponent* header)
{
    if (result != 0 && header != nullptr)
    {
		header->handlePluginChoice (result);
    }
}

void HeaderComponent::handlePluginChoice (int chosenId)
{
    if (processor.getWrapper().wrapPlugin (chosenId))
    {
        pluginNameLabel->setText (processor.getWrapper().getWrappedPluginInfoString(), dontSendNotification);
    }
}

//==============================================================================
void HeaderComponent::update()
{
    pluginNameLabel->setText (processor.getWrapper().getWrappedPluginInfoString(), dontSendNotification);
    createPluginMenu();
}


void HeaderComponent::createPluginMenu()
{
    processor.getWrapper().addPluginsToMenu (pluginListMenu);
}

void HeaderComponent::drawPianoAndFolderPath (Path& pianoPath, Path& folderPath)
{
}