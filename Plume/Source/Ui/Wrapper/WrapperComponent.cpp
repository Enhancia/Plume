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
WrapperComponent::WrapperComponent (PluginWrapper& wrap)
    : wrapper (wrap)
{
    TRACE_IN;
    // Loads all the images for the component
    Image scan = ImageFileFormat::loadFrom (PlumeData::folder_png, PlumeData::folder_pngSize);
    Image editor = ImageFileFormat::loadFrom (PlumeData::eye_png, PlumeData::eye_pngSize);
    
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
    pluginNameLabel->addMouseListener (this, false);
    
    addAndMakeVisible (openEditorButton = new ImageButton ("Open Editor Button"));
    openEditorButton->setImages (false, true, false, 
                                 editor, 1.0f, Colour (0x00000000),
                                 editor, 0.7f, Colour (0x10ffffff),
                                 editor, 0.7f, Colour (0x501600f0));
    openEditorButton->addListener (this);
    
    lastPluginDir = File::getSpecialLocation (File::currentApplicationFile).getParentDirectory();
    
}

WrapperComponent::~WrapperComponent()
{
    TRACE_IN;
}

//==============================================================================
void WrapperComponent::paint (Graphics& g)
{
    g.fillAll (Colour (0xff909090));
    
    // Draws Enhancia's Logo
    {
        //g.setColour (Colours::black);
        int width = getWidth()/8 > getHeight()*2/3 ? getHeight()*2/3 : getWidth()/8,
            height = width,
            x = getWidth()/20,
            y = getHeight()/6;
        
        Image logo = ImageFileFormat::loadFrom (PlumeData::Logo_ENHANCIA_Round_png, PlumeData::Logo_ENHANCIA_Round_pngSize);
        
        g.drawImage (logo,
                     x, y, width, height,
                     0, 0, logo.getWidth(), logo.getHeight());
    }
}

void WrapperComponent::resized()
{
    int h = getHeight(), w = getWidth();
    
	scanButton->setBounds (w*3/16, h/3, h/3, h/3);
    pluginNameLabel->setBounds (w*3/16 + h/3, h/3, w*8/16 - h/3, h/3);
	openEditorButton->setBounds (w*3/4, h/4, w*3/16, h/2);
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
		if (wrapper.hasOpenedWrapperEditor())
		{
			closeEditor();
		}
		else
		{
			openEditor();
		}
        
    }
}

void WrapperComponent::mouseUp (const MouseEvent& event)
{
    if (event.originalComponent == pluginNameLabel)
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
                               lastPluginDir,
                               "*.dll;*.vst;*.so",
                               true);
                               
    if (pluginScanner.browseForFileToOpen())
    {
        DBG ("successful scan: "<< pluginScanner.getResult().getFileName());
        
        PLUME::UI::ANIMATE_UI_FLAG = false;
        
        if (wrapper.isWrapping())
        {
            wrapper.unwrapPlugin();
        }
        // Tries to wrap the plugin. If successful changes the label and opens the editor
        if (wrapper.wrapPlugin (pluginScanner.getResult()))
        {
            lastPluginDir = pluginScanner.getResult().getParentDirectory();
			pluginNameLabel->setText(wrapper.getWrappedPluginName(), dontSendNotification);
			openEditor();
        }
        PLUME::UI::ANIMATE_UI_FLAG = true;
    }
}

void WrapperComponent::openEditor()
{
    if (wrapper.isWrapping())
    {
        wrapper.createWrapperEditor();
    }
}

void WrapperComponent::closeEditor()
{
    if (wrapper.isWrapping())
    {
        wrapper.clearWrapperEditor();
    }
}

bool WrapperComponent::hasEditor()
{
    return wrapper.isWrapping();
}

void WrapperComponent::windowToFront ()
{
    if (wrapper.isWrapping() && wrapper.hasOpenedWrapperEditor())
    {
        wrapper.createWrapperEditor();
    }
}

//==============================================================================
void WrapperComponent::update()
{
    pluginNameLabel->setText(wrapper.getWrappedPluginName(), dontSendNotification);
}