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
    void setNameLabel(const String nameLabelArg);
    void setAuthorLabel(const String authorLabelArg);
    void setVerLabel(const String verLabelArg);
    void setPluginLabel(const String pluginLabelArg);
    
    //==============================================================================
    void visibilityChanged() override;
    void buttonClicked (Button* bttn) override;
    bool keyPressed (const KeyPress& key) override;
    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ed) override;

    void saveUserPreset();
    void clearLabels();

private:
    //==============================================================================
    void update();
    void createLabels();
    void setLabelProperties (Label& labelToSet, bool editable =true);
    void createBox();
    
    //==============================================================================
    juce::Rectangle<int> panelArea;
    
    std::unique_ptr<Label> nameLabel;
    std::unique_ptr<Label> authorLabel;
    std::unique_ptr<Label> verLabel;
    std::unique_ptr<Label> pluginLabel;
    std::unique_ptr<ComboBox> typeBox;
    std::unique_ptr<TextButton> cancelButton;
    std::unique_ptr<TextButton> saveButton;
    
    PlumeProcessor& processor;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewPresetPanel)
};