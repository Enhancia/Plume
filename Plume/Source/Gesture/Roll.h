/*
  ==============================================================================

    Roll.h
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Gesture/Gesture.h"

/**
 *  \class Roll Roll.h
 *
 *  \brief Roll gesture class.
 *
 *  Gesture class for to create the roll effect. Has it's own variables and overrides the base Gesture class methods
 *  in accordance to the effect.
 */
class Roll : public Gesture
{
public:
    Roll (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
          float lowValue = -30.0f, float highValue = 30.0f, String description = "");
    ~Roll();
    
    //==============================================================================
    void addGestureMidi(MidiBuffer& midiMessages, MidiBuffer& plumeBuffer) override;
    int getMidiValue () override;
    
    void updateMappedParameters() override;
    float getValueForMappedParameter (Range<float> paramRange, bool reversed) override;
    
    //==============================================================================
    void updateValue (const Array<float> rawData) override;

    //==============================================================================
    RangedAudioParameter& rangeLow; /**< \brief Tilt's low range value. The full effect will happend between this and rangeHigh. */
	RangedAudioParameter& rangeHigh; /**< \brief Tilt's high range value. The full effect will happend between rangeLow and this. */

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Roll)
};