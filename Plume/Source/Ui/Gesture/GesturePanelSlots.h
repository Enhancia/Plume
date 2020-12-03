/*
  ==============================================================================

    GesturePanelSlots.h
    Created: 27 May 2019 2:05:16pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Gesture/GestureArray.h"

class GestureComponent : public PlumeComponent,
                         private Label::Listener
{
public:
    //==============================================================================
    GestureComponent (Gesture& gest, GestureArray& gestArray,
                                     const bool& dragModeReference,
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
    void paintParameterSlotDisplay (Graphics& g, juce::Rectangle<int> area,
                                                 const int numRows,
                                                 const int numColumns,
                                                 const int margin = 0);
    void drawGesturePath (Graphics& g, juce::Rectangle<int> area);

    //==============================================================================
    Gesture& gesture;
    GestureArray& gestureArray;
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
    EmptyGestureSlotComponent (const int slotId, GestureArray& gestArray,
                                                 const bool& dragModeReference,
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
    
    GestureArray& gestureArray;
    const bool& dragMode;
    const int& draggedGesture;
    const int& draggedOverSlot;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EmptyGestureSlotComponent)
};