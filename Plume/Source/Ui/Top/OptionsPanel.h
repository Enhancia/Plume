/*
  ==============================================================================

    OptionsPanel.h
    Created: 24 Jan 2019 5:23:26pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class OptionsPanel    : public Component,
                        private TextPropertyComponent::Listener,
                        private Button::Listener
                        
{
public:
    //==============================================================================
    OptionsPanel (PlumeProcessor& proc);
    ~OptionsPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void mouseUp (const MouseEvent& event) override;
    void visibilityChanged() override;
    void textPropertyComponentChanged (TextPropertyComponent* tpc) override;
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    void createAndAddProperties();
    
    //==============================================================================
    Rectangle<int> optionsArea;
    ScopedPointer<PropertyPanel> settings;
    ScopedPointer<ShapeButton> scanButton;
    PlumeProgressBar* bar = nullptr;
    
    PlumeProcessor& processor;
    
    Value pluginDir;
    Value presetDir;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};
