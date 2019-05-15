/*
  ==============================================================================

    GesturePanel.cpp
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#include "Gesture/GestureArray.h"
#include "Ui/Gesture/GesturePanel.h"
#include "Ui/Gesture/Tuner/GesturesTuner.h"
#include "Ui/Gesture/SettingsTabs/MapperComponent.h"
//#include "GestureGridComponent.h"
#include "GestureSettingsComponent.h"

//==============================================================================
GesturePanel::GesturePanel (GestureArray& gestArray, PluginWrapper& wrap,
                            AudioProcessorValueTreeState& params, NewGesturePanel& newGest,
                            int freqHz)
                            : gestureArray (gestArray), wrapper (wrap),
                              parameters (params), newGesturePanel (newGest),
                              freq (freqHz)
{
    TRACE_IN;
    setComponentID ("gesturePanel");
    setWantsKeyboardFocus (true);

    initialiseGestureSlots();
    createAndAddCloseButton();

    startTimerHz (freq);
}

GesturePanel::~GesturePanel()
{
    TRACE_IN;
    stopTimer();
    unselectCurrentGesture();
    newGesturePanel.hidePanel (true);
    removeListenerForAllParameters();
}

//==============================================================================
const String GesturePanel::getInfoString()
{
    return "Gesture Panel: \n\n"
           "- Click on a gesture to access to its settings.\n"
           "- Click on a \"+\" button to add a gesture in a slot.";
}

void GesturePanel::update()
{
    if (newGesturePanel.isVisible())
    {
        int lastSlot = newGesturePanel.getLastSelectedSlot();

        updateSlotIfNeeded (lastSlot);
        newGesturePanel.hidePanel();

        renameGestureInSlot (lastSlot);
    }

    stopTimer();

    for (int i=0; i<PLUME::NUM_GEST; i++)
    {
        updateSlotIfNeeded (i);
    }

    if (selectedGesture != -1)
    {
        gestureSettings->update();
    }

    startTimerHz (freq);
}

//==============================================================================
void GesturePanel::paint (Graphics& g)
{
    using namespace PLUME::UI;
    // Background
    
    /*
    if (hasSelectedGesture())
    {
        g.setColour (Colours::white);
        g.fillRect (gestureSlots[selectedGesture]->getRight() - MARGIN,
                    gestureSlots[selectedGesture]->getY()+1,
                    4*MARGIN,
                    gestureSlots[selectedGesture]->getHeight()-2);
    }
    */

    g.drawImage (backgroundImage, getLocalBounds().toFloat(), RectanglePlacement::xLeft +
                                                              RectanglePlacement::yTop  +
                                                              RectanglePlacement::doNotResize);
    //g.setColour (Colour (0x30000000));
    //g.fillRect (getLocalBounds());
                                                              
}

void GesturePanel::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds();

    if (settingsVisible)
    {
        gestureSettings->setBounds (area.removeFromRight (jmax (getWidth()/2, getWidth() - SIDEBAR_WIDTH*3/2)));
        closeButton->setBounds (juce::Rectangle<int> (30, 30).withX (gestureSettings->getRight() - 2*MARGIN - 30)
                                                             .withY (gestureSettings->getY() + 2*MARGIN)
                                                             .reduced (7));
    }
    
    // Sets the area for each gestureComp
    area = area.reduced (2*PLUME::UI::MARGIN, 2*PLUME::UI::MARGIN);

    int gestureHeight = (area.getHeight() - (PLUME::NUM_GEST - 1) * PLUME::UI::MARGIN) / PLUME::NUM_GEST;

    for (int i=0; i<gestureSlots.size(); i++)
    {
        gestureSlots[i]->setBounds (area.removeFromTop (gestureHeight));
        area.removeFromTop (PLUME::UI::MARGIN);
    }
}

void GesturePanel::timerCallback()
{
    if (gestureSettings != nullptr)
    {
        gestureSettings->updateDisplay();
    }
}

void GesturePanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton)
    {
        unselectCurrentGesture();
    }
}

void GesturePanel::mouseUp (const MouseEvent &event)
{
    if (event.eventComponent != this)
    {
        if (event.mods.isLeftButtonDown())
        {
            if (auto* gestureComponent = dynamic_cast<GestureComponent*> (event.eventComponent))
            {
                if (!gestureComponent->isSelected())
                {
                    selectGestureExclusive (*gestureComponent);
                }
            }

            if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (event.eventComponent))
            {
                newGesturePanel.showPanelForGestureID (emptySlot->id);
            }
        }

        if (event.mods.isPopupMenu())
        {
            if (auto* gestureComponent = dynamic_cast<GestureComponent*> (event.eventComponent))
            {
                createMenuForGestureId (gestureComponent->id);
            }
        }
    }
}

bool GesturePanel::keyPressed (const KeyPress &key)
{
    if (hasSelectedGesture() && key.isValid())
    {
        if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
        {
            removeGestureAndGestureComponent (selectedGesture);
        }

        else if (key.getTextCharacter() == 'r')
        {
			if (key.getModifiers().isAltDown())
			{
				renameGestureInSlot (selectedGesture);
			}
        }
    }

	return false;
}

void GesturePanel::initialiseGestureSlots()
{
    for (int i=0; i<PLUME::NUM_GEST; i++)
    {
        if (Gesture* gestureToCreateComponentFor = gestureArray.getGesture (i))
        {
            gestureSlots.add (new GestureComponent (*gestureToCreateComponentFor));
            addParameterListenerForGestureId (i);
        }
        else
        {
            gestureSlots.add (new EmptyGestureSlotComponent (i));
        }

        addAndMakeVisible (gestureSlots.getLast());
        gestureSlots.getLast()->addMouseListener (this, false);
    }
}

void GesturePanel::updateSlotIfNeeded (int slotToCheck)
{
    // 1st check, if a gesture was deleted (slot is GestureComponent but should be empty)
    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (gestureSlots[slotToCheck]))
    {
        if (gestureArray.getGesture (slotToCheck) == nullptr)
        {
            removeParameterListenerForGestureId (slotToCheck);
            gestureSlots.set (slotToCheck, new EmptyGestureSlotComponent (slotToCheck), true);
            addAndMakeVisible (gestureSlots[slotToCheck]);
            gestureSlots[slotToCheck]->addMouseListener (this, false);
            resized();
            repaint();
        }
    }
    // 2nd check, if a gesture was created (slot is empty but should be a gestureComponent)
    else if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (gestureSlots[slotToCheck]))
    {
        if (auto* gestureThatWasCreated = gestureArray.getGesture (slotToCheck))
        {
            addParameterListenerForGestureId (slotToCheck);

            gestureSlots.set (slotToCheck, new GestureComponent (*gestureThatWasCreated), true);
            addAndMakeVisible (gestureSlots[slotToCheck]);
            gestureSlots[slotToCheck]->addMouseListener (this, false);

			if (newGesturePanel.getLastSelectedSlot() == slotToCheck)
			{
				newGesturePanel.hidePanel(true);
				selectGestureExclusive(*dynamic_cast<GestureComponent*> (gestureSlots[slotToCheck]));
			}

            resized();
            repaint();
        }
    }
}

void GesturePanel::addGestureComponent (Gesture& gest)
{
    /*
    if (gestureComponents.addIfNotAlreadyThere (new GestureComponent (gest, gestureArray, wrapper)))
    {
        addAndMakeVisible (gestureComponents.getLast());

        int gestureHeight = (getHeight() - (PLUME::NUM_GEST - 1) * MARGIN) / PLUME::NUM_GEST; // gets the height of each gesture component4
        gestureComponents.getLast()->setBounds (0, gest.id * (gestureHeight + MARGIN), getWidth(), gestureHeight);
    
    
        for (int i=0; i<PLUME::param::numParams; i++)
        {
            parameters.addParameterListener (String (gest.id) + PLUME::param::paramIds[i], this);
        }
    }
    */
}

void GesturePanel::renameGestureInSlot (int slotNumber)
{
    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (gestureSlots[slotNumber]))
    {
        gestureComponent->startNameEntry();
    }
    else
    {
        /* Tried to rename a gesture, but the selected dlot was empty...
        */
        jassertfalse;
    }
}

void GesturePanel::removeGestureAndGestureComponent (int gestureId)
{
    if (gestureId < 0 || gestureId > PLUME::NUM_GEST) return;
    stopTimer();

    if (gestureId == selectedGesture)
    {
        unselectCurrentGesture();
        gestureSettings.reset (nullptr);
    }

    removeParameterListenerForGestureId (gestureId);
    gestureArray.removeGesture (gestureId);
    updateSlotIfNeeded (gestureId);

    if (!isTimerRunning()) startTimerHz (freq);
}

bool GesturePanel::hasSelectedGesture()
{
    return selectedGesture != -1;
}

void GesturePanel::switchGestureSelectionState (GestureComponent& gestureComponentToSwitch)
{
    if (gestureComponentToSwitch.isSelected())
    {
        gestureComponentToSwitch.setSelected (false);
        selectedGesture = -1;
        setSettingsVisible (false);
    }
    else
    {
        selectGestureExclusive (gestureComponentToSwitch);
    }
}

void GesturePanel::selectGestureExclusive (GestureComponent& gestureComponentToSelect)
{
    gestureComponentToSelect.setSelected (true);

    for (auto* slot : gestureSlots)
    {
        if (auto* gestureComponent = dynamic_cast<GestureComponent*> (slot))
        {
            if (gestureComponent != &gestureComponentToSelect && gestureComponent->isSelected())
            {
                gestureComponent->setSelected (false);
                gestureArray.getGesture (gestureComponent->id)->removeAllChangeListeners();
            }
        }
    }

    gestureSettings.reset (new GestureSettingsComponent (gestureComponentToSelect.getGesture(),
                                                         gestureArray, wrapper));
    selectedGesture = gestureComponentToSelect.id;
    setSettingsVisible (true);
}

void GesturePanel::unselectCurrentGesture()
{
	// Can't unselect if nothing is selected..
	if (selectedGesture == -1) return;

    if (auto* gestureComponentToUnselect = dynamic_cast<GestureComponent*> (gestureSlots[selectedGesture]))
    {
        if (!gestureComponentToUnselect->isSelected())
        {
            // Plume tried to unselect an already unselected gesture...
            jassertfalse;
            return;
        }

        gestureComponentToUnselect->setSelected (false);

        if (auto* gestureToUnselect = gestureArray.getGesture (selectedGesture))
            gestureToUnselect->removeAllChangeListeners();

        selectedGesture = -1;
        setSettingsVisible (false);
        gestureSettings.reset (nullptr);

        return;
    }

    /* Hitting this assert means that Plume tried to unselect a gesture, but the slot
       it tried to access is an empty slot. That might either mean that an emptySlot failed
       to be updated to a GestureComponent, or that Plume failed to keep track of the actual
       selected Gesture.
    */
    jassertfalse;
}

void GesturePanel::createMenuForGestureId (int id)
{
    PopupMenu gestureMenu;

    gestureMenu.addItem (1, "Rename", true);
    gestureMenu.addItem (2, "Duplicate", false);
    gestureMenu.addItem (3, "Delete", true);
    
    handleMenuResult (id,
                      gestureMenu.showMenu (PopupMenu::Options().withParentComponent (getParentComponent())
                                                                .withMaximumNumColumns (3)
                                                                .withPreferredPopupDirection (PopupMenu::Options::PopupDirection::downwards)
                                                                .withStandardItemHeight (20)));
}

void GesturePanel::handleMenuResult (int gestureId, const int menuResult)
{
    switch (menuResult)
    {
        case 0: // No choice
            break;
            
        case 1: // Rename gesture
            renameGestureInSlot (gestureId);
            break;
            
        case 2: // Duplicate
            // TODO
            break;

        case 3: // Delete gesture
            removeGestureAndGestureComponent (gestureId);
            update();
    }
}

void GesturePanel::setSettingsVisible (bool shouldBeVisible)
{
    if (!shouldBeVisible)
    {
        gestureSettings->setVisible (false);
        closeButton->setVisible (false);
        settingsVisible = false;
        resized();
        repaint();

        return;
    }

    if (shouldBeVisible && gestureSettings != nullptr)
    {
        addAndMakeVisible (gestureSettings, 0);
        closeButton->setVisible (true);
        settingsVisible = true;
        resized();
        repaint();

        return;
    }

    /* Plume attempted to show the gesture settings panel when it was not instanciated!
       There should be a call similar to the following one before calling this method if you want
       to display the settings panel:
            gestureSettings.reset ( [gesture that was selected] , gestureArray, wrapper); 
    */
    jassertfalse;
}

void GesturePanel::createAndAddCloseButton()
{
    addAndMakeVisible (closeButton = new ShapeButton ("Close Settings Button", Colour(0x00000000),
                                                                               Colour(0x00000000),
                                                                               Colour(0x00000000)),
                      -1);

    Path p;
    p.startNewSubPath (0, 0);
    p.lineTo (3*PLUME::UI::MARGIN, 3*PLUME::UI::MARGIN);
    p.startNewSubPath (0, 3*PLUME::UI::MARGIN);
    p.lineTo (3*PLUME::UI::MARGIN, 0);

    closeButton->setShape (p, false, true, false);
    closeButton->setOutline (Colour (0xff101010), 1.0f);
    closeButton->addMouseListener (this, false);
    closeButton->addListener (this);
}

void GesturePanel::removeListenerForAllParameters()
{
    for (auto* gesture : gestureArray.getArray())
    {
		removeParameterListenerForGestureId (gesture->id);
    }
}

void GesturePanel::addParameterListenerForGestureId (const int id)
{
    for (int i = 0; i<PLUME::param::numParams; i++)
    {
        parameters.addParameterListener (String (id) + PLUME::param::paramIds[i], this);
    }
}

void GesturePanel::removeParameterListenerForGestureId (const int id)
{
    for (int i = 0; i<PLUME::param::numParams; i++)
    {
        parameters.removeParameterListener (String (id) + PLUME::param::paramIds[i], this);
    }
}

void GesturePanel::parameterChanged (const String& parameterID, float)
{
    // if the ID is "x_value" or "x_vibrato_intensity": doesn't update
    // (only the MovingCursor object in the the GestureTunerComponent is updated)
    if (parameterID.endsWith ("ue") || parameterID.endsWith ("y") || !PLUME::UI::ANIMATE_UI_FLAG) return;
    
    const int gestureId = parameterID.substring(0,1).getIntValue();
    
    if (auto* gestureComponentToUpdate = dynamic_cast<GestureComponent*> (gestureSlots[gestureId]))
    {
        if (gestureComponentToUpdate->id == gestureId)
        {
			const MessageManagerLock mmLock;

			gestureComponentToUpdate->update();

            if (gestureSettings != nullptr)
            {
                if (gestureSettings->getGestureId() == gestureId)
                {
                    gestureSettings->update();
                }
            }
        }
    }
}

//==============================================================================
// Gesture Grid 

GesturePanel::GestureComponent::GestureComponent (Gesture& gest) : gesture (gest), id (gest.id)
{
    createLabel();
}
GesturePanel::GestureComponent::~GestureComponent()
{
    gestureNameLabel->removeListener (this);
    gestureNameLabel = nullptr;
}

const String GesturePanel::GestureComponent::getInfoString()
{
    return gesture.getName() + " | " + gesture.getTypeString (true) + "\n\n" +
           "State : " + (gesture.isActive() ? "Enabled" : "Disabled") +
           " | Mode : " + (gesture.isMidiMapped() ? "MIDI\n" : "Parameters\n")
           + "\n" + gestureDescription;
}
void GesturePanel::GestureComponent::update()
{
    gestureNameLabel->setText (gesture.getName(), dontSendNotification);
}

void GesturePanel::GestureComponent::paint (Graphics& g)
{
    if (selected)
    {
        g.setColour (Colour (0xffffffff));
    }
    else
    {
        g.setColour (highlighted ? Colours::white.withAlpha (0.8f) : Colours::white.withAlpha (0.6f));
    }

    g.fillRoundedRectangle (getLocalBounds().toFloat(), 3.0f);

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
    
    g.drawText (gesture.isMidiMapped() ? "MIDI" : "Param",
                stateArea,
                Justification::centredRight, true);
    
}
void GesturePanel::GestureComponent::resized()
{
    gestureNameLabel->setBounds (getLocalBounds().withHeight (30)
                                                .withTrimmedLeft (getWidth()/4)
                                                .withWidth (getWidth()/2));
}
void GesturePanel::GestureComponent::editorShown (Label* lbl, TextEditor& ted)
{
    ted.setColour (TextEditor::highlightColourId, Colour (0xff101010));
    ted.setColour (TextEditor::textColourId, Colour (0xff959595));
    ted.setJustification (Justification::centred);
}

void GesturePanel::GestureComponent::labelTextChanged (Label* lbl)
{
    gesture.setName (gestureNameLabel->getText());
    
    dynamic_cast<GesturePanel*> (getParentComponent())->update();
}

void GesturePanel::GestureComponent::mouseEnter (const MouseEvent &event)
{
    setHighlighted (true);
}
void GesturePanel::GestureComponent::mouseExit (const MouseEvent &event)
{
    setHighlighted (false);
}

Gesture& GesturePanel::GestureComponent::getGesture()
{
    return gesture;
}
bool GesturePanel::GestureComponent::isSelected() const
{
    return selected;
}
void GesturePanel::GestureComponent::setSelected (bool shouldBeSelected)
{
    selected = shouldBeSelected;
    repaint();
}
void GesturePanel::GestureComponent::setHighlighted (bool shouldBeHighlighted)
{
    highlighted = shouldBeHighlighted;
    repaint();
}
void GesturePanel::GestureComponent::setSolo (bool shouldBeSolo)
{
    solo = shouldBeSolo;
    repaint();
}
void GesturePanel::GestureComponent::startNameEntry()
{
    gestureNameLabel->showEditor();
}

void GesturePanel::GestureComponent::createLabel()
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

GesturePanel::EmptyGestureSlotComponent::EmptyGestureSlotComponent (const int slotId) : id (slotId)
{
}
GesturePanel::EmptyGestureSlotComponent::~EmptyGestureSlotComponent()
{
}

const String GesturePanel::EmptyGestureSlotComponent::getInfoString()
{
    return String();
}
void GesturePanel::EmptyGestureSlotComponent::update()
{
}

void GesturePanel::EmptyGestureSlotComponent::paint (Graphics& g)
{
    g.setColour (highlighted ? Colours::white.withAlpha (0.4f) : Colours::white.withAlpha (0.1f));
    g.fillEllipse (getLocalBounds().withSizeKeepingCentre (20, 20).toFloat());

    if (highlighted) g.drawRoundedRectangle (getLocalBounds().reduced(1).toFloat(), 3.0f, 1.0f);

    g.setColour (Colours::black);
    g.setFont (PLUME::font::plumeFontBold.withHeight (30.0f));
    g.drawText ("+", getLocalBounds(), Justification::centred, true);
}

void GesturePanel::EmptyGestureSlotComponent::resized()
{
}

void GesturePanel::EmptyGestureSlotComponent::mouseEnter (const MouseEvent &event)
{
    highlighted = true;
    repaint();
}
void GesturePanel::EmptyGestureSlotComponent::mouseExit (const MouseEvent &event)
{
    highlighted = false;
    repaint();
}