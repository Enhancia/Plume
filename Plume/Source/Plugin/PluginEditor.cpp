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
#define FRAMERATE 60

//==============================================================================
PlumeEditor::PlumeEditor (PlumeProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (800, 600);
    
    // Creates the 3 main components
    addAndMakeVisible (wrapperComp = new WrapperComponent(processor.getWrapper()));
    wrapperComp->setBounds (2*MARGIN, 2*MARGIN, 492, TOP_PANEL);
    
    addAndMakeVisible (presetComp = new PresetComponent(processor));
	presetComp->setBounds(wrapperComp->getWidth() + 3*MARGIN, 2*MARGIN, getWidth() - wrapperComp->getWidth() - 5*MARGIN, TOP_PANEL);
	
    // Adds itself as a change listener for plume's processor
    processor.addChangeListener (this);
}

PlumeEditor::~PlumeEditor()
{
    wrapperComp = nullptr;
    presetComp = nullptr;
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
void PlumeEditor::changeListenerCallback(ChangeBroadcaster* source)
{
    updateFullInterface();
}

//==============================================================================
void PlumeEditor::updateFullInterface()
{
    wrapperComp->update();
    presetComp->update();
}