/*
  ==============================================================================

    InfoPanel.h
    Created: 15 Feb 2019 12:19:08pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"

//==============================================================================
/*
*/
class InfoPanel    : public Component,
                     public ActionListener
{
public:
    //==============================================================================
    InfoPanel (Button& hideButton);
    ~InfoPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void actionListenerCallback (const String &message) override;
    void mouseMove (const MouseEvent& event) override;

private:
    //==============================================================================
    bool alerted = false;
    std::unique_ptr<TextEditor> textEditor;
    Button& hideInfoButton;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPanel)
};
