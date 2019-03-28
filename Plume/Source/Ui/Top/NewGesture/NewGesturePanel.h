/*
  ==============================================================================

    NewGesturePanel.h
    Created: 28 Mar 2019 11:10:16am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class NewGesturePanel    : public PlumeComponent,
                           private Label::Listener,
                           private Button::Listener
{
public:
    //==============================================================================
    explicit NewGesturePanel (PlumeProcessor& proc);
    ~NewGesturePanel();

    //==============================================================================
    // Component Methods
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // PlumeComponent Methods
    const String getInfoString();
    void update();
    
    //==============================================================================
    // Callbacks
    void visibilityChanged() override;
    void buttonClicked (Button* bttn) override;
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ed) override;

private:
    //==============================================================================
    juce::Rectangle<int> panelArea;
    
    PlumeProcessor& processor;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewGesturePanel)
};