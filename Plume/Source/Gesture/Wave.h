/*
  ==============================================================================

    Wave.h
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Gesture.h"

/**
 *  \class Vibrato Vibrato.h
 *
 *  \brief Vibrato gesture class.
 *
 *  Gesture class for to create the vibrato effect. Has it's own variables and overrides the base Gesture class methods
 *  in accordance to the effect.
 */
class Wave : public Gesture
{
public:
    Wave (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
          String description = "");
    ~Wave();
    
    //==============================================================================
    void addGestureMidi(MidiBuffer& midiMessages, MidiBuffer& plumeBuffer) override;
    void updateMidiValue () override;
    
    void updateMappedParameters() override;
    float computeMappedParameterValue (Range<float> paramRange, bool reversed) override;
    
    //==============================================================================
    void updateValue (const Array<float> rawData) override;
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Wave)
};