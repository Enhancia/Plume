/*
  ==============================================================================

    GesturePanel.cpp
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#include "../../Gesture/GestureArray.h"
#include "GesturePanel.h"
#include "Tuner/GesturesTuner.h"
#include "SettingsTabs/mappercomponent.h"
#include "GestureSettingsComponent.h"

//==============================================================================
GesturePanel::GesturePanel (GestureArray& gestArray, PluginWrapper& wrap,
                            AudioProcessorValueTreeState& params, NewGesturePanel& newGest, Component& newPrst,
                            int freqHz)
                            : gestureArray (gestArray), wrapper (wrap),
                              parameters (params), newGesturePanel (newGest), newPresetPanel (newPrst),
                              freq (freqHz)
{
    setComponentID ("gesturePanel");
    setWantsKeyboardFocus (true);

    //shadowEffect.setShadowProperties ({Colour (0x50000000), PLUME::UI::MARGIN,
    //                                  {0, 0}});
    //setComponentEffect (&shadowEffect);

    initialiseGestureSlots();
    createAndAddCloseButton();

    startTimerHz (freq);
}

GesturePanel::~GesturePanel()
{
    //setComponentEffect (nullptr);
    stopTimer();
    unselectCurrentGesture();
    newGesturePanel.hidePanel (true);
    newPresetPanel.setVisible(false);
    removeListenerForAllParameters();
}

//==============================================================================
const String GesturePanel::getInfoString()
{
    const String bullet = " " + String::charToString (juce_wchar(0x2022));
    
    return "Gesture Panel: \n\n"
           + bullet + " Click on a gesture to access to its settings.\n"
           + bullet + " Click on a \"+\" button to add a gesture in a slot.";
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
        gestureSettings->updateMappedParameters();
    }

    startTimerHz (freq);
}

//==============================================================================
void GesturePanel::paint (Graphics& g)
{
    paintShadows (g);
}

void GesturePanel::paintOverChildren (Graphics& g)
{
    /* TODO paint the component snapshot during a drag.
       Get a snapshot of the component being dragged (might wanna cache it so it is not 
       recalculated for every repaint), and display it centred at the current mouse position.
    if (dragMode)
    {
        Image gestureComponentImage;
        // here get the image

        //g.drawImage (gestureComponentImage, Rectangle_that_is_the_size_of_the_image);
    }
    */

    paintDragAndDropSnapshot (g);
}

void GesturePanel::paintShadows (Graphics& g)
{
    Path shadowPath;

    if (gestureSettings)
    {
        // Gesture Settings
        auto gestureSettingsArea = gestureSettings->getBounds();

        shadowPath.addRoundedRectangle (gestureSettingsArea.toFloat(), 10.0f);
    }
    {
        // Gesture Components
        for (int slot=0; slot < PLUME::NUM_GEST; slot++)
        {
            if (auto* gestureComp = dynamic_cast<GestureComponent*> (gestureSlots[slot]))
            {
                shadowPath.addRoundedRectangle (gestureComp->getBounds().toFloat(), 10.0f);
            }
        }
    }

    DropShadow shadow (Colour (0x30000000), 10, {2, 3});
    shadow.drawForPath (g, shadowPath);
}

void GesturePanel::paintDragAndDropSnapshot (Graphics& g)
{
    if(ImageCache::getFromHashCode(hashCode).isValid()) {
        g.setOpacity(0.4f);
        g.drawImage (ImageCache::getFromHashCode(hashCode), draggedImgPosition.toFloat ());
    }
}

void GesturePanel::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds();
    resizeSlotsAndTrimAreaAccordingly (area, MARGIN, MARGIN_SMALL);
    
    if (settingsVisible)
    {
        gestureSettings->setBounds (area.reduced (MARGIN, MARGIN_SMALL));
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
}

void GesturePanel::mouseUp (const MouseEvent &event)
{
    if (event.eventComponent != this)
    {
        if (event.mods.isLeftButtonDown())
        {
            if (dragMode) endDragMode();

			handleLeftClickUp (event);
        }

        else if (event.mods.isPopupMenu())
        {
            if (auto* gestureComponent = dynamic_cast<GestureComponent*> (event.eventComponent))
            {
                createMenuForGestureId (gestureComponent->id);
            }
        }
    }
}

void GesturePanel::mouseDrag (const MouseEvent& event)
{
    auto relativeEvent = event.getEventRelativeTo (this);

    if (relativeEvent.mods.isLeftButtonDown())
    {
        if (auto* gestureComponent = dynamic_cast<GestureComponent*> (relativeEvent.originalComponent))
        {
            if (!dragMode)
                startDragMode (*gestureComponent);

            repaint(draggedImgPosition);

            draggedImgPosition.setPosition(
                relativeEvent.position.getX() - gestureComponent->getWidth() / 2.0f,
                relativeEvent.position.getY() - gestureComponent->getHeight() / 2.0f
            );

            repaint(draggedImgPosition);

			int formerDraggedOverId = draggedOverSlotId;

            if (Component* componentUnderMouse = getComponentAt (relativeEvent.getPosition()))
            {
                if (auto* otherGesture = dynamic_cast<GestureComponent*> (componentUnderMouse))
                {
					if (otherGesture->id != draggedGestureComponentId)
					{
						draggedOverSlotId = otherGesture->id;
						otherGesture->repaint();
					}
				}
                else if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (componentUnderMouse))
                {
                    draggedOverSlotId = emptySlot->id;
					emptySlot->repaint();
                }
				else
				{
					draggedOverSlotId = -1;
				}
            }
            else
            {
                draggedOverSlotId = -1;
            }

			if (formerDraggedOverId != -1 && formerDraggedOverId != draggedOverSlotId)
				gestureSlots[formerDraggedOverId]->repaint();

            /* TODO repaint here for the component snapshot.
               For optimal smoothness, get the last coordinates here, and repaint around
               both the current and last mouse position, in a square that is the size
               of the snapshot.
            //repaint();
            */
        }
    }
}

void GesturePanel::handleLeftClickUp (const MouseEvent& event)
{
    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (event.eventComponent))
    {
        // Mouse didn't leave the component it clicked in the first place
        if (getComponentAt (event.getEventRelativeTo (this).getPosition()) == event.eventComponent)
        {
            if (!gestureComponent->isSelected())
            {
                PLUME::log::writeToLog ("Gesture " + String (gestureComponent->getGesture().getName()) + " (Id " + String (gestureComponent->id) + ") : Selecting",
                                        PLUME::log::gesture);

                selectGestureExclusive (*gestureComponent);
            }
        }
        else // Mouse was dragged to another component
        {
            if (Component* componentUnderMouse = getComponentAt (event.getEventRelativeTo (this).getPosition()))
            {
                if (auto* slotUnderMouse = dynamic_cast<EmptyGestureSlotComponent*> (componentUnderMouse))
                {
                    moveGestureToId (gestureComponent->id, slotUnderMouse->id);
                }
                else if (auto* gestureComponentUnderMouse = dynamic_cast<GestureComponent*> (componentUnderMouse))
                {
                    swapGestures (gestureComponent->id, gestureComponentUnderMouse->id);
                }
            }
        }
    }

    else if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (event.eventComponent))
    {
        newGesturePanel.showPanelForGestureID (emptySlot->id);
    }
}

void GesturePanel::handleLeftClickDrag (const MouseEvent&)
{
}

bool GesturePanel::keyPressed (const KeyPress& key)
{
    if (hasSelectedGesture () && key.isValid ())
    {
        //remove gesture
        if (key.getKeyCode () == KeyPress::deleteKey || key.getKeyCode () == KeyPress::backspaceKey)
        {
            removeGestureAndGestureComponent (selectedGesture);
      
            if(findExistingGesture() != -1)
                selectGestureExclusive (findExistingGesture());
        }
        //rename gesture
        else if (key.getTextCharacter () == 'r')
        {
            if (key.getModifiers ().isCommandDown())
                renameGestureInSlot (selectedGesture);
        }
        //rename gesture
        else if (key == PLUME::keyboard_shortcut::rename) {
            renameGestureInSlot (selectedGesture);
        }
        //duplicate gesture
        else if (key == PLUME::keyboard_shortcut::duplicateGesture) {
            gestureArray.duplicateGesture (selectedGesture);
            update ();
        }
        //save gesture
        else if (key == PLUME::keyboard_shortcut::saveGesture) {
            newPresetPanel.setVisible (true);
        }
        //moving up
        else if (key.isKeyCode (KeyPress::upKey)) {
            if (selectedGesture > 0) {

                auto tempPosition = selectedGesture;
                selectedGesture--;

                while (gestureArray.getGesture (selectedGesture) == nullptr)
                {
                    if (selectedGesture == 0)
                        break;
                    selectedGesture--;
                }

                if (gestureArray.getGesture (selectedGesture) == nullptr)
                    selectedGesture = tempPosition;

                selectGestureExclusive (selectedGesture);
            }
        }
        //moving down
        else if (key.isKeyCode (KeyPress::downKey)) {
            if (selectedGesture < gestureSlots.size () - 1) {

                auto tempPosition = selectedGesture;
                selectedGesture++;

                while (gestureArray.getGesture (selectedGesture) == nullptr)
                {
                    if (selectedGesture == gestureSlots.size () - 1)
                        break;
                    selectedGesture++;
                }

                if (gestureArray.getGesture (selectedGesture) == nullptr)
                    selectedGesture = tempPosition;

                selectGestureExclusive (selectedGesture);
            }
        }
        //moving left
        else if (key.isKeyCode (KeyPress::leftKey)) {
            if (selectedGesture == 4 || selectedGesture == 5 || selectedGesture == 6 || selectedGesture == 7) {

                auto tempPosition = selectedGesture;
                selectedGesture -= 4;

                if (gestureArray.getGesture (selectedGesture) == nullptr)
                    selectedGesture = 0;

                while (gestureArray.getGesture (selectedGesture) == nullptr)
                {
                    selectedGesture++;
                    if (selectedGesture == gestureSlots.size () - 1) {
                        selectedGesture = tempPosition;
                        break;
                    }
                }

                selectGestureExclusive (selectedGesture);
            }
        }
        //moving right
        else if (key.isKeyCode (KeyPress::rightKey)) {
            if (selectedGesture == 0 || selectedGesture == 1 || selectedGesture == 2 || selectedGesture == 3) {

                auto tempPosition = selectedGesture;
                selectedGesture += 4;

                if (gestureArray.getGesture (selectedGesture) == nullptr)
                    selectedGesture = 4;

                while (gestureArray.getGesture (selectedGesture) == nullptr)
                {
                    selectedGesture++;
                    if (selectedGesture == gestureSlots.size () - 1) {
                        selectedGesture = tempPosition;
                        break;
                    }
                }

                selectGestureExclusive (selectedGesture);
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
            gestureSlots.add (new GestureComponent (*gestureToCreateComponentFor, gestureArray,
                                                    dragMode, draggedGestureComponentId, draggedOverSlotId));
            addParameterListenerForGestureId (i);
        }
        else
        {
            gestureSlots.add (new EmptyGestureSlotComponent (i, gestureArray,
                                                                dragMode,
                                                                draggedGestureComponentId,
                                                                draggedOverSlotId));
        }

        addAndMakeVisible (gestureSlots.getLast());
        gestureSlots.getLast()->addMouseListener (this, false);
    }
}

void GesturePanel::resizeSlotsAndTrimAreaAccordingly (juce::Rectangle<int>& area, int marginX, int marginY)
{
    using namespace PLUME::UI;
    if (PLUME::NUM_GEST == 0 || gestureSlots.size() == 0) return;

    // There should be an even number of gestures for the grid of gesture slots to make sense...
    jassert (PLUME::NUM_GEST%2 == 0);
    int numRows = PLUME::NUM_GEST/2;

    int tempWidth = area.getWidth()/4;

    auto initialArea = area;
    
    if (!settingsVisible)
    {
        area.reduce (tempWidth/2, 0);
    }

    auto column1 = area.removeFromLeft (tempWidth);
    auto column2 = area.removeFromRight (tempWidth);

    // move columns to left/right border of global area
    column1.setX(initialArea.getX());
    column2.setX(initialArea.getWidth() - column2.getWidth());

    int slotHeight = area.getHeight()/numRows;

    // sets bounds depending on the value in the array
    for (int i=0; i<gestureSlots.size(); i++)
    {
        gestureSlots[i]->setBounds (i < numRows ? column1.removeFromTop (slotHeight)
														 .reduced (marginX, marginY)
                                                : column2.removeFromTop (slotHeight)
														 .reduced (marginX, marginY));
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
            gestureSlots.set (slotToCheck, new EmptyGestureSlotComponent (slotToCheck,
                                                                          gestureArray,
                                                                          dragMode,
                                                                          draggedGestureComponentId,
                                                                          draggedOverSlotId),
                                           true);

            addAndMakeVisible (gestureSlots[slotToCheck]);
            gestureSlots[slotToCheck]->addMouseListener (this, false);
            resized();
            repaint (gestureSlots[slotToCheck]->getBounds().expanded (13));
        }
    }
    // 2nd check, if a gesture was created (slot is empty but should be a gestureComponent)
    else if (auto* emptySlot = dynamic_cast<EmptyGestureSlotComponent*> (gestureSlots[slotToCheck]))
    {
        if (auto* gestureThatWasCreated = gestureArray.getGesture (slotToCheck))
        {
            addParameterListenerForGestureId (slotToCheck);

            gestureSlots.set (slotToCheck, new GestureComponent (*gestureThatWasCreated,
                                                                 gestureArray,
                                                                 dragMode,
                                                                 draggedGestureComponentId,
                                                                 draggedOverSlotId),
                                           true);

            addAndMakeVisible (gestureSlots[slotToCheck]);
            gestureSlots[slotToCheck]->addMouseListener (this, false);

			if (newGesturePanel.getLastSelectedSlot() == slotToCheck)
			{
				newGesturePanel.hidePanel(true);
				selectGestureExclusive(*dynamic_cast<GestureComponent*> (gestureSlots[slotToCheck]));
			}

            resized();
            repaint (gestureSlots[slotToCheck]->getBounds().expanded (13));
        }
    }
}


void GesturePanel::moveGestureToId (int idToMoveFrom, int idToMoveTo)
{
    PLUME::log::writeToLog ("Gesture " + gestureArray.getGesture (idToMoveFrom)->getName() + " (Id " + String (idToMoveFrom) + ") : Moving to id " + String (idToMoveTo),
                            PLUME::log::gesture);

    bool mustChangeSelection = (selectedGesture == idToMoveFrom);
    
    if (mustChangeSelection) unselectCurrentGesture();

    gestureArray.moveGestureToId (idToMoveFrom, idToMoveTo);
	update();

    if (mustChangeSelection)
    {
        selectGestureExclusive (idToMoveTo);
    }
}

void GesturePanel::swapGestures (int firstId, int secondId)
{
    PLUME::log::writeToLog ("Gesture " + gestureArray.getGesture (firstId)->getName() + " (Id " + String (firstId) + ") : Swapping with Gesture "
                                       + gestureArray.getGesture (secondId)->getName() + " (Id " + String (secondId) + ")",
                            PLUME::log::gesture);

    bool mustChangeSelection = (selectedGesture == firstId || selectedGesture == secondId);
    int idToSelect;

    if (mustChangeSelection)
    {
        idToSelect = (selectedGesture == firstId ? secondId : firstId);
        unselectCurrentGesture();
    }

    // Deletes GestureComponents for the 2 slots
    gestureSlots.set (firstId, new EmptyGestureSlotComponent (firstId, gestureArray,
                                                                       dragMode,
                                                                       draggedGestureComponentId,
                                                                       draggedOverSlotId), true);
    gestureSlots.set (secondId, new EmptyGestureSlotComponent (secondId, gestureArray,
                                                                         dragMode,
                                                                         draggedGestureComponentId,
                                                                         draggedOverSlotId), true);

    gestureArray.swapGestures (firstId, secondId);
    update();
    
    if (mustChangeSelection) selectGestureExclusive (idToSelect);
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
    PLUME::log::writeToLog ("Gesture " + gestureArray.getGesture (gestureId)->getName() + " (Id " + String (gestureId) + ") : Deleting",
                            PLUME::log::gesture);

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

void GesturePanel::selectGestureExclusive (const int idToSelect)
{
    if (idToSelect < 0 || idToSelect >= gestureSlots.size()) return;

    if (auto* gestureComponent = dynamic_cast<GestureComponent*> (gestureSlots[idToSelect]))
    {
        selectGestureExclusive (*gestureComponent);
    }
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
    gestureMenu.addItem (2, "Duplicate", true);
    gestureMenu.addItem (3, "Delete", true);
    
    gestureMenu.showMenuAsync (PopupMenu::Options().withParentComponent (getParentComponent())
                                                                .withMaximumNumColumns (3)
                                                                .withPreferredPopupDirection (PopupMenu::Options::PopupDirection::downwards)
                                                                .withStandardItemHeight (20),
                               ModalCallbackFunction::forComponent (menuCallback, this, id));
}

void GesturePanel::menuCallback(int result, GesturePanel* gPanel, int id)
{
    if (gPanel != nullptr)
    {
        gPanel->handleMenuResult (id, result);
    }
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
            PLUME::log::writeToLog ("Gesture " + gestureArray.getGesture (gestureId)->getName() + " (Id " + String (gestureId) + ") : Duplicating", PLUME::log::gesture);
            gestureArray.duplicateGesture (gestureId);
            update();
            selectGestureExclusive (gestureId);
            break;

        case 3: // Delete gesture
            removeGestureAndGestureComponent (gestureId);
            if (findExistingGesture () != -1)
                selectGestureExclusive (findExistingGesture ());
            update();
    }
}

void GesturePanel::setSettingsVisible (bool shouldBeVisible)
{
    if (!shouldBeVisible)
    {
        gestureSettings->setVisible (false);
        settingsVisible = false;
        resized();
        repaint();

        return;
    }

    if (shouldBeVisible && gestureSettings != nullptr)
    {
        addAndMakeVisible (*gestureSettings, 0);
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
    if (!PLUME::UI::ANIMATE_UI_FLAG) return;
    
    const int gestureId = parameterID.substring(0,1).getIntValue();
    
    if (auto* gestureComponentToUpdate = dynamic_cast<GestureComponent*> (gestureSlots[gestureId]))
    {
        ScopedLock pcLock (parameterCallbackLock);
        
        if (gestureComponentToUpdate->id == gestureId)
        {
            if (gestureSettings != nullptr)
            {
                if (gestureSettings->getGestureId() == gestureId)
                {
                    gestureSettings->update (parameterID.substring (1, parameterID.length()));
                }
            }
        }
    }
}

int GesturePanel::findExistingGesture () {

    int gestureId = 0;
    int result = -1;

    while (gestureArray.getGesture (gestureId) == nullptr)
    {
        gestureId++;
        if(gestureId == gestureSlots.size () - 1)
            break;
    }

    if(gestureArray.getGesture (gestureId) != nullptr)
        result = gestureId;

    return result;
}


void GesturePanel::startDragMode (GestureComponent& gestureComponent)
{
    dragMode = true;
    draggedGestureComponentId = gestureComponent.id;
    draggedOverSlotId = -1;

    for (auto* slot : gestureSlots)
    {
        slot->repaint();
    }

    ImageCache::addImageToCache (gestureComponent.createComponentSnapshot (gestureComponent.getLocalBounds (), false), hashCode);

    draggedImgPosition.setSize (
        ImageCache::getFromHashCode (hashCode).getWidth (),
        ImageCache::getFromHashCode (hashCode).getHeight ()
    );
}

void GesturePanel::endDragMode()
{
    dragMode = false;
    draggedGestureComponentId = -1;
    draggedOverSlotId = -1;

    for (auto* slot : gestureSlots)
    {
        slot->repaint();
    }

    ImageCache::releaseUnusedImages();
    repaint();
}
