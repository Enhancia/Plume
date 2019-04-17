/*
  ==============================================================================

    GesturePanel.h
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Gesture/GestureArray.h"
#include "Wrapper/PluginWrapper.h"
//#include "GestureGridComponent.h"
#include "GestureSettingsComponent.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"
#include "Ui/Top/NewGesture/NewGesturePanel.h"

//==============================================================================
/*
*/
class GesturePanel    : public PlumeComponent,
                        public Timer,
                        private Button::Listener,
                        private AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    GesturePanel (GestureArray& gestureArray, PluginWrapper& wrap,
                  AudioProcessorValueTreeState& params, NewGesturePanel& newGest,
                  int freqHz = 60);
    ~GesturePanel();

    //==============================================================================
    // PlumeComponent
    const String getInfoString() override;
    void update() override;

    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // Callbacks
    void timerCallback() override;
    void buttonClicked (Button*) override;
    void parameterChanged (const String &parameterID, float newValue) override;

    void mouseUp (const MouseEvent &event) override;
    bool keyPressed (const KeyPress &key) override;

    //==============================================================================
    void removeListenerForAllParameters();
    void addParameterListenerForGestureId (const int id);
    void removeParameterListenerForGestureId (const int id);

    //==============================================================================
    void initialiseGestureSlots();
    void removeGestureAndGestureComponent (int gestureId);
    void addGestureComponent (Gesture& gest);
    bool hasSelectedGesture();

private:
    class GestureComponent : public PlumeComponent
    {
    public:
        GestureComponent (Gesture& gest);
        ~GestureComponent();

        const String getInfoString() override;
        void update() override;

        void paint (Graphics&) override;
        void resized() override;

        void mouseEnter (const MouseEvent &event) override;
        void mouseExit (const MouseEvent &event) override;

        Gesture& getGesture();
        bool isSelected() const;
        void setSelected (bool);
        void setHighlighted (bool);
        void setSolo (bool);

        const int id;

    private:
        Gesture& gesture;
        String gestureDescription = "";
        bool on = gesture.isActive(), selected = false, highlighted = false, solo = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureComponent)
    };


    class EmptyGestureSlotComponent : public PlumeComponent
    {
    public:
        EmptyGestureSlotComponent (const int slotId);
        ~EmptyGestureSlotComponent();

        const String getInfoString() override;
        void update() override;

        void paint (Graphics&) override;
        void resized() override;

        void mouseEnter (const MouseEvent &event) override;
        void mouseExit (const MouseEvent &event) override;

        const int id;

    private:
        bool highlighted = false;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EmptyGestureSlotComponent)
    };

    void switchGestureSelectionState (GestureComponent& gestureComponentToSwitch);
    void selectGestureExclusive (GestureComponent& gestureComponentToSelect);
    void unselectCurrentGesture ();
    void updateSlotIfNeeded (int slotToCheck);

    void setSettingsVisible (bool shouldBeVisible);
    void createAndAddCloseButton();
    
    //==============================================================================
    Image backgroundImage = ImageFileFormat::loadFrom (PlumeData::homePageEnhancia_jpg,
                                                       PlumeData::homePageEnhancia_jpgSize);

    OwnedArray<PlumeComponent> gestureSlots;
    ScopedPointer<GestureSettingsComponent> gestureSettings;
    ScopedPointer<ShapeButton> closeButton;

    //==============================================================================
    int selectedGesture = -1;
    int freq;
    bool settingsVisible = false;

    //==============================================================================
    GestureArray& gestureArray;
    PluginWrapper& wrapper;
    AudioProcessorValueTreeState& parameters;
    NewGesturePanel& newGesturePanel;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};
