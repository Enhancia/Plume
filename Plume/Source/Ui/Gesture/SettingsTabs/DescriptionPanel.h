/*
  ==============================================================================

    DescriptionPanel.h
    Created: 20 May 2019 11:19:36am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/PlumeCommon.h"
#include "../../../Gesture/Gesture.h"

//==============================================================================
/*
*/
class DescriptionPanel    : public Component,
                            public PlumeComponent,
                            private Label::Listener
{
public:
    DescriptionPanel (Gesture& g);
    ~DescriptionPanel();

    const String getInfoString() override;
    void update() override;
        
    void paint (Graphics&) override;
    void resized() override;

    void labelTextChanged (Label* lbl) override;
    void editorShown (Label* lbl, TextEditor& ed) override;

    void resetEditorToDefault();

private:
    Gesture& gesture;
    std::unique_ptr<Label> descriptionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DescriptionPanel)
};