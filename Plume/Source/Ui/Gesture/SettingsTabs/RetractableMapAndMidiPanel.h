/*
  ==============================================================================

    RetractableMapAndMidiPanel.h
    Created: 13 Jun 2019 10:39:01am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/PlumeCommon.h"

#include "../../../Wrapper/PluginWrapper.h"
#include "../../../Gesture/Gesture.h"
#include "MapperComponent.h"
#include "MidiModeComponent.h"

//==============================================================================
/*
*/
class RetractableMapAndMidiPanel    : public Component,
                                      public PlumeComponent,
                                      public Button::Listener,
                                      public ChangeListener
{
public:
    enum PanelMode
    {
        parameterMode =0,
        midiMode
    };

    struct Retractable
    {
        void setComponents (Component* bannerComponent, Component* bodyComponent);

        Component* banner = nullptr;
        Component* body = nullptr;
    };

    //==============================================================================
    RetractableMapAndMidiPanel (Gesture& gest, GestureArray& gestArr,
                                PluginWrapper& wrap);
    ~RetractableMapAndMidiPanel();

    //==============================================================================
    const String getInfoString() override;
    void update() override;
    void updateDisplay();
    void updateMidiRange (MidiRangeTuner::DraggableObject thumbToUpdate = MidiRangeTuner::none);
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void changeListenerCallback (ChangeBroadcaster* source) override;

    //==============================================================================
    bool isRetracted();
    void setPanelMode (PanelMode newMode);
    void setRetracted (bool shouldBeRetracted);
    void initializeParamCompArray();
    
    //==============================================================================
    const int bannerHeight = 40;

private:
    //==============================================================================
    void resized (Retractable& retractableToResize);
    void setRetracted (Retractable& retractableToRetract);

    //==============================================================================
    void createHideBodyButtonPath();
    void initializeHideBodyButton();

    //==============================================================================
    PanelMode panelMode = parameterMode;
    bool retracted = false;

    //==============================================================================
    Gesture& gesture;
    GestureArray& gestureArray;
    PluginWrapper& wrapper;

    Retractable parametersRetractable;
    Retractable midiRetractable;

    std::unique_ptr<MapperBanner> parametersBanner;
    std::unique_ptr<MapperComponent> parametersBody;
    std::unique_ptr<MidiBanner> midiBanner;
    std::unique_ptr<MidiModeComponent> midiBody;
    std::unique_ptr<ShapeButton> hideBodyButton;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RetractableMapAndMidiPanel)
};