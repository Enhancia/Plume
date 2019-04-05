/*
  ==============================================================================

    GestureSettingsComponent.h
    Created: 28 Mar 2019 2:09:04pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Gesture/GestureArray.h"
#include "Wrapper/PluginWrapper.h"
#include "Ui/Gesture/Tuner/GesturesTuner.h"
#include "Ui/Gesture/Mapper/MapperComponent.h"

//==============================================================================
/*
*/
class GestureSettingsComponent : public PlumeComponent,
                                 private Button::Listener
{
public:
    //==============================================================================
    GestureSettingsComponent (Gesture& gest, GestureArray& gestArray, PluginWrapper& wrap);
    ~GestureSettingsComponent();

    //==============================================================================
    // PlumeComponent
    const String getInfoString() override;
    void update() override;
    
    //==============================================================================
    // PlumeComponent
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    //callbacks
    void buttonClicked (Button*) override;
    
    //==============================================================================
    int getGestureId();
    Tuner& getTuner();
    void updateDisplay();
    void updateComponents();
    
private:
    //==============================================================================
    void createTuner();
	void disableIfGestureWasDeleted();
    
    //==============================================================================
    bool disabled = false;

    //==============================================================================
    Gesture& gesture;
    const int gestureId;
    GestureArray& gestureArray;
    PluginWrapper& wrapper;
    
    ScopedPointer<Tuner> gestTuner;
    ScopedPointer<MapperComponent> gestMapper;
    ScopedPointer<TextButton> midiParameterToggle;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureSettingsComponent)
};
    
    
    
