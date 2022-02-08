/*
  ==============================================================================

    Tilt.h
    Created: 9 Jul 2018 2:57:36pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Gesture.h"

/**
 *  \class Tilt Tilt.h
 *
 *  \brief Tilt gesture class.
 *
 *  Gesture class for to create the tilt effect. Has it's own variables and overrides the base Gesture class methods
 *  in accordance to the effect.
 */
class Tilt : public Gesture
{
public:
    Tilt (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
          float lowValue = PLUME::gesture::TILT_DEFAULT_MIN,
          float highValue = PLUME::gesture::TILT_DEFAULT_MAX, String description = "");
    ~Tilt();
    
    //==============================================================================
    void addGestureMidi(MidiBuffer& midiMessages, MidiBuffer& plumeBuffer) override;
    void updateMidiValue () override;
    
    bool shouldUpdateParameters() override;
    float computeMappedParameterValue (Range<float> paramRange, bool reversed) override;
    
    //==============================================================================
    void updateValue (const Array<float> rawData) override;
    
    //============================================================================== 
    float rangeLow; /**< \brief Tilt's low range value. The full effect will happend between this and rangeHigh. */
	  float rangeHigh; /**< \brief Tilt's high range value. The full effect will happend between rangeLow and this. */
    
    NormalisableRange<float> tiltDisplayRange;
	
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Tilt)
};