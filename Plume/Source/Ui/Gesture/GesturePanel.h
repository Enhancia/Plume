/*
  ==============================================================================

    GesturePanel.h
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Gesture/GestureArray.h"
#include "../../Wrapper/PluginWrapper.h"
#include "../LookAndFeel/PlumeLookAndFeel.h"
#include "../Top/NewGesture/NewGesturePanel.h"
#include "GestureSettingsComponent.h"
#include "GesturePanelSlots.h"

//==============================================================================
/*
*/
class GesturePanel    : public Component,
                        public PlumeComponent,
                        public Timer,
                        private Button::Listener,
                        private AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    GesturePanel (GestureArray& gestureArray, PluginWrapper& wrap,
                  AudioProcessorValueTreeState& params, NewGesturePanel& newGest, Component& newPrst,
                  int freqHz = 60);
    ~GesturePanel();

    //==============================================================================
    // PlumeComponent
    const String getInfoString() override;
    void update() override;

    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void paintOverChildren (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // Callbacks
    void timerCallback() override;
    void buttonClicked (Button*) override;
    void parameterChanged (const String &parameterID, float newValue) override;

    /**
     * @brief Get the first gesture found in the list
     * @return Gesture Id
    */
    int findExistingGesture ();

    void mouseUp (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent &event) override;
    bool keyPressed (const KeyPress &key) override;

    //==============================================================================
    void removeListenerForAllParameters();
    void addParameterListenerForGestureId (const int id);
    void removeParameterListenerForGestureId (const int id);

    //==============================================================================
    void initialiseGestureSlots();
    void resizeSlotsAndTrimAreaAccordingly (juce::Rectangle<int>& area, int marginX = PLUME::UI::MARGIN,
                                                                        int marginY = PLUME::UI::MARGIN);
    void removeGestureAndGestureComponent (int gestureId);
    bool hasSelectedGesture();
    void renameGestureInSlot (int slotNumber);

private:
    //==============================================================================
    void paintShadows (Graphics& g);
    void paintDragAndDropSnapshot (Graphics& g);

    //==============================================================================
    void switchGestureSelectionState (GestureComponent& gestureComponentToSwitch);
    void selectGestureExclusive (const int idToSelect);
    void selectGestureExclusive (GestureComponent& gestureComponentToSelect);
    void unselectCurrentGesture ();
    void updateSlotIfNeeded (int slotToCheck);
    void moveGestureToId (int idToMoveFrom, int idToMoveTo);
    void swapGestures (int firstId, int secondId);

    //==============================================================================
    void createMenuForGestureId (int id);
    static void menuCallback(int result, GesturePanel* gPanel, int id);
    void handleMenuResult (int gestureId, const int menuResult);
    void handleLeftClickUp (const MouseEvent &event);
    void handleLeftClickDrag (const MouseEvent &event);

    //==============================================================================
    void setSettingsVisible (bool shouldBeVisible);
    void createAndAddCloseButton();

    //==============================================================================
    void startDragMode (GestureComponent& gestureComponent);
    void endDragMode();

    //==============================================================================
    OwnedArray<Component> gestureSlots;
    std::unique_ptr<GestureSettingsComponent> gestureSettings;
    //DropShadowEffect shadowEffect;

    //==============================================================================
    int selectedGesture = -1;
    int freq;
    bool settingsVisible = false;

    //==============================================================================
    bool dragMode = false;
    int draggedGestureComponentId = -1;
    int draggedOverSlotId = -1;

    juce::Rectangle<int> draggedImgPosition;
    const int64 hashCode = 420;

    //==============================================================================
    CriticalSection parameterCallbackLock;

    //==============================================================================
    GestureArray& gestureArray;
    PluginWrapper& wrapper;
    AudioProcessorValueTreeState& parameters;
    NewGesturePanel& newGesturePanel;
    Component& newPresetPanel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};
