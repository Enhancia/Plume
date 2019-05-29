/*
  ==============================================================================

    OptionsPanel.h
    Created: 24 Jan 2019 5:23:26pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"
#include "Ui/Top/Options/ScannerComponent.h"
#include "Ui/Common/TabbedPanelComponent.h"
#include "Ui/Top/Options/SubPanels/FileOptionsSubPanel.h"
#include "Ui/Top/Options/SubPanels/GeneralOptionsSubPanel.h"

//==============================================================================
/*
*/
class OptionsPanel    : public Component,
                        private Button::Listener
{
public:
    //==============================================================================
    explicit OptionsPanel (PlumeProcessor& proc);
    ~OptionsPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void mouseUp (const MouseEvent& event) override;
    void mouseEnter (const MouseEvent& event) override;
    void mouseExit (const MouseEvent& event) override;
    void visibilityChanged() override;

private:
    //==============================================================================
    juce::Rectangle<int> optionsArea;
    ScopedPointer<TabbedPanelComponent> tabbedOptions;
    ScopedPointer<ShapeButton> closeButton;
    PlumeProcessor& processor;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OptionsPanel)
};
