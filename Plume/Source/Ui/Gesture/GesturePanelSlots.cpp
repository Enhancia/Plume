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

GestureComponent::GestureComponent (Gesture& gest, GestureArray& gestArray,
                                    const bool& dragModeReference,
                                    const int& draggedGestureReference,
                                    const int& draggedOverSlotReference)
    : gestureArray (gestArray),
      id (gest.id), gesture (gest),
      dragMode (dragModeReference),
      draggedGesture (draggedGestureReference),
      draggedOverSlot (draggedOverSlotReference)
{
    setComponentID ("gestComp" + String(id));
    createLabel();
    createButton();
}

GestureComponent::~GestureComponent()
{
    gestureNameLabel->removeListener (this);
    gestureNameLabel = nullptr;
    muteButton->removeMouseListener (this);
    muteButton = nullptr;
}

const String GestureComponent::getInfoString()
{
    if (auto* gesturePtr = gestureArray.getGesture (id))
    {
        return gesturePtr->getName() + " | " + gesturePtr->getTypeString (true) + "\n\n" +
               "State : " + (gesturePtr->isActive() ? "Enabled" : "Disabled") +
               " | Mode : " + (gesturePtr->generatesMidi() ? "MIDI\n" : "Parameters\n")
               + "\n" + gesturePtr->getDescription();
    }
    else return "";

}

void GestureComponent::update()
{
    gestureNameLabel->setText (gesture.getName().toUpperCase(), sendNotification);
    muteButton->setToggleState (gesture.isActive(), sendNotification);
    repaint();
}

void GestureComponent::paint (Graphics& g)
{
    // BackGround + Header Fill
    g.setColour (getPlumeColour (basePanelBackground));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);

    // Gesture Image
    Path roundedRectangleBackground;
    roundedRectangleBackground.addRoundedRectangle (selected ? getLocalBounds().toFloat().reduced (1.0f)
                                                             : getLocalBounds().toFloat(),
                                                    10.0f);

    g.saveState();
    g.reduceClipRegion (roundedRectangleBackground);
    drawGesturePath (g, getLocalBounds().withSizeKeepingCentre (jmax (getWidth(), getHeight()),
                                                                jmax (getWidth(), getHeight())));
    g.restoreState();

    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (gestureArray.getGesture (draggedGesture)->getHighlightColour());
        g.drawRoundedRectangle (getLocalBounds().toFloat().reduced(1.0f), 10.0f, 3.0f);
    }
    else if (selected)
    {
        g.setColour (gesture.getHighlightColour());
        g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (1.0f), 10.0f, 1.0f);
    }

    auto area = getLocalBounds().withTrimmedTop (30);

    // Bottom display
    auto stateArea = area.removeFromBottom (25)
                         .reduced (PLUME::UI::MARGIN*3, PLUME::UI::MARGIN_SMALL);

    g.setFont (PLUME::font::plumeFontLight.withHeight (12.0f));
    g.setColour (getPlumeColour (basePanelSubText));
    
    if (gesture.generatesMidi())
    {
        g.drawText (gesture.midiType == Gesture::pitch ? "Pitch MIDI"
                                                       : "CC " + String (gesture.getCc()) + " MIDI",
                    stateArea, Justification::centred, true);
    }
    else
    {
        paintParameterSlotDisplay (g, stateArea, 1, 6, PLUME::UI::MARGIN);
    }

    // Highlight
    if (!selected && highlighted)
    {
        g.setColour (Colours::white.withAlpha (0.05f));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    }
}

void GestureComponent::resized()
{
    auto headerArea = getLocalBounds().removeFromTop (30);

    gestureNameLabel->setBounds (getLocalBounds().withSizeKeepingCentre (getWidth()*2/3, 32));
    muteButton->setBounds (headerArea.removeFromRight (30 + PLUME::UI::MARGIN)
                                     .withSizeKeepingCentre (18, 18));
}
void GestureComponent::editorShown (Label*, TextEditor& ted)
{
    ted.setColour (TextEditor::highlightColourId, Colour (0xff101010));
    ted.setColour (TextEditor::textColourId, Colour (0xff959595));
    ted.setJustification (Justification::centred);
    ted.setInputRestrictions(26);
}

void GestureComponent::labelTextChanged (Label*)
{
    auto truncatedName = gestureNameLabel->getText().substring(0, 26);

    gesture.setName(truncatedName);
    gestureNameLabel->setText(truncatedName.toUpperCase(), dontSendNotification);
    
    dynamic_cast<PlumeComponent*> (getParentComponent())->update();
}

void GestureComponent::mouseEnter (const MouseEvent &)
{
    if (isMouseOver (true)) setHighlighted (true);
}
void GestureComponent::mouseExit (const MouseEvent &)
{
    if (!isMouseOver (true))
        setHighlighted (false);
}
void GestureComponent::mouseDrag (const MouseEvent &)
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
    addAndMakeVisible (*(gestureNameLabel = std::make_unique<Label> ("gestureNameLabel", gesture.getName().toUpperCase())));
    gestureNameLabel->setEditable (false, false, false);
    gestureNameLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    gestureNameLabel->setColour (Label::textColourId, getPlumeColour (basePanelMainText));
    gestureNameLabel->setFont (PLUME::font::plumeFontMedium.withHeight (15.0f).withExtraKerningFactor (0.06f));
    gestureNameLabel->setJustificationType (Justification::centred);
    gestureNameLabel->setInterceptsMouseClicks (false, false);
    gestureNameLabel->addListener (this);
}

void GestureComponent::createButton()
{
    addAndMakeVisible (*(muteButton = std::make_unique<PlumeShapeButton> ("Mute Button",
                                                          getPlumeColour (plumeBackground),
                                                          Gesture::getHighlightColour (gesture.type, false),
                                                          Gesture::getHighlightColour (gesture.type))));

    muteButton->setShape (PLUME::path::createPath (PLUME::path::onOff), false, true, false);
    muteButton->setToggleState (gesture.isActive(), dontSendNotification);
    muteButton->setGetsHighlighted (false);
    muteButton->setClickingTogglesState (true);
    muteButton->addMouseListener (this, false);
    muteButton->onClick = [this] ()
    { 
        gesture.setActive (muteButton->getToggleState());

        PLUME::log::writeToLog ("Gesture " + gesture.getName() + " (Id " + String (gesture.id) + (muteButton->getToggleState() ? ") Muting." : ") Unmuting."),
                                PLUME::log::LogCategory::gesture);

        if (selected)
        {
            if (auto* closeButton = dynamic_cast<Button*> (getParentComponent()
    														  ->findChildWithID ("Close Button")))
    			closeButton->setToggleState (gesture.isActive(), dontSendNotification);
            
            if (auto* gestureSettings = dynamic_cast<PlumeComponent*> (getParentComponent()
                                                                         ->findChildWithID ("Gesture Settings")))
                gestureSettings->update();
        }

        repaint();
    };
}

void GestureComponent::paintParameterSlotDisplay  (Graphics& g, juce::Rectangle<int> area,
                                                            const int numRows,
                                                            const int numColumns,
                                                            const int sideLength)
{
    /*  Hitting this assert means you're trying to paint this object with a number of
        parameters that doesn't match the actual maximum number of parameters allowed
        for a gesture.
    */
    jassert (numRows * numColumns == PLUME::MAX_PARAMETER);

    int rowHeight = area.getHeight()/numRows;
    int columnWidth = area.getWidth()/numColumns;

    for (int row=0; row < numRows; row++)
    {
        auto columnArea = area.removeFromTop (rowHeight);

        for (int column=0; column < numColumns; column++)
        {
            int slotSide = jmin (sideLength, rowHeight - 8, columnWidth - 8);
            auto slotArea = columnArea.removeFromLeft (columnWidth)
                                      .withSizeKeepingCentre (slotSide, slotSide);

            g.setColour ((row*numColumns) + column < gesture.getParameterArray().size() ?
                            gesture.getHighlightColour() :
                            getPlumeColour (plumeBackground));
            g.fillRoundedRectangle (slotArea.toFloat(), sideLength / 3.5f);
        }
    }
}

void GestureComponent::drawGesturePath (Graphics& g, juce::Rectangle<int> area)
{
    Path gesturePath;

    switch (gesture.type)
    {
        case (int (Gesture::tilt)):
            gesturePath = PLUME::path::createPath (PLUME::path::tilt);
            break;

        case (int (Gesture::vibrato)):
            gesturePath = PLUME::path::createPath (PLUME::path::vibrato);
            break;

        case (int (Gesture::pitchBend)):
            gesturePath = PLUME::path::createPath (PLUME::path::pitchBend);
            break;

        case (int (Gesture::roll)):
            gesturePath = PLUME::path::createPath (PLUME::path::roll);
            break;

        default:
            return;
    }

    gesturePath.scaleToFit (area.toFloat().getX(),
                            area.toFloat().getY(),
                            area.toFloat().getWidth(),
                            area.toFloat().getHeight(),
                            false);

    Colour pathColour (0xff808080);
    ColourGradient gesturePathGradient (pathColour.withAlpha (0.4f),
                                        {area.toFloat().getX(),
                                         area.toFloat().getY() + area.toFloat().getHeight() },
                                        pathColour.withAlpha (0.4f),
                                        {area.toFloat().getX() + area.toFloat().getWidth(),
                                         area.toFloat().getY()},
                                        false);

    gesturePathGradient.addColour (0.35, pathColour.withAlpha (0.0f));
    gesturePathGradient.addColour (0.65, pathColour.withAlpha (0.0f));

    g.setGradientFill (gesturePathGradient);
    g.strokePath (gesturePath, PathStrokeType (2.0f));
}

//==============================================================================
// Gesture Slot 

EmptyGestureSlotComponent::EmptyGestureSlotComponent (const int slotId,
                                                      GestureArray& gestArray,
                                                      const bool& dragModeReference,
                                                      const int& draggedGestureReference,
                                                      const int& draggedOverSlotReference)
    : id (slotId), gestureArray (gestArray),
                   dragMode (dragModeReference),
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

    outline.addRoundedRectangle (getLocalBounds().toFloat().reduced(1.0f), 10.0f);

    plusIcon.startNewSubPath ({getWidth()/2.0f - 10.0f, getHeight()/2.0f});
    plusIcon.lineTo          ({getWidth()/2.0f + 10.0f, getHeight()/2.0f});
    plusIcon.startNewSubPath ({getWidth()/2.0f, getHeight()/2.0f - 10.0f});
    plusIcon.lineTo          ({getWidth()/2.0f, getHeight()/2.0f + 10.0f});

    if (highlighted)
    {
        g.setColour (getPlumeColour (emptySlotBackground));
        g.fillRoundedRectangle (getLocalBounds().toFloat(), 10.0f);
    }


    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (gestureArray.getGesture (draggedGesture)->getHighlightColour());
    }
    else
    {
        g.setColour (getPlumeColour (emptySlotOutline));
    }

    // Plus Icon
    g.strokePath (plusIcon, {2.0f, PathStrokeType::mitered, PathStrokeType::rounded});

    // Shadow
    Path shadowPath;
    shadowPath.addRectangle (getLocalBounds().expanded (3));
    shadowPath.addRoundedRectangle (getLocalBounds().reduced (5).toFloat(), 6.0f);
    shadowPath.setUsingNonZeroWinding (false);

    g.saveState();
    g.reduceClipRegion (outline);
    DropShadow (Colour (0x30000000), 15, {0, 0}).drawForPath (g, shadowPath);
    g.restoreState();
    
    // Outline
    if (dragMode && draggedGesture != id && draggedOverSlot == id)
    {
        g.setColour (gestureArray.getGesture (draggedGesture)->getHighlightColour());
		g.drawRoundedRectangle(getLocalBounds().toFloat().reduced (1.0f), 10.0f, 3.0f);
    }

    else //if (highlighted)
    {
        g.setColour (getPlumeColour (emptySlotOutline));
        PathStrokeType outlineStroke (1.0f, PathStrokeType::mitered, PathStrokeType::butt);
        Path dashedOutline;
		const float dashLengths[] = {5.0f, 5.0f};

        outlineStroke.createDashedStroke (dashedOutline, outline, dashLengths, 2);

        g.fillPath (dashedOutline);
    }
}

void EmptyGestureSlotComponent::resized()
{
}

void EmptyGestureSlotComponent::mouseEnter (const MouseEvent &)
{
    highlighted = true;
    repaint();
}
void EmptyGestureSlotComponent::mouseExit (const MouseEvent &)
{
    highlighted = false;
    repaint();
}
void EmptyGestureSlotComponent::mouseDrag (const MouseEvent &)
{
}
