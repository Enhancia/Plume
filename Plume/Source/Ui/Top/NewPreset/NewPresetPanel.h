/*
  ==============================================================================

    NewPresetPanel.h
    Created: 12 Feb 2019 4:08:14pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/PlumeCommon.h"
#include "../../../Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class NewPresetPanel    : public Component,
                          private Label::Listener,
                          private Button::Listener
{
public:
    //==============================================================================
    explicit NewPresetPanel (PlumeProcessor& proc);
    ~NewPresetPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void visibilityChanged() override;
    void buttonClicked (Button* bttn) override;
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ed) override;

private:
    //==============================================================================
    void update();
    void createLabels();
    void setLabelProperties (Label& labelToSet, bool editable =true);
    void createBox();
    void createUserPreset();
    
    //==============================================================================
    juce::Rectangle<int> panelArea;
    
    ScopedPointer<Label> nameLabel;
    ScopedPointer<Label> authorLabel;
    ScopedPointer<Label> verLabel;
    ScopedPointer<Label> pluginLabel;
    ScopedPointer<ComboBox> typeBox;
    ScopedPointer<TextButton> cancelButton;
    ScopedPointer<TextButton> saveButton;
    
    PlumeProcessor& processor;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewPresetPanel)
};