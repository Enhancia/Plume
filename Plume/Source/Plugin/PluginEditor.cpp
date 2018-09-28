/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"
#include "Plugin/PluginEditor.h"

#define MARGIN 8
#define TOP_PANEL 80
#define FRAMERATE 80


#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
//==============================================================================
PlumeEditor::PlumeEditor (PlumeProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    TRACE_IN;
    
    setSize (800, 540);
    
    // Creates the 3 main components
    addAndMakeVisible (wrapperComp = new WrapperComponent (processor.getWrapper()));
    wrapperComp->setBounds (2*MARGIN, 2*MARGIN, 492, TOP_PANEL);
    
    addAndMakeVisible (presetComp = new PresetComponent (processor));
	presetComp->setBounds(wrapperComp->getWidth() + 3*MARGIN, 2*MARGIN, getWidth() - wrapperComp->getWidth() - 5*MARGIN, TOP_PANEL);
	
	addAndMakeVisible (gesturePanel = new GesturePanel (processor.getGestureArray(), processor.getWrapper(), FRAMERATE));
	gesturePanel->setBounds(2 * MARGIN, TOP_PANEL + 4*MARGIN, getWidth() - 4*MARGIN, getHeight() - TOP_PANEL - 4*MARGIN);
	gesturePanel->initialize();
	
    // Adds itself as a change listener for plume's processor
    processor.addActionListener (this);
    
    //====================================================================
    // Creates the data reader component, might need to be deleted in the future
    /*
	auto* dataReader = processor.getDataReader();
    addAndMakeVisible (dataReader);
    dataReader->setTopLeftPosition (Point<int> (getWidth()  - dataReader->getWidth()  - 2*MARGIN,
												getHeight() - dataReader->getHeight() - 2*MARGIN));
	*/
}

PlumeEditor::~PlumeEditor()
{
    TRACE_IN;
    
    processor.removeActionListener (this);
    wrapperComp = nullptr;
    presetComp = nullptr;
    gesturePanel = nullptr;
}

//==============================================================================
void PlumeEditor::paint (Graphics& g)
{
    g.fillAll (Colour (0xffe0e0e0));
}

void PlumeEditor::resized()
{
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
	gesturePanel->setBounds(2 * MARGIN, TOP_PANEL + 4*MARGIN, getWidth() - 4*MARGIN, getHeight() - TOP_PANEL - 4*MARGIN);
	gesturePanel->initialize();
}