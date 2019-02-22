/*
  ==============================================================================

    InfoPanel.h
    Created: 15 Feb 2019 12:19:08pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"

//==============================================================================
/*
*/
class InfoPanel    : public Component,
                     public ActionListener
{
public:
    //==============================================================================
    InfoPanel();
    ~InfoPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void actionListenerCallback (const String &message) override;
    void mouseMove (const MouseEvent& event) override;

private:
    //==============================================================================
    String displayText = "Rien lol";
    ScopedPointer<TextEditor> textEditor;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPanel)
};
