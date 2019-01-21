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
    setName ("Header");
    setComponentID ("header");
    
    // Plugin Name
    addAndMakeVisible (pluginNameLabel = new Label ("Plugin Name Label", processor.getWrapper().getWrappedPluginInfoString()));
    pluginNameLabel->setFont (Font (PLUME::UI::font, 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    pluginNameLabel->setJustificationType (Justification::centred);
    pluginNameLabel->setEditable (false, false, false);
    pluginNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    pluginNameLabel->setColour (Label::textColourId, PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText));
    pluginNameLabel->addMouseListener (this, false);
    
    // Preset Name
    addAndMakeVisible (presetNameLabel = new Label ("Preset Name Label", processor.getPresetHandler().getCurrentPreset()));
    presetNameLabel->setFont (Font (PLUME::UI::font, 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    presetNameLabel->setJustificationType (Justification::centred);
    presetNameLabel->setEditable (false, false, false);
    presetNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    presetNameLabel->setColour (Label::textColourId, PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText));
    
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
	pluginListButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText), 2.0f);
	pluginListButton->addListener (this);
    
	// Plugin List menu
    createPluginMenu (KnownPluginList::sortByFormat);
	
    // drawPianoAndFolderPath (pianoPath, folderPath);
    
}

HeaderComponent::~HeaderComponent()
{
}

//==============================================================================
const String HeaderComponent::getInfoString()
{
    return "- Displays the current preset and plugin.\n"
           "- Click on the arrow to display all the available plugins to use. If there are none"
           "or they are not up to date, scan them using the option menu (button on the side bar).\n"
           "- Click on the plugin name to open its interface.";
}

void HeaderComponent::update()
{
    pluginNameLabel->setText (processor.getWrapper().getWrappedPluginInfoString(), dontSendNotification);
    presetNameLabel->setText (processor.getPresetHandler().getCurrentPreset(), dontSendNotification);
    createPluginMenu (KnownPluginList::sortByFormat);
}

//==============================================================================
void HeaderComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    
    g.fillAll (currentTheme.getColour(PLUME::colour::headerBackground));
    
    //Gradient for horizontal lines
    auto grad = ColourGradient::vertical (currentTheme.getColour(PLUME::colour::headerSeparatorTop),
                                          MARGIN, 
                                          currentTheme.getColour(PLUME::colour::headerSeparatorBottom),
                                          getHeight() - MARGIN);
    grad.addColour (0.8, currentTheme.getColour(PLUME::colour::headerSeparatorBottom));
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


void HeaderComponent::mouseEnter (const MouseEvent &event)
{
    if (event.eventComponent == pluginNameLabel)
	{
        pluginNameLabel->setColour (Label::textColourId, PLUME::UI::currentTheme.getColour(PLUME::colour::headerHighlightedText));
	}
}

void HeaderComponent::mouseExit (const MouseEvent &event)
{
    if (event.eventComponent == pluginNameLabel)
	{
        pluginNameLabel->setColour (Label::textColourId, PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText));
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

void HeaderComponent::createPluginMenu (KnownPluginList::SortMethod sort)
{
	pluginListMenu.clear();
	pluginListMenu.setLookAndFeel (&getTopLevelComponent()->getLookAndFeel());
	pluginListMenu.addSectionHeader ("Plugins:");
    processor.getWrapper().addPluginsToMenu (pluginListMenu, sort);
}

void HeaderComponent::drawPianoAndFolderPath (Path& pianoPath, Path& folderPath)
{
}