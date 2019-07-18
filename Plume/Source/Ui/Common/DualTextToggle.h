/*
  ==============================================================================

    DualTextToggleComponent.h
    Created: 9 Apr 2019 2:40:13pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/LookAndFeel/PlumeLookAndFeel.h"
#include "Common/PlumeCommon.h"

//==============================================================================
/*
*/
class DualTextToggle    : public Component
{
public:
    enum DualToggleStyle
    {
        oneStateVisible =0,
        twoStatesVisible,
        toggle,
        toggleWithTopText
    };

    //==============================================================================
    DualTextToggle() = default;
    DualTextToggle (String falseStateText, String trueStateText,
                    DualToggleStyle initialStyle = oneStateVisible);

    DualTextToggle (String falseStateText, String trueStateText,
                    Colour falseStateColour, Colour trueStateColour,
                    DualToggleStyle initialStyle = oneStateVisible);
    ~DualTextToggle();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void mouseUp (const MouseEvent &event) override;

    //==============================================================================
    void setStyle (DualToggleStyle style);
    void setToggleState (bool stateToSet);
    bool getToggleState();
    void switchToggleState();

    //==============================================================================
    void setFalseStatetext (String textToSet);
    void setTrueStatetext (String textToSet);
    void setFalseStateColour (Colour coulourToSet);
    void setTrueStateColour (Colour coulourToSet);

    std::function<void()> onStateChange = nullptr;

private:
    //==============================================================================
    void paintOneStateVisibleToggle (Graphics&);
    void paintTwoStateVisibleToggle (Graphics&);
    void paintStateInAreaWithAlpha (Graphics& g, bool stateToPaint,
                                                 juce::Rectangle <int> areaToPaint,
                                                 float alpha =1.0f);
    void paintToggle (Graphics& g, juce::Rectangle<int> areaToPaint);
    void paintToggleWithTopText (Graphics&);

    //==============================================================================
    bool state = false;
    DualToggleStyle style = oneStateVisible;
    String stateFalseText = String(), stateTrueText = String();
    Colour stateFalseColour = Colour (0xf4f412), stateTrueColour = Colour (0x369ad5);

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DualTextToggle)
};


class AudioParameterBoolToggle : public DualTextToggle
{
public:
    AudioParameterBoolToggle (AudioParameterBool& p);
    ~AudioParameterBoolToggle();

private:
    //==============================================================================
    AudioParameterBool& param;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioParameterBoolToggle)
};