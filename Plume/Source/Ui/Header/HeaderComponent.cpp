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
HeaderComponent::HeaderComponent (PlumeProcessor& proc, Component& newPrst)  : processor (proc),
                                                                               newPresetPanel (newPrst)
{
    setName ("Header");
    setComponentID ("header");
    
    // Plugin Name
    addAndMakeVisible (pluginNameLabel = new Label ("Plugin Name Label", processor.getWrapper().getWrappedPluginInfoString()));
    pluginNameLabel->setFont (PLUME::font::plumeFont.withHeight (PLUME::font::HEADER_LABEL_FONT_H));
    pluginNameLabel->setJustificationType (Justification::centred);
    pluginNameLabel->setEditable (false, false, false);
    pluginNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    pluginNameLabel->setColour (Label::textColourId, PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText));
    pluginNameLabel->addMouseListener (this, false);
    
    // Preset Name
    addAndMakeVisible (presetNameLabel = new Label ("Preset Name Label", processor.getPresetHandler().getCurrentPresetName()));
    presetNameLabel->setFont (PLUME::font::plumeFont.withHeight (PLUME::font::HEADER_LABEL_FONT_H));
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
	pluginListButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText), 2.0f);
	pluginListButton->addMouseListener (this, false);
	pluginListButton->addListener (this);

    // Save Preset Button
    addAndMakeVisible (savePresetButton = new ShapeButton ("Save Preset Button",
                                                           Colour(0x00000000),
                                                           Colour(0x00000000),
                                                           Colour(0x00000000)));
                                                           //Colour(PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText)),
                                                           //Colour(PLUME::UI::currentTheme.getColour(PLUME::colour::headerHighlightedText)),
                                                           //Colour(PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText))));
    savePresetButton->setShape (PLUME::path::createFlatSavePath(), false, true, false);
    savePresetButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText), 1.5f);
    savePresetButton->addMouseListener (this, false);
    savePresetButton->addListener (this);
    
	// Plugin List menu
    createPluginMenu (KnownPluginList::sortByManufacturer);
    
}

HeaderComponent::~HeaderComponent()
{
    pluginNameLabel = nullptr;
    presetNameLabel = nullptr;
    pluginListButton = nullptr;
    savePresetButton = nullptr;
}

//==============================================================================
const String HeaderComponent::getInfoString()
{
    return "Header :\n\n"
           "- Click on the arrow to display all the available plugins to use.\n"
           "- Click on the plugin name to open its interface.\n";
}

void HeaderComponent::update()
{
    pluginNameLabel->setText (processor.getWrapper().getWrappedPluginInfoString(), dontSendNotification);
    presetNameLabel->setText (processor.getPresetHandler().getCurrentPresetName(), dontSendNotification);
    createPluginMenu (KnownPluginList::sortByManufacturer);
}

//==============================================================================
void HeaderComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    
    g.fillAll (currentTheme.getColour(PLUME::colour::headerBackground));
    
    //Gradient for horizontal lines
    auto grad = ColourGradient::vertical (currentTheme.getColour(PLUME::colour::headerSeparatorTop),
                                          float(MARGIN), 
                                          currentTheme.getColour(PLUME::colour::headerSeparatorBottom),
                                          float(getHeight() - MARGIN));
    grad.addColour (0.8, currentTheme.getColour(PLUME::colour::headerSeparatorBottom));
    g.setGradientFill (grad);
    
    auto area = getLocalBounds();

    // Preset area
    {
        auto presetArea = area.removeFromLeft (area.getWidth()/3 + 2*MARGIN);

        // Preset folder drawing
        Path p = PLUME::path::createFolderPath();
	    p.scaleToFit (float(presetArea.getX()) + MARGIN, 0.0f, float(getHeight()-2*MARGIN), float(getHeight()), true);
        g.fillPath (p);
        presetArea.removeFromLeft (HEADER_HEIGHT);
    
        // Save button
        presetArea.removeFromRight (HEADER_HEIGHT);

        // Preset label Outline
        g.drawRoundedRectangle (presetArea.reduced (0, MARGIN).toFloat(), 3, 1);
    }

    // Separator
    g.drawVerticalLine (area.getX(),
                        float(MARGIN),
                        float(getHeight() - MARGIN));
    
    // Plugin Area
    {
        area.removeFromRight (getHeight()); // space for the plugin list arrow
    
        // Plugin piano drawing
	    Path p = PLUME::path::createPianoPath();
	    p.scaleToFit (float(area.getX() + MARGIN), 0.0f, float(getHeight()-2*MARGIN), float(getHeight()), true);
        g.fillPath (p);
    
        // Plugin label Outline
        g.drawRoundedRectangle (area.withLeft (area.getX() + getHeight()).reduced (0, MARGIN).toFloat(), 3, 1);
    }           
}

void HeaderComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();

    // Preset Area
    {
        auto presetArea = area.removeFromLeft (area.getWidth()/3 + 2*MARGIN);
        
        savePresetButton->setBounds (presetArea.removeFromRight (HEADER_HEIGHT).reduced (0, MARGIN*3/2));
        presetNameLabel->setBounds (presetArea.withTrimmedLeft (HEADER_HEIGHT)
                                              .reduced (0, MARGIN));
    }

    // Plugin Area
    {
        //area.removeFromLeft (MARGIN); // Space before separator
        pluginListButton->setBounds (area.removeFromRight (getHeight()).reduced (MARGIN*3/2, MARGIN));
    
        pluginNameLabel->setBounds (area.withTrimmedLeft (HEADER_HEIGHT).reduced (0, MARGIN));
    }
}

//==============================================================================
void HeaderComponent::mouseUp (const MouseEvent &event)
{
	if (event.eventComponent == pluginNameLabel)
	{
		if (processor.getWrapper().isWrapping())
		{
			if (auto* editor = getParentComponent())
			{
				processor.getWrapper().createWrapperEditor(editor);
			}
        }
	}
}


void HeaderComponent::mouseEnter (const MouseEvent &event)
{
    if (event.eventComponent == pluginNameLabel && processor.getWrapper().isWrapping())
	{
        pluginNameLabel->setColour (Label::textColourId,
                                    PLUME::UI::currentTheme.getColour (PLUME::colour::headerHighlightedText));
	}
	
	else if (event.eventComponent == pluginListButton)
	{
        pluginListButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerHighlightedText), 2.0f);
	}

    if (event.eventComponent == savePresetButton)
    {
        savePresetButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerHighlightedText), 1.7f);
    }
}

void HeaderComponent::mouseExit (const MouseEvent &event)
{
    if (event.eventComponent == pluginNameLabel)
	{
        pluginNameLabel->setColour (Label::textColourId,
                                    PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText));
	}
	
	else if (event.eventComponent == pluginListButton)
	{
        pluginListButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText), 2.0f);
	}

    if (event.eventComponent == savePresetButton)
    {
        savePresetButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText), 1.7f);
    }
}

void HeaderComponent::buttonClicked (Button* bttn) 
{
    if (auto* shapeBttn = dynamic_cast<ShapeButton*> (bttn))
    {
        if (shapeBttn == pluginListButton)
        {
            shapeBttn->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText), 2.0f);
        }
        else if (shapeBttn == savePresetButton)
        {
            shapeBttn->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText), 1.7f);
        }
    }

    if (bttn == pluginListButton)
    {
        pluginListMenu.showMenuAsync (PopupMenu::Options().withParentComponent (getParentComponent())
                                                          .withMinimumWidth (100)
                                                          .withMaximumNumColumns (3)
                                                          .withPreferredPopupDirection
                                                              (PopupMenu::Options::PopupDirection::downwards)
                                                          .withTargetScreenArea
                                                              (juce::Rectangle<int> (getScreenX() + getWidth()-2,
                                                                                     getScreenY() + getHeight(),
                                                                                     1, 1)),
                                      ModalCallbackFunction::forComponent (pluginMenuCallback, this));
    }

    if (bttn == savePresetButton)
    {
        newPresetPanel.setVisible (true);
    }
}

void HeaderComponent::pluginMenuCallback (int result, HeaderComponent* header)
{
    if (header != nullptr)
    {
		header->handlePluginChoice (result);
    }
}

void HeaderComponent::handlePluginChoice (int chosenId)
{
    if (chosenId == 0)
    {   
        // resets the button colour if no choice was made
        pluginListButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText), 2.0f);
    }
    else
    {
        // Removes the window in case it's opened
        processor.getWrapper().clearWrapperEditor();


        if (processor.getWrapper().wrapPlugin (chosenId))
        {
            pluginNameLabel->setText (processor.getWrapper().getWrappedPluginInfoString(), dontSendNotification);

            // Creates the wrapper editor object as a child of the editor
            createPluginWindow();
        }
    }
}

void HeaderComponent::createPluginMenu (KnownPluginList::SortMethod sort)
{
	pluginListMenu.clear();
	pluginListMenu.addSectionHeader ("Plugins:");
    processor.getWrapper().addPluginsToMenu (pluginListMenu, sort);
}


void HeaderComponent::createPluginWindow()
{
    if (auto* editor = getParentComponent())
    {
        processor.getWrapper().createWrapperEditor (editor);
    }
}
