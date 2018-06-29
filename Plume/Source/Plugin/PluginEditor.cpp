/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"
#include "Plugin/PluginEditor.h"

#define MARGIN 8

//==============================================================================
PlumeEditor::PlumeEditor (PlumeProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (800, 600);
    
    addAndMakeVisible (wrapperComp = new WrapperComponent(processor.getWrapper()));
    wrapperComp->setBounds (2*MARGIN, 2*MARGIN, (getWidth()-2*MARGIN)*2/3-MARGIN, 60);
}

PlumeEditor::~PlumeEditor()
{
    wrapperComp = nullptr;
}

//==============================================================================
void PlumeEditor::paint (Graphics& g)
{
    g.fillAll (Colour (0xffe0e0e0));
}

void PlumeEditor::resized()
{
}
