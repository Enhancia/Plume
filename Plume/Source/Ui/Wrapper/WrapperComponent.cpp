/*
  ==============================================================================

    WrapperComponent.cpp
    Created: 26 Jun 2018 5:42:24pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/Wrapper/WrapperComponent.h"

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
//==============================================================================
WrapperComponent::WrapperComponent(PluginWrapper& wrap)
    : wrapper (wrap)
{
    TRACE_IN;
    // Loads all the images for the component
    String PlumeDir = File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getFullPathName();
    
    File f (PlumeDir + "/Resources/Images/Wrapper/folder.png");
    Image scan = ImageFileFormat::loadFrom (f);
    
    f = File (PlumeDir + "/Resources/Images/Wrapper/eye.png");
    Image editor = ImageFileFormat::loadFrom (f);
    
    // Creates the buttons and the label
    addAndMakeVisible (scanButton = new ImageButton ("Scan Plugin Button"));
    scanButton->setImages (false, true, false, 
                           scan, 1.0f, Colour (0x00000000),
                           scan, 0.7f, Colour (0x00000000),
                           scan, 0.7f, Colour (0x501600f0));
    scanButton->addListener (this);
    
    addAndMakeVisible (pluginNameLabel = new Label ("Plugin Name Label", TRANS (wrapper.getWrappedPluginName())));
    pluginNameLabel->setFont (Font (15.00f, Font::bold).withTypefaceStyle ("Regular"));
    pluginNameLabel->setJustificationType (Justification::centred);
    pluginNameLabel->setEditable (false, false, false);
    pluginNameLabel->setColour (Label::backgroundColourId, Colour (0xff323232));
    
    addAndMakeVisible (openEditorButton = new ImageButton ("Open Editor Button"));
    openEditorButton->setImages (false, true, false, 
                                 editor, 1.0f, Colour (0x00000000),
                                 editor, 0.7f, Colour (0x10ffffff),
                                 editor, 0.7f, Colour (0x501600f0));
    openEditorButton->addListener (this);
}

WrapperComponent::~WrapperComponent()
{
    TRACE_IN;
}

//==============================================================================
void WrapperComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xff909090));

    {
        //g.setColour (Colours::black);
        int width = getWidth()/8 > getHeight()*2/3 ? getHeight()*2/3 : getWidth()/8,
            height = width,
            x = getWidth()/20,
            y = getHeight()/6;
        
        String PlumeDir = File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getFullPathName();
        File f (PlumeDir + "/Resources/Images/Logo/Logo_ENHANCIA_Round.png");
        Image logo = ImageFileFormat::loadFrom (f);
        
        g.drawImage (logo,
                     x, y, width, height,
                     0, 0, logo.getWidth(), logo.getHeight());
    }
}

void WrapperComponent::resized()
{
    int h = getHeight(), w = getWidth();
    
	scanButton->setBounds (w*3/16 + 2, h/3, w/16, h/3);
    pluginNameLabel->setBounds (w*1/4, h/3, w*7/16, h/3);
	openEditorButton->setBounds (w*23/32, h/4, w/4, h/2);
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
    TRACE_IN;
    // Lets the user chose a file, and changes the plugin path accordingly
    FileChooser pluginScanner ("Select the plugin you want to load.",
                               File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getParentDirectory(),
                               "*.dll;*.vst;*.so",
                               true);
                               
    if (pluginScanner.browseForFileToOpen())
    {
        DBG ("successful scan: "<< pluginScanner.getResult().getFileName());
        
        // Tries to wrap the plugin. If successful changes the label and opens the editor
        if (wrapper.wrapPlugin(pluginScanner.getResult()))
        {
			pluginNameLabel->setText(wrapper.getWrappedPluginName(), dontSendNotification);
			openEditor();
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

//==============================================================================
void WrapperComponent::update()
{
    pluginNameLabel->setText(wrapper.getWrappedPluginName(), dontSendNotification);
}