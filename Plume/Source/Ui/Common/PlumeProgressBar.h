/*
  ==============================================================================

    PlumeProgressBar.h
    Created: 29 Jan 2019 3:54:30pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Ui/LookAndFeel/PlumeLookAndFeel.h"

//==============================================================================
/*
*/
class PlumeProgressBar    : public Component
{
public:
    static constexpr int BAR_H = 10;
    
    //==============================================================================
    PlumeProgressBar (float& prog, String& message,
                      const String prefix = "", const String finish = "Finished Scanning");
    ~PlumeProgressBar();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    float& progress;
    String& progressMessage;
    const String messagePrefix;
    const String finishMessage;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeProgressBar)
};
