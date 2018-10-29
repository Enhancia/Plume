/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"
#include "Plugin/PluginEditor.h"

#define MARGIN 8
#define TOP_PANEL 70
#define FRAMERATE 100


#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
//==============================================================================
PlumeEditor::PlumeEditor (PlumeProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    TRACE_IN;
    
    setLookAndFeel (&plumeLookAndFeel);

    // Creates the 3 main components
    addAndMakeVisible (wrapperComp = new WrapperComponent (processor.getWrapper()));
    addAndMakeVisible (presetComp = new PresetComponent (processor));
	addAndMakeVisible (gesturePanel = new GesturePanel (processor.getGestureArray(), processor.getWrapper(), FRAMERATE));
	gesturePanel->initialize();
	
    // Adds itself as a change listener for plume's processor
    processor.addActionListener (this);

	// Base size and resize settings
	setResizable (true, false);
	addAndMakeVisible (resizableCorner = new ResizableCornerComponent (this, getConstrainer()));
	
	setSize(800, 450);
	setResizeLimits (getWidth()*2/3, getHeight()*2/3, getWidth()*3, getHeight()*3);
}

PlumeEditor::~PlumeEditor()
{
    TRACE_IN;
    
    processor.removeActionListener (this);
    wrapperComp = nullptr;
    presetComp = nullptr;
    gesturePanel = nullptr;
    resizableCorner = nullptr;
    
    setLookAndFeel (nullptr);
}

//==============================================================================
void PlumeEditor::paint (Graphics& g)
{
    // Background
    g.fillAll (plumeLookAndFeel.getPlumeColour (PlumeLookAndFeel::background));
    
    // Version Text
    g.setColour (Colour (0xff000000));
    g.setFont (Font (10.0f, Font::italic).withTypefaceStyle ("Regular"));
    g.drawText ("Plume " + String(JucePlugin_VersionString),
                MARGIN, getHeight() - MARGIN, 100, MARGIN,
                Justification::centredLeft, true);
}

void PlumeEditor::resized()
{
	wrapperComp->setBounds(MARGIN, MARGIN, getWidth() * 3 / 4, TOP_PANEL);
	presetComp->setBounds(getWidth() * 3 / 4 + 2 * MARGIN, MARGIN, getWidth() - getWidth() * 3 / 4 - 3 * MARGIN, TOP_PANEL);
	gesturePanel->setBounds(2 * MARGIN, TOP_PANEL + 3 * MARGIN, getWidth() - 4 * MARGIN, getHeight() - TOP_PANEL - 3 * MARGIN);
	resizableCorner->setBounds (getWidth() - 20, getHeight() - 20, 20, 20);
}

//==============================================================================
void PlumeEditor::actionListenerCallback(const String &message)
{
    TRACE_IN;
    
    if (message.compare("updateInterface") == 0)
    {
        updateFullInterface();
    }
    
    else if (message.compare("blockInterface") == 0)
    {
        gesturePanel->stopTimer();
        gesturePanel.reset();
    }
}

//==============================================================================
PlumeProcessor& PlumeEditor::getProcessor()
{
    return processor;
}

//==============================================================================
void PlumeEditor::updateFullInterface()
{
    TRACE_IN;
        
    wrapperComp->update();
    presetComp->update();

    addAndMakeVisible (gesturePanel = new GesturePanel (processor.getGestureArray(), processor.getWrapper(), FRAMERATE));
    gesturePanel->initialize();
	gesturePanel->setBounds(2 * MARGIN, TOP_PANEL + 3 * MARGIN, getWidth() - 4 * MARGIN, getHeight() - TOP_PANEL - 3 * MARGIN);
}