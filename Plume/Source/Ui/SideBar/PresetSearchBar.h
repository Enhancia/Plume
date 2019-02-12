/*
  ==============================================================================

    PresetSearchBar.h
    Created: 12 Feb 2019 9:40:15am
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
class PresetSearchBar    : public Component,
                           private Label::Listener,
                           private TextEditor::Listener,
                           private Button::Listener
{
public:
    //==============================================================================
    PresetSearchBar (PlumeProcessor& proc);
    ~PresetSearchBar();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void labelTextChanged (Label* lbl) override;
    void editorShown  (Label* lbl, TextEditor& ed) override;
    void editorHidden (Label* lbl, TextEditor& ed) override;
    void textEditorTextChanged (TextEditor& ed) override;

private:
    //==============================================================================
    ScopedPointer<Label> searchLabel;
    ScopedPointer<ShapeButton> cancelButton;
    
    //==============================================================================
    PlumeProcessor& processor;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetSearchBar)
};
