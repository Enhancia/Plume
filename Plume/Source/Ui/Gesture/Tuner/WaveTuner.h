/*
  ==============================================================================

    VibratoTuner.h
    Created: 20 Jul 2018 5:11:13pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/Gesture/Tuner/SymmetricalTuner.h"
#include "Gesture/Vibrato.h"

class VibratoTuner: public SymmetricalTuner
{
public:
    VibratoTuner(Vibrato& vib)
        :   SymmetricalTuner (vib.getValueReference(), vib.getRangeReference(), vib.gain, 500.0f)
    {
    }
    
    ~VibratoTuner()
    {
    }
    
private:
};