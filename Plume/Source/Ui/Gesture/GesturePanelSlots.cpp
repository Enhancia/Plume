/*
  ==============================================================================

    GesturePanelSlots.cpp
    Created: 27 May 2019 2:05:16pm
    Author:  Alex

  ==============================================================================
*/

#include "GesturePanelSlots.h"

//==============================================================================
// Gesture Component

GestureComponent::GestureComponent (Gesture& gest, const bool& dragModeReference,
                                                   const int& draggedGestureReference,
                                                   const int& draggedOverSlotReference)
    : gesture (gest), id (gest.id),
                      dragMode (dragModeReference),
                      draggedGesture (draggedGestureReference),
                      draggedOverSlot (draggedOverSlotReference)
{
    createLabel();
}
GestureComponent::~GestureComponent()
{
    gestureNameLabel->removeListener (this);
    gestureNameLabel = nullptr;
}

const String GestureComponent::getInfoString()
{
    return gesture.getName() + " | " + gesture.getTypeString (true) + "\n\n" +
           "State : " + (gesture.isActive() ? "Enabled" : "Disabled") +
           " | Mode : " + (gesture.generatesMidi() ? "MIDI\n" : "Parameters\n")
           + "\n" + gesture.getDescription();
}
void GestureComponent::update()
{
    gestureNameLabel->setText (gesture.getName(), dontSendNotification);
}

void GestureComponent::paint (Graphics& g)
{
    if (selected)
    {
        g.setColour (Colours::white.withAlpha (0.9f));
    }
    else
    {
        g.setColour (highlighted ? Colours::white.withAlpha (0.8f) : Colours::white.withAlpha (0.6f));
    }

    g.fillRoundedRectangle (getLocalBounds().toFloat(), 3.0f);

    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (Colour (0xff6065e0));
        g.drawRoundedRectangle (getLocalBounds().reduced (1.5f).toFloat(), 3.0f, 3.0f);
    }

    auto nameAndTypeArea = getLocalBounds().withHeight(30);

    g.setColour (Colours::black.withAlpha (0.6f));
    g.setFont (PLUME::font::plumeFontLight.withHeight (13.0f));
    g.drawText (gesture.getTypeString (true), nameAndTypeArea.removeFromLeft (getWidth()/4)
                                                             .withTrimmedLeft (PLUME::UI::MARGIN),
                Justification::centredLeft, true);

    nameAndTypeArea.removeFromLeft(getWidth()/2); // gesture Name Label

    auto stateArea = nameAndTypeArea.withHeight (getHeight())
                                    .withTrimmedRight (PLUME::UI::MARGIN);

    g.setFont (PLUME::font::plumeFontLight.withHeight (13.0f));
    g.setColour (Colours::black.withAlpha (0.6f));

    g.drawText (gesture.isActive() ? "On" : "Muted",
                stateArea.removeFromTop (stateArea.getHeight()/2),
                Justification::centredRight, true);
    
    g.drawText (gesture.generatesMidi() ? "MIDI" : "Param",
                stateArea,
                Justification::centredRight, true);
    
}
void GestureComponent::resized()
{
    gestureNameLabel->setBounds (getLocalBounds().withHeight (30)
                                                .withTrimmedLeft (getWidth()/4)
                                                .withWidth (getWidth()/2));
}
void GestureComponent::editorShown (Label* lbl, TextEditor& ted)
{
    ted.setColour (TextEditor::highlightColourId, Colour (0xff101010));
    ted.setColour (TextEditor::textColourId, Colour (0xff959595));
    ted.setJustification (Justification::centred);
}

void GestureComponent::labelTextChanged (Label* lbl)
{
    gesture.setName (gestureNameLabel->getText());
    gestureNameLabel->setText (gesture.getName(), dontSendNotification);
    
    dynamic_cast<PlumeComponent*> (getParentComponent())->update();
}

void GestureComponent::mouseEnter (const MouseEvent &event)
{
    setHighlighted (true);
}
void GestureComponent::mouseExit (const MouseEvent &event)
{
    setHighlighted (false);
}
void GestureComponent::mouseDrag (const MouseEvent &event)
{
    DBG ("Mouse Drag : gesture Comp : id " << id);
}

Gesture& GestureComponent::getGesture()
{
    return gesture;
}
bool GestureComponent::isSelected() const
{
    return selected;
}
void GestureComponent::setSelected (bool shouldBeSelected)
{
    selected = shouldBeSelected;
    repaint();
}
void GestureComponent::setHighlighted (bool shouldBeHighlighted)
{
    highlighted = shouldBeHighlighted;
    repaint();
}
void GestureComponent::setSolo (bool shouldBeSolo)
{
    solo = shouldBeSolo;
    repaint();
}
void GestureComponent::startNameEntry()
{
    gestureNameLabel->showEditor();
}

void GestureComponent::createLabel()
{
    addAndMakeVisible (gestureNameLabel = new Label ("gestureNameLabel", gesture.getName()));
    gestureNameLabel->setEditable (false, false, false);
    gestureNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    gestureNameLabel->setColour (Label::textColourId, Colour (0xff000000));
    gestureNameLabel->setFont (PLUME::font::plumeFontBold.withHeight (15.0f));
    gestureNameLabel->setJustificationType (Justification::centred);
    gestureNameLabel->setInterceptsMouseClicks (false, false);
    gestureNameLabel->addListener (this);
}

//==============================================================================
// Gesture Slot 

EmptyGestureSlotComponent::EmptyGestureSlotComponent (const int slotId,
                                                      const bool& dragModeReference,
                                                      const int& draggedGestureReference,
                                                      const int& draggedOverSlotReference)
    : id (slotId), dragMode (dragModeReference),
                   draggedGesture (draggedGestureReference),
                   draggedOverSlot (draggedOverSlotReference)
{
}
EmptyGestureSlotComponent::~EmptyGestureSlotComponent()
{
}

const String EmptyGestureSlotComponent::getInfoString()
{
    return String();
}
void EmptyGestureSlotComponent::update()
{
}

void EmptyGestureSlotComponent::paint (Graphics& g)
{
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
		g.setColour (Colour (0xff6065e0));
		g.drawRoundedRectangle(getLocalBounds().reduced (1.5f).toFloat(), 3.0f, 3.0f);
    }
    
    g.setColour (highlighted ? Colours::white.withAlpha (0.4f) : Colours::white.withAlpha (0.1f));
    g.fillEllipse (getLocalBounds().withSizeKeepingCentre (20, 20).toFloat());

    if (highlighted) g.drawRoundedRectangle (getLocalBounds().reduced(1).toFloat(), 3.0f, 1.0f);

    g.setColour (Colours::black);
    g.setFont (PLUME::font::plumeFontBold.withHeight (30.0f));
    g.drawText ("+", getLocalBounds(), Justification::centred, true);
}

void EmptyGestureSlotComponent::resized()
{
}

void EmptyGestureSlotComponent::mouseEnter (const MouseEvent &event)
{
    highlighted = true;
    repaint();
}
void EmptyGestureSlotComponent::mouseExit (const MouseEvent &event)
{
    highlighted = false;
    repaint();
}
void EmptyGestureSlotComponent::mouseDrag (const MouseEvent &event)
{
    DBG ("Mouse Drag : empty slot Comp : id " << id);
}