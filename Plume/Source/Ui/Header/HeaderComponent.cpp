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
    addAndMakeVisible (pluginNameLabel = new Label ("Plugin Name Label", processor.getWrapper().getWrappedPluginName()));
    pluginNameLabel->setFont (PLUME::font::plumeFont.withHeight (PLUME::font::HEADER_LABEL_FONT_H));
    pluginNameLabel->setJustificationType (Justification::centred);
    pluginNameLabel->setEditable (false, false, false);
    pluginNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    pluginNameLabel->setColour (Label::textColourId, getPlumeColour (headerText));
    pluginNameLabel->addMouseListener (this, false);
    
    // Preset Name
    addAndMakeVisible (presetNameLabel = new Label ("Preset Name Label", processor.getPresetHandler().getCurrentPresetName()));
    presetNameLabel->setFont (PLUME::font::plumeFont.withHeight (PLUME::font::HEADER_LABEL_FONT_H));
    presetNameLabel->setJustificationType (Justification::centred);
    presetNameLabel->setEditable (false, false, false);
    presetNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    presetNameLabel->setColour (Label::textColourId, getPlumeColour (headerText));
    
    // Plugin List Button
    Path arrow;
    arrow.startNewSubPath (PLUME::UI::MARGIN, 2*PLUME::UI::MARGIN);
    arrow.lineTo (PLUME::UI::HEADER_HEIGHT/2, PLUME::UI::HEADER_HEIGHT - PLUME::UI::MARGIN);
    arrow.lineTo (PLUME::UI::HEADER_HEIGHT - PLUME::UI::MARGIN, 2*PLUME::UI::MARGIN);
    
    addAndMakeVisible (pluginListButton = new PlumeShapeButton ("Plugin List Button",
                                                                Colour(0x00000000),
                                                                getPlumeColour (headerButtonStroke)));
	pluginListButton->setShape (arrow, false, false, false);
	pluginListButton->addListener (this);

    // Save Preset Button
    addAndMakeVisible (savePresetButton = new PlumeShapeButton ("Save Preset Button",
                                                                Colour(0x00000000),
                                                                getPlumeColour (headerButtonStroke)));

    savePresetButton->setShape (PLUME::path::createPath (PLUME::path::flatSave), false, true, false);
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
    pluginNameLabel->setText (processor.getWrapper().getWrappedPluginName(), dontSendNotification);
    presetNameLabel->setText (processor.getPresetHandler().getCurrentPresetName(), dontSendNotification);
    createPluginMenu (KnownPluginList::sortByManufacturer);
}

//==============================================================================
void HeaderComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    
    // Background
    g.setColour (getPlumeColour (headerBackground));
    g.fillRoundedRectangle (getLocalBounds().reduced (MARGIN_SMALL).toFloat(), 15.0f);

    g.setColour (getPlumeColour (presetDisplayBackground));

    // Preset Name Area
    g.fillRoundedRectangle (presetNameLabel->getBounds().reduced (0, 6)
                                                        .withLeft (savePresetButton->getX())
                                                        .toFloat(),
                            presetNameLabel->getHeight()/3.0f);

    // Plugin Name Area
    g.reduceClipRegion (getLocalBounds().withLeft (pluginNameLabel->getX()));
    g.setColour (getPlumeColour (pluginDisplayBackground));

    g.fillRoundedRectangle (getLocalBounds().reduced (MARGIN_SMALL).toFloat(), 15.0f);

    /*
    g.fillRoundedRectangle (pluginNameLabel->getBounds().reduced (0, 6)
                                                        .toFloat(),
                            pluginNameLabel->getHeight()/3.0f);*/
}

void HeaderComponent::resized()
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();

    // Plugin Area
    {
        auto pluginArea = area.removeFromRight (getWidth()/4);

        pluginListButton->setBounds (pluginArea.removeFromRight (getHeight()).reduced (MARGIN));
    
        pluginNameLabel->setBounds (pluginArea.reduced (0, MARGIN_SMALL));
    }

    // Preset Area
    {
        auto presetArea = getLocalBounds().withSizeKeepingCentre (200, area.getHeight());
        
        savePresetButton->setBounds (presetArea.removeFromLeft (HEADER_HEIGHT).reduced (MARGIN_SMALL));
        presetNameLabel->setBounds (presetArea.reduced (0, MARGIN_SMALL));
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
                                    getPlumeColour (headerText).withAlpha (0.5f));
	}
}

void HeaderComponent::mouseExit (const MouseEvent &event)
{
    if (event.eventComponent == pluginNameLabel)
	{
        pluginNameLabel->setColour (Label::textColourId,
                                    getPlumeColour (headerText));
	}
}

void HeaderComponent::buttonClicked (Button* bttn) 
{
    if (auto* shapeBttn = dynamic_cast<ShapeButton*> (bttn))
    {
        if (shapeBttn == pluginListButton)
        {
            //shapeBttn->setOutline (getPlumeColour (headerText), 2.0f);
        }
        else if (shapeBttn == savePresetButton)
        {
            //shapeBttn->setOutline (getPlumeColour (headerText), 1.7f);
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
        pluginListButton->setOutline (getPlumeColour (headerText), 2.0f);
    }
    else
    {
        // Removes the window in case it's opened
        processor.getWrapper().clearWrapperEditor();


        if (processor.getWrapper().wrapPlugin (chosenId))
        {
            pluginNameLabel->setText (processor.getWrapper().getWrappedPluginName(), dontSendNotification);

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
