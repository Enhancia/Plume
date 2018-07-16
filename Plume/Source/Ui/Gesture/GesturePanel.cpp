/*
  ==============================================================================

    GesturePanel.cpp
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "GesturePanel.h"

//==============================================================================
GesturePanel::GesturePanel()
{
}

GesturePanel::~GesturePanel()
{
}

void GesturePanel::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
}

void GesturePanel::resized()
{
}
