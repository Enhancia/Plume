/*
  ==============================================================================

    NewGesturePanel.cpp
    Created: 28 Mar 2019 11:10:16am
    Author:  Alex

  ==============================================================================
*/

#include "NewGesturePanel.h"

NewGesturePanel::NewGesturePanel (PlumeProcessor& proc) : processor (proc)
{
}

NewGesturePanel::~NewGesturePanel()
{
}

//==============================================================================
// Component Methods
void NewGesturePanel::paint (Graphics&)
{
}
void NewGesturePanel::resized()
{
}

//==============================================================================
// PlumeComponent Methods
const String NewGesturePanel::getInfoString()
{
	return "Gesture creation Panel:\n\n"
		   "- Select a gesture type and add a custom name and description.";
}
void NewGesturePanel::update()
{
}

//==============================================================================
// Callbacks
void NewGesturePanel::visibilityChanged()
{
}
void NewGesturePanel::buttonClicked (Button* bttn)
{
}
void NewGesturePanel::labelTextChanged (Label* lbl)
{
}
void NewGesturePanel::editorShown (Label* lbl, TextEditor& ed)
{
}