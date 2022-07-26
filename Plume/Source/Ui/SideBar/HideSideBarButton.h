/*
  ==============================================================================

    HideSideBarButton.h
    Created: 29 May 2019 12:03:32pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"

//==============================================================================
/*
*/
class HideSideBarButton    : public Button
{
public:
    //==============================================================================
    HideSideBarButton();
    ~HideSideBarButton();

    //==============================================================================
    //Button
    void paintButton (Graphics&, bool, bool) override;

private:
    //=========================================================================
    void drawArrowPath (Graphics&, bool, bool);
    void drawButtonBackgroundPath (Graphics&);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HideSideBarButton)
};