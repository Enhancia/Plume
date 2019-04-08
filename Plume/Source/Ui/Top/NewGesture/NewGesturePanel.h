/*
  ==============================================================================

    NewGesturePanel.h
    Created: 28 Mar 2019 11:10:16am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class NewGesturePanel    : public PlumeComponent,
                           private Label::Listener,
                           private Button::Listener
{
public:
    //==============================================================================
    explicit NewGesturePanel (PlumeProcessor& proc);
    ~NewGesturePanel();

    //==============================================================================
    // Component Methods
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // PlumeComponent Methods
    const String getInfoString();
    void update();
    
    //==============================================================================
    // Callbacks
    void visibilityChanged() override;
    void mouseEnter (const MouseEvent &event) override;
    void mouseUp (const MouseEvent &event) override;
    void buttonClicked (Button* bttn) override;
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ed) override;

    //==============================================================================
    void showPanelForGestureID (const int gestureID);
    void hidePanel (const bool resetSelectedSlot = false);
    void updateGesturePanel();
    const int getLastSelectedSlot();

private:
    class GestureTypeSelector : public PlumeComponent
    {
    public:
        GestureTypeSelector (int gestType);
        ~GestureTypeSelector();

        const String getInfoString() override;
        void update() override;

        void paint (Graphics&) override;
        void resized() override;

        void mouseEnter (const MouseEvent &event) override;
        void mouseExit (const MouseEvent &event) override;

        bool isSelected();
        void setSelected (bool shouldBeSelected);
        void setHighlighted (bool shouldBeHighlighted);

        const int gestureType; // from Gesture::GestureType

    private:
        bool selected = false;
        bool highlighted = false;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureTypeSelector)
    };

    void selectGestureTypeExclusive (GestureTypeSelector* gestureTypeToSelect);
    void unselectGestureType();

    //==============================================================================
    // Overriden to prevent public use!
    // Use showPanelForGestureID (int) or hidePanel (bool) instead.
    void setVisible (bool) override; 

    //==============================================================================
    void createAndAddButtons();
    void createGestureSelectorButtons();
    void resizeGestureSelectorButtons (juce::Rectangle<int> buttonsArea);
    void createAndAddTextEditor();

    //==============================================================================
    juce::Rectangle<int> panelArea {0, 0, getWidth(), getHeight()};
    int selectedGestureType = -1;
    int selectedGestureSlot = -1;

    //==============================================================================
    PlumeProcessor& processor;
    
    //==============================================================================
    ScopedPointer<TextButton> addGestureButton;
    ScopedPointer<ShapeButton> closeButton;
    ScopedPointer<Label> gestureNameLabel;
    ScopedPointer<TextEditor> descriptionTextEditor;
    OwnedArray<GestureTypeSelector> gestureSelectors;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewGesturePanel)
};