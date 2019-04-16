/*
  ==============================================================================

    GestureSettingsPanel.h
    Created: 15 Apr 2019 10:39:14am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class GestureSettingsPanel    : public Component
{
public:
    GestureSettingsPanel();
    ~GestureSettingsPanel();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureSettingsPanel)
};
