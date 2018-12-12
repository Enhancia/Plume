/*
  ==============================================================================

    SideBarComponent.h
    Created: 12 Dec 2018 4:52:31pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class SideBarComponent    : public Component
{
public:
    SideBarComponent();
    ~SideBarComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SideBarComponent)
};
