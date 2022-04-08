/*
  ==============================================================================

    FilterBox.h
    Created: 7 Feb 2019 5:39:29pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Plugin/PluginProcessor.h"

class FilterBox    : public ListBox,
                     public ListBoxModel
{
public:
    //==============================================================================
    FilterBox (const String &componentName, PlumeProcessor& p);
    ~FilterBox();
    
    //==============================================================================
    //ListBox methods
    
    void paint (Graphics& g) override;
    void paintOverChildren (Graphics& g) override;
    
    //==============================================================================
    //ListBoxModel methods
    
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;

    //Component* refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
    void listBoxItemClicked (int row, const MouseEvent& event) override;
    bool keyPressed (const KeyPress& key) override;
    void setFilter (int row);
    //void listBoxItemDoubleClicked (int row, const MouseEvent& event) override;
    //void backgroundClicked (const MouseEvent& event) override;
    //void deleteKeyPressed (int lastRowSelected) override;
    //void returnKeyPressed (int lastRowSelected) override;
    //void listWasScrolled() override;
    //var getDragSourceDescription (const SparseSet<int>& rowsToDescribe) override;
    //String getTooltipForRow (int row) override;
    //MouseCursor getMouseCursorForRow (int row) override;
    
private:
    //==============================================================================
    PlumeProcessor& processor;
    int currentRow = 0;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterBox)
};