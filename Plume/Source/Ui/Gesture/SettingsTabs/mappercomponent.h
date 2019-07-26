/*
  ==============================================================================

    Mapper.h
    Created: 24 Jul 2018 8:44:18pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"

#include "Wrapper/PluginWrapper.h"
#include "Gesture/Gesture.h"
#include "Ui/Gesture/SettingsTabs/MappedParameterComponent.h"
#include "Ui/Gesture/SettingsTabs/MidiModeComponent.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

//==============================================================================
/*
*/
class MapperComponent    : public Component
{
public:
    //==============================================================================
    MapperComponent (Gesture& gest, GestureArray& gestArr,
                     PluginWrapper& wrap);
    ~MapperComponent();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void updateDisplay();
    void updateComponents();
    void initializeParamCompArray();
    void updateParamCompArray();
    
    //==============================================================================
    bool mapModeOn;

private:
    //==============================================================================
    Gesture& gesture;
    GestureArray& gestureArray;
    PluginWrapper& wrapper;
    
    //==============================================================================
    CriticalSection paramCompArrayLock;
    
    //==============================================================================
    bool allowDisplayUpdate = true;
    const int NUM_ROWS = 1, NUM_COLUMNS = 6;
    
    void addAndMakeArrayVisible();
    void resizeArray (juce::Rectangle<int> bounds, const int numColumns, const int numRows);
    
    //==============================================================================
    OwnedArray<MappedParameterComponent> paramCompArray;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapperComponent)
};


class MapperBanner    : public Component,
                        private Button::Listener
{
public:
    //==============================================================================
    MapperBanner (Gesture& gest, GestureArray& gestArr, PluginWrapper& wrap);
    ~MapperBanner();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

    //==============================================================================
    void updateComponents();

private:
    void paintParameterSlotDisplay (Graphics& g, juce::Rectangle<int> area,
                                                 const int numRows,
                                                 const int numColumns,
                                                 const int margin = 0);

    //==============================================================================
    Gesture& gesture;
    GestureArray& gestureArray;
    PluginWrapper& wrapper;

    //==============================================================================
    const int NUM_ROWS = 2, NUM_COLUMNS = 3;
    ScopedPointer<TextButton> mapButton;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapperBanner)
};