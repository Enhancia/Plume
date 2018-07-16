/*
  ==============================================================================

    GesturePanel.h
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class GesturePanel    : public Component
{
public:
    GesturePanel();
    ~GesturePanel();

    void paint (Graphics&) override;
    void resized() override;

private:
    class GestureComponent;
    
    OwnedArray<GestureComponent>;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GesturePanel)
};
