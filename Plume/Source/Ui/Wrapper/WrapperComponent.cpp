/*
  ==============================================================================

    WrapperComponent.cpp
    Created: 26 Jun 2018 5:42:24pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/Wrapper/WrapperComponent.h"

//==============================================================================
WrapperComponent::WrapperComponent(PluginWrapper& wrap)
    : wrapper (wrap)
{
    // Loads all the images for the component
    File f ("D:/Workspace/GitWorkspace/Plume/Plume/Ressources/Images/Wrapper/folder.png");
    Image scan = ImageFileFormat::loadFrom (f);
    
    f = File ("D:/Workspace/GitWorkspace/Plume/Plume/Ressources/Images/Wrapper/eye.png");
    Image editor = ImageFileFormat::loadFrom (f);
    
    // Creates the buttons and the label
    addAndMakeVisible (scanButton = new ImageButton ("Scan Plugin Button"));
    scanButton->setImages (true, true, true, 
                           scan, 1.0f, Colour (0x98000000),
                           scan, 0.7f, Colour (0x10ffffff),
                           scan, 0.7f, Colour (0x501600f0));
    scanButton->addListener (this);
	scanButton->setTopLeftPosition(Point<int>(100, 15));
    
    addAndMakeVisible (pluginNameLabel = new Label ("Plugin Name Label", TRANS ("No plugin")));
    pluginNameLabel->setFont (Font (15.00f, Font::bold).withTypefaceStyle ("Regular"));
    pluginNameLabel->setJustificationType (Justification::centred);
    pluginNameLabel->setEditable (false, false, false);
    pluginNameLabel->setColour (Label::backgroundColourId, Colour (0xff505050));
    pluginNameLabel->setColour (Label::textColourId, Colour (0xffffffff));
    pluginNameLabel->setColour (Label::outlineColourId, Colour (0x00000000));
    pluginNameLabel->setBounds (130, 15, 300, 30);
    
    
    addAndMakeVisible (openEditorButton = new ImageButton ("Open Editor Button"));
    openEditorButton->setImages (true, true, true, 
                           editor, 1.0f, Colour (0x00000000),
                           editor, 0.7f, Colour (0x10ffffff),
                           editor, 0.7f, Colour (0x501600f0));
    openEditorButton->addListener (this);
	openEditorButton->setTopLeftPosition(Point<int>(480, 15));
	
}

WrapperComponent::~WrapperComponent()
{
}

//==============================================================================
void WrapperComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xff909090));

    {
        //g.setColour (Colours::black);
        int width = getHeight()/2, height = getHeight()/2, x = 20, y = getHeight()/4;
        File f ("D:/Workspace/GitWorkspace/Plume/Plume/Ressources/Images/Logo/Logo_ENHANCIA_Round.png");
        Image logo = ImageFileFormat::loadFrom (f);
        g.drawImage (logo,
                     x, y, width, height,
                     0, 0, logo.getWidth(), logo.getHeight());
    }
}

void WrapperComponent::resized()
{
}

//==============================================================================
void WrapperComponent::buttonClicked (Button* bttn)
{
    if (bttn == scanButton)
    {
        scanPlugin();
    }
    
    else if (bttn == openEditorButton)
    {
        openEditor();
    }
}

//==============================================================================
void WrapperComponent::scanPlugin()
{
    // Lets the user chose a file, and changes the plugin path accordingly
    FileChooser pluginScanner ("Select the plugin you want to load.",
                               File::getSpecialLocation (File::currentApplicationFile),
                               "*.dll;*.vst;*.so");
                               
    if (pluginScanner.browseForFileToOpen())
    {
        DBG ("successful scan: "<< pluginScanner.getResult().getFileName());
        
        // Tries to wrap the plugin. If successful chages the label
        if (wrapper.wrapPlugin(pluginScanner.getResult()))
        {
			pluginNameLabel->setText(wrapper.getWrappedPluginName(), dontSendNotification);
        }
    }
}

void WrapperComponent::openEditor()
{
    if (wrapper.isWrapping())
    {
        wrapper.createWrapperEditor();
    }
}