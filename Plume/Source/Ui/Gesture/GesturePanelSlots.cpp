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
    createButton();
}

GestureComponent::~GestureComponent()
{
    gestureNameLabel->removeListener (this);
    gestureNameLabel = nullptr;
    muteButton = nullptr;
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
    gestureNameLabel->setText (gesture.getName().toUpperCase(), sendNotification);
    muteButton->setToggleState (gesture.isActive(), sendNotification);
}

void GestureComponent::paint (Graphics& g)
{
    if (!selected && highlighted)
    {
        g.beginTransparencyLayer (0.5f);
    }

    // BackGround + Header Fill
    g.setColour (getPlumeColour (basePanelBackground));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);

    g.saveState();
    g.reduceClipRegion (0, 0, getWidth(), 30);
    g.setColour (getPlumeColour (basePanelHeaderFill));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    g.restoreState();

    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (Colour (0xff6065e0));
        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 3.0f);
    }
    else if (selected)
    {
        g.setColour (gesture.getHighlightColour());
        g.drawRoundedRectangle (getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 1.0f);
    }

    // Text
    auto stateArea = getLocalBounds().withTop (getHeight()-25)
                                     .withLeft (getWidth()/2)
                                     .withTrimmedRight (PLUME::UI::MARGIN);

    g.setFont (PLUME::font::plumeFontLight.withHeight (12.0f));
    g.setColour (getPlumeColour (basePanelSubText));
    
    g.drawText (gesture.generatesMidi() ? "MIDI MODE" : "PARAM MODE",
                stateArea,
                Justification::centredRight, true);

    if (!selected && highlighted)
    {
        g.endTransparencyLayer();
    }
}

void GestureComponent::resized()
{
    auto headerArea = getLocalBounds().removeFromTop (30);

    gestureNameLabel->setBounds (headerArea.withSizeKeepingCentre (getWidth()*2/3, 25));
    muteButton->setBounds (headerArea.removeFromRight (30 + PLUME::UI::MARGIN)
                                     .withSizeKeepingCentre (18, 18));
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
    addAndMakeVisible (gestureNameLabel = new Label ("gestureNameLabel", gesture.getName().toUpperCase()));
    gestureNameLabel->setEditable (false, false, false);
    gestureNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    gestureNameLabel->setColour (Label::textColourId, getPlumeColour (basePanelMainText));
    gestureNameLabel->setFont (PLUME::font::plumeFontBold.withHeight (15.0f));
    gestureNameLabel->setJustificationType (Justification::centred);
    gestureNameLabel->setInterceptsMouseClicks (false, false);
    gestureNameLabel->addListener (this);
}

void GestureComponent::createButton()
{
    addAndMakeVisible (muteButton = new PlumeShapeButton ("Mute Button",
                                                          getPlumeColour (plumeBackground),
                                                          getPlumeColour (mutedHighlight),
                                                          Gesture::getHighlightColour (gesture.type)));

    muteButton->setShape (PLUME::path::createPath (PLUME::path::onOff), false, true, false);
    muteButton->setToggleState (gesture.isActive(), dontSendNotification);
    muteButton->setClickingTogglesState (true);
    muteButton->onClick = [this] ()
    { 
        gesture.setActive (muteButton->getToggleState());
        
        if (selected)
        {
            if (auto* closeButton = dynamic_cast<Button*> (getParentComponent()
    														  ->findChildWithID ("Close Button")))
    			closeButton->setToggleState (gesture.isActive(), dontSendNotification);
        }

        repaint();
    };
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
    Path outline, plusIcon;

    outline.addRoundedRectangle (getLocalBounds().reduced (1.0f), 10.0f);

    plusIcon.startNewSubPath ({getWidth()/2.0f - 10.0f, getHeight()/2.0f});
    plusIcon.lineTo          ({getWidth()/2.0f + 10.0f, getHeight()/2.0f});
    plusIcon.startNewSubPath ({getWidth()/2.0f, getHeight()/2.0f - 10.0f});
    plusIcon.lineTo          ({getWidth()/2.0f, getHeight()/2.0f + 10.0f});

    if (highlighted)
    {
        g.setColour (getPlumeColour (emptySlotBackground));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    }

    g.setColour (getPlumeColour (emptySlotOutline));

    // Plus Icon
    g.strokePath (plusIcon, {3.0f, PathStrokeType::mitered, PathStrokeType::rounded});

    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
		g.setColour (Colour (0xff6065e0));
		g.drawRoundedRectangle(getLocalBounds().reduced (1.0f).toFloat(), 10.0f, 3.0f);
    }

    else //if (highlighted)
    {
        PathStrokeType outlineStroke (1.0f, PathStrokeType::mitered, PathStrokeType::butt);
        Path dashedOutline;
		const float dashLengths[] = {10.0f, 10.0f};

        outlineStroke.createDashedStroke (dashedOutline, outline, dashLengths, 2);

        g.fillPath (dashedOutline);
    }
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
}