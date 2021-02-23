/*
  ==============================================================================

    GesturesTuner.h
    Created: 23 Jul 2018 4:51:09pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"

#include "Tuner.h"
#include "TwoRangeTuner.h"
#include "OneRangeTuner.h"
#include "VibratoTuner.h"

#include "../../../Gesture/Vibrato.h"
#include "../../../Gesture/PitchBend.h"
#include "../../../Gesture/Tilt.h"
#include "../../../Gesture/Wave.h"
#include "../../../Gesture/Roll.h"

//==============================================================================
class PitchBendTuner: public TwoRangeTuner
{
public:
    PitchBendTuner(PitchBend& pb)
        :   TwoRangeTuner (pb.getValueReference(), pb.getRangeReference(),
                           pb.rangeLeftLow, pb.rangeLeftHigh,
                           pb.rangeRightLow, pb.rangeRightHigh,
                           Range<float> (PLUME::UI::PITCHBEND_DISPLAY_MIN, PLUME::UI::PITCHBEND_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")))
    {}
    
    ~PitchBendTuner()
    {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchBendTuner)
};


//==============================================================================
class TiltTuner: public OneRangeTuner
{
public:
    TiltTuner(Tilt& tilt)
        :   OneRangeTuner (tilt.getValueReference(), tilt.getRangeReference(),
                           tilt.rangeLow,
                           tilt.rangeHigh,
                           Range<float> (PLUME::UI::TILT_DISPLAY_MIN, PLUME::UI::TILT_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")), OneRangeTuner::tilt)
    {}
    
    ~TiltTuner()
    {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TiltTuner)
};

/*
//==============================================================================
class WaveTuner: public OneRangeTuner
{
public:
    WaveTuner(Wave& wave)
        :   OneRangeTuner (wave.getValueReference(), wave.getRangeReference(), wave.range,
                           Range<float> (PLUME::UI::WAVE_DISPLAY_MIN, PLUME::UI::WAVE_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")), OneRangeTuner::wave)
    {}
    
    ~WaveTuner()
    {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveTuner)
};
*/

//==============================================================================
class RollTuner: public OneRangeTuner
{
public:
    RollTuner(Roll& roll)
        :   OneRangeTuner (roll.getValueReference(), roll.getRangeReference(),
                           roll.rangeLow, roll.rangeHigh,
                           Range<float> (PLUME::UI::ROLL_DISPLAY_MIN, PLUME::UI::ROLL_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")), OneRangeTuner::roll)
    {}
    
    ~RollTuner()
    {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RollTuner)
};