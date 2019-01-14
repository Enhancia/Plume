/*
  ==============================================================================

    PresetBoxModel.h
    Created: 10 Jan 2019 11:42:59am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"

class PresetBoxModel    : public ListBoxModel
{
public:
    PresetBoxModel (PlumeProcessor& p);
    ~PresetBoxModel();
    
    int getNumRows() override;
    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;

    //Component* refreshComponentForRow (int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
    //void listBoxItemClicked (int row, const MouseEvent&) override;
    void listBoxItemDoubleClicked (int row, const MouseEvent&) override;
    //void backgroundClicked (const MouseEvent&) override;
    //void deleteKeyPressed (int lastRowSelected) override;
    //void returnKeyPressed (int lastRowSelected) override;
    //void listWasScrolled() override;
    //var getDragSourceDescription (const SparseSet<int>& rowsToDescribe) override;
    //String getTooltipForRow (int row) override;
    //MouseCursor getMouseCursorForRow (int row) override;
    
private:
    //==============================================================================
    PlumeProcessor& processor;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBoxModel)
};