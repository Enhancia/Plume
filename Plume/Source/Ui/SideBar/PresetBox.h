/*
  ==============================================================================

    PresetBoxModel.h
    Created: 10 Jan 2019 11:42:59am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Plugin/PluginProcessor.h"
#include "../Gesture/GesturePanel.h"

class PresetBox    : public ListBox,
                     public ListBoxModel,
                     public PlumeComponent,
                     private Label::Listener
{
public:
    //==============================================================================
    PresetBox (const String &componentName, PlumeProcessor& p);
    ~PresetBox();
    
    //==============================================================================
    //PlumeComponent methods
    
    const String getInfoString() override;
    void update() override;
    
    //==============================================================================
    //ListBox methods
    
    void paint (Graphics& g) override;
    void paintOverChildren (Graphics& g) override;
    
    //==============================================================================
    //ListBoxModel methods
    
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;

    Component* refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
    void listBoxItemClicked (int row, const MouseEvent& event) override;
    void listBoxItemDoubleClicked (int row, const MouseEvent& event) override;
    void backgroundClicked (const MouseEvent& event) override;
    void deleteKeyPressed (int lastRowSelected) override;
    void returnKeyPressed (int lastRowSelected) override;
    void selectedRowsChanged (int lastRowSelected) override;
    //void listWasScrolled() override;
    //var getDragSourceDescription (const SparseSet<int>& rowsToDescribe) override;
    String getTooltipForRow (int row) override;
    //MouseCursor getMouseCursorForRow (int row) override;
    
    //==============================================================================
    //Listener
    
    void labelTextChanged (Label* lbl) override;
    void editorHidden (Label* lbl, TextEditor& ted) override;
    
    //==============================================================================
    //PresetBox methods
    
    void startNewPresetEntry();
    void startRenameEntry (const int row);
    void deletePreset (const int row);

    
private:
    //==============================================================================
    static void menuCallback (int result, PresetBox* pBox, int row);
    void handleMenuResult (const int row, const int menuResult);
    
    //==============================================================================
    void setPreset (const int row);
    void createUserPreset (const String& presetName);
    void renamePreset (const String& newName);

    void prepareGesturePanelToPresetChange();
    void updateHeader();
    
    //==============================================================================
    PlumeProcessor& processor;
    int presetIdToEdit = -1;
    bool newPresetEntry = false;
    std::unique_ptr<Label> editLabel;
    std::unique_ptr<Label> dreamCrusher; /**< @Brief Evil's pointer, always null*/
    PopupMenu rightClickMenu;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBox)
};
