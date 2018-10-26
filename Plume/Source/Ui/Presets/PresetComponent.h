/*
  ==============================================================================

    PresetComponent.h
    Created: 20 Jun 2018 10:58:08am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"

//==============================================================================
/*
*/
class PresetComponent    : public Component,
                           private Button::Listener
{
public:
    //==============================================================================
    PresetComponent(PlumeProcessor& p);
    ~PresetComponent();

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    
    //==============================================================================
    void savePreset();
    void loadPreset();
    
    //==============================================================================
    void update();

private:
    //==============================================================================
    ScopedPointer<Label> nameLabel;

    ScopedPointer<TextButton> saveButton;
    ScopedPointer<TextButton> loadButton;
    
    //==============================================================================
    //String currentPreset;
    
    //==============================================================================
    PlumeProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetComponent)
};
