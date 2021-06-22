/*
  ==============================================================================

    HeaderComponent.h
    Created: 12 Dec 2018 4:53:18pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Plugin/PluginProcessor.h"
#include "../Gesture/GesturePanel.h"
#include "../Common/DualTextToggle.h"

//==============================================================================
/*
*/
class HeaderComponent    : public Component,
                           public PlumeComponent,
                           public Button::Listener,
                           private AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    HeaderComponent (PlumeProcessor& proc, Component& newPrst);
    ~HeaderComponent();

    //==============================================================================
    // PlumeComponent
    const String getInfoString() override;
    void update() override;
    
    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // Mouse Listener
    void mouseUp (const MouseEvent &event) override;
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void buttonClicked (Button* bttn) override;
    void parameterChanged (const String &parameterID, float newValue) override;
    
    //==============================================================================
    void createPluginMenu (KnownPluginList::SortMethod sort);
    void createPluginWindow();

    //==============================================================================
    void setPreviousPreset();
    void setNextPreset();

private:
    //==============================================================================
	static void pluginMenuCallback (int result, HeaderComponent* header);
	void handlePluginChoice (int chosenId);
    void createButtons();
    void setPresetWithOffset (const int offset);
    void prepareGesturePanelAndLoadPreset (const int presetId);

    //==============================================================================
    PlumeProcessor& processor;
    Component& newPresetPanel;
    
    PopupMenu pluginListMenu;
    std::unique_ptr<PlumeShapeButton> pluginListButton;
    std::unique_ptr<PlumeShapeButton> savePresetButton;
    std::unique_ptr<PlumeShapeButton> leftArrowButton;
    std::unique_ptr<PlumeShapeButton> rightArrowButton;
    std::unique_ptr<PlumeShapeButton> trackArmButton;
    std::unique_ptr<Label> pluginNameLabel;
    std::unique_ptr<Label> presetNameLabel;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComponent)
};
