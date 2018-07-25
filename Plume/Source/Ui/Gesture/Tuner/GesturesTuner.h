/*
  ==============================================================================

    GesturesTuner.h
    Created: 23 Jul 2018 4:51:09pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/Gesture/Tuner/SymmetricalTuner.h"
#include "Ui/Gesture/Tuner/OneRangeTuner.h"
#include "Ui/Gesture/Tuner/TwoRangeTuner.h"
#include "Gesture/Vibrato.h"
#include "Gesture/PitchBend.h"
#include "Gesture/Tilt.h"
#include "Gesture/Wave.h"
#include "Gesture/Roll.h"

//==============================================================================
class VibratoTuner: public SymmetricalTuner
{
public:
    VibratoTuner(Vibrato& vib)
        :   SymmetricalTuner (vib.getValueReference(), vib.getRangeReference(), vib.gain, 500.0f)
    {}
    
    ~VibratoTuner()
    {}
private:
};

//==============================================================================
class PitchBendTuner: public TwoRangeTuner
{
public:
    PitchBendTuner(PitchBend& pb)
        :   TwoRangeTuner (pb.getValueReference(), pb.getRangeReference(),
                           pb.rangeLeft, pb.rangeRight, Range<float> (-90.0f, 90.0f),
                           String (CharPointer_UTF8 ("\xc2\xb0")))
    {}
    
    ~PitchBendTuner()
    {}
};


//==============================================================================
class TiltTuner: public OneRangeTuner
{
public:
    TiltTuner(Tilt& tilt)
        :   OneRangeTuner (tilt.getValueReference(), tilt.getRangeReference(), tilt.range, Range<float> (-90.0f, 90.0f),
                           String (CharPointer_UTF8 ("\xc2\xb0")))
    {}
    
    ~TiltTuner()
    {}
};

/*
//==============================================================================
class WaveTuner: public OneRangeTuner
{
public:
    WaveTuner(Wave& wave)
        :   OneRangeTuner (wave.getValueReference(), wave.getRangeReference(), wave.range, Range<float> (-90.0f, 90.0f),
                           String (CharPointer_UTF8 ("\xc2\xb0")))
    {}
    
    ~WaveTuner()
    {}
};

//==============================================================================
class RollTuner: public OneRangeTuner
{
public:
    RollTuner(Roll& roll)
        :   OneRangeTuner (roll.getValueReference(), roll.getRangeReference(), roll.range, Range<float> (-90.0f, 90.0f),
                           String (CharPointer_UTF8 ("\xc2\xb0")))
    {}
    
    ~RollTuner()
    {}
};
*/