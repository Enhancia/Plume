/*
  ==============================================================================

    Vibrato.h
    Created: 9 Jul 2018 3:04:42pm
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
class Vibrato : public Gesture
{
public:
    Vibrato (String gestName, int gestId, AudioProcessorValueTreeState& plumeParameters,
             float val = PLUME::gesture::VIBRATO_RANGE_DEFAULT,
             float thresh = PLUME::gesture::VIBRATO_THRESH_DEFAULT, String description = "");
    ~Vibrato();
    
    //==============================================================================
    void addGestureMidi(MidiBuffer& midiMessages, MidiBuffer& plumeBuffer) override;
    void updateMidiValue () override;
    
    bool shouldUpdateParameters() override;
    float computeMappedParameterValue (Range<float> paramRange, bool reversed) override;
    
    //==============================================================================
    void setIntensityValue (float newVal);
    std::atomic<float>& getIntensityReference();
    void updateValue (const Array<float> rawData) override;
    
    //==============================================================================
    bool getSend(); /**< \brief Getter for the send boolean value */
    
    //==============================================================================
    float gain; /**< Sensibility of the vibrato. From 0.0f (no effect) to 500.0f (maximum effect)*/
    float threshold; /**< threshold used to trigger the effect*/
    
    NormalisableRange<float> gainDisplayRange;
    NormalisableRange<float> thresholdDisplayRange;
    NormalisableRange<float> intensityRange;

private:
    void updateSendLogic();
    std::atomic<float> intensity; /**< Current intensity of the vibrato. The effect is triggered if this is above the threshold parameter*/
    
    // Booleans that represent the state of the vibrato
    bool send = false; /**< \brief Boolean to know if the gesture should send midi */
    bool vibLast = false; /**< \brief Boolean to know if the gesture sent midi in the last buffer*/
    float lastIntensity = -1.0f;
    bool wasBeingChangedIntensity = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Vibrato)
};