/*
  ==============================================================================

    GesturePanelSlots.h
    Created: 27 May 2019 2:05:16pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Gesture/GestureArray.h"

class GestureComponent : public PlumeComponent,
                         private Label::Listener
{
public:
    //==============================================================================
    GestureComponent (Gesture& gest, const bool& dragModeReference,
                                     const int& draggedGestureReference,
                                     const int& draggedOverSlotReference);
    ~GestureComponent();

    //==============================================================================
    const String getInfoString() override;
    void update() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void editorShown (Label* lbl, TextEditor& ted) override;
    void labelTextChanged (Label* lbl) override;

    //==============================================================================
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent &event) override;

    //==============================================================================
    Gesture& getGesture();
    bool isSelected() const;
    void setSelected (bool);
    void setHighlighted (bool);
    void setSolo (bool);
    void startNameEntry();

    //==============================================================================
    const int id;

private:
    //==============================================================================
    void createLabel();
    void createButton();

    //==============================================================================
    Gesture& gesture;
    ScopedPointer<Label> gestureNameLabel;
    ScopedPointer<PlumeShapeButton> muteButton;
    
    bool on = gesture.isActive(), selected = false, highlighted = false, solo = false;

    const bool& dragMode;
    const int& draggedGesture;
    const int& draggedOverSlot;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureComponent)
};


class EmptyGestureSlotComponent : public PlumeComponent
{
public:
    //==============================================================================
    EmptyGestureSlotComponent (const int slotId, const bool& dragModeReference,
                                                 const int& draggedGestureReference,
                                                 const int& draggedOverSlotReference);
    ~EmptyGestureSlotComponent();

    //==============================================================================
    const String getInfoString() override;
    void update() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void mouseDrag (const MouseEvent &event) override;

    const int id;

private:
    //==============================================================================
    bool highlighted = false;
    
    const bool& dragMode;
    const int& draggedGesture;
    const int& draggedOverSlot;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EmptyGestureSlotComponent)
};