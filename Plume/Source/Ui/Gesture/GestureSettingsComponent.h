/*
  ==============================================================================

    GestureSettingsComponent.h
    Created: 28 Mar 2019 2:09:04pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Gesture/GestureArray.h"
#include "../../Wrapper/PluginWrapper.h"
#include "../Common/TabbedPanelComponent.h"
#include "Tuner/GesturesTuner.h"
#include "SettingsTabs/RetractableMapAndMidiPanel.h"
#include "SettingsTabs/mappercomponent.h"
#include "SettingsTabs/DescriptionPanel.h"

//==============================================================================
/*
*/
class GestureSettingsComponent : public PlumeComponent,
                                 private Button::Listener
{
public:
    //==============================================================================
    GestureSettingsComponent (Gesture& gest, GestureArray& gestArray, PluginWrapper& wrap,
                              PlumeShapeButton& closeButton);
    ~GestureSettingsComponent();

    //==============================================================================
    // PlumeComponent
    const String getInfoString() override;
    void update() override;
    void update (const String& parameterThatChanged);
    
    //==============================================================================
    // PlumeComponent
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    //callbacks
    void buttonClicked (Button*) override;
    
    //==============================================================================
    const int getGestureId();
    Tuner& getTuner();
    void updateDisplay();
    void updateComponents();
    void updateMappedParameters();
    
private:
    //==============================================================================
    void paintBackground (Graphics&);

    //==============================================================================
    void createTuner();
    void createToggles();
    void createPanels();
    void showAppropriatePanel();
	void disableIfGestureWasDeleted();
    
    //==============================================================================
    bool disabled = false;

    //==============================================================================
    Gesture& gesture;
    const int gestureId;
    GestureArray& gestureArray;
    PluginWrapper& wrapper;
    const int HEADER_HEIGHT = 30;

    ScopedPointer<Tuner> gestTuner;
    ScopedPointer<MapperComponent> gestMapper;
    ScopedPointer<DualTextToggle> midiParameterToggle;
    ScopedPointer<PlumeShapeButton> muteButton;
    ScopedPointer<RetractableMapAndMidiPanel> retractablePanel;
    ScopedPointer<DescriptionPanel> descriptionPanel;
    PlumeShapeButton& closeButton;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureSettingsComponent)
};
