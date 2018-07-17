/*
  ==============================================================================

    Gesture.h
    Created: 2 Jul 2018 3:15:48pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

/**
 *  \class Gesture Gesture.h
 *
 *  \brief Gesture base class.
 *
 *  Base class to create gestures, with virtual mehtods to create MIDI or map the gesture to a parameter.
 */
class Gesture
{
public:
    //==============================================================================
    /**
     *  \enum GestureType
     *
     *  \brief Holds every type of movement. If you create a new movement, add it to the enum.
     *
     */
    enum GestureType
    {
        vibrato =0,
        pitchBend,
        tilt,
        wave,
        roll,
        numGestures
    };
    
    //==============================================================================
    /**
     *  \brief Constructor.
     *
     *  Sets the gesture name and type, sets the "active" and "mapped" booleans to false.
     *
     *  \param gestName A string that holds the gesture name
     *  \param gestType The gesture type. Uses the gestureType enum.
     */
    Gesture(String gestName, int gestType, Range<float> r, float defaultValue = 0.0f)	: name (gestName), type (gestType)
    {
        on = false;
        mapped = false;
        range = r;
        value = defaultValue;
    }
    
    /**
     *  \brief Destructor.
     */
    ~Gesture() {}

    //==============================================================================
    // virtual getters used by the processor and the editor
    
    /**
     *  \brief Helper function to write the gesture's MIDI to a buffer.
     *
     *  Gets a midiBuffer (generally from the processor) and adds the gesture's MIDI. This method should be overridden
     *  by gestures to create the right MIDI. Generally you can use getMidiValue() to get the value and only create the
     *  right Midi message to add it to the buffer.
     */
    virtual void addGestureMidi (MidiBuffer& midiMessages) =0;
    
    /**
     *  \brief Method that returns the value that would be used to create a MIDI message.
     *
     *  Override this method to return the right value, according to gesture current value.
     *
     *  \return The correct value to create a midi message, between 0 and 127.
     */
    virtual int getMidiValue() =0;
    
    /**
     *  \brief Helper function to write the correct values for the mapped parameters.
     */
    virtual void updateMappedParameters() =0;
    virtual float getValueForMappedParameter(int paramId) =0;
    
    //==============================================================================
    virtual void updateValue (const Array<float> rawData) =0;
    virtual void addGestureParameters() =0;
    
    //==============================================================================
    // Getters to get const references to the value and range of the gesture.
    
    const float& getRawValueRef()
    {
        return value;
    }
    
    const Range<float>& getRangeRef()
    {
        return range;
    }
    
    //==============================================================================
    /**
     *  \brief Setter for the "on" boolean value.
     *
     *  \param shouldBeOn The boolean value to set. 
     */
    void setActive (bool shouldBeOn)
    {
        on = shouldBeOn;
    }
    
    /**
     *  \brief Setter for the "mapped" boolean value.
     *
     *  \param shouldBeMapped The boolean value to set.
     */
    void setMapped (bool shouldBeMapped)
    {
        mapped = shouldBeMapped;
    }
    
    /**
     *  \brief Getter for the "mapped" boolean value.
     */
    bool isMapped ()
    {
        return mapped;
    }
    
    //==============================================================================
    const String name; /** < Specific name of the gesture. By default [gesture_type]_default */
    const int type; /** < Type of Gesture. Int value from gestureType enum */
    
protected:
    //==============================================================================
    /**
     *  \brief Helper function to get convert a value and range to a MIDI ready value between 0 and 127.
     *  
     *  Use this function to get a value for a MIDI CC, or a modwheel MIDI for Tilt, Wave or Roll. If the value
     *  isn't within the range, the return value will be 0 or 127 depending on the value.
     *
     *  \param minVal Low value of the range
     *  \param maxVal High value of the range
     *  \param value  Current value inside the range
     */
    static int normalizeMidi (float minVal, float maxVal, float value)
    {
        int norm;
        float a, b;
  
        a = 127.0f / (maxVal - minVal);
        b = -a * minVal;
  
        norm = (a*value+b);
        if (norm < 0) norm = 0;
        if (norm > 127) norm = 127;
  
        return (int) norm;
    }
    
    /**
     *  \brief Helper function to map a floating point value to an int interval.
     *
     *  Used mostly to get the pitchBend value for Vibrato and PitchBend gestures. If the value is outside the range
     *  the return value will either be minNew or maxNew.
     * 
     *  \param minVal Low value of the range
     *  \param maxVal High value of the range
     *  \param value  Current value inside the range
     *  \param minNew low value of the new range
     *  \param maxNew high value of the new range
     */
    static int map (float value, float minVal, float maxVal, int minNew, int maxNew)
    {
        if (minVal == maxVal) return 0;
    
        if (value < minVal) return minNew;
        if (value > maxVal) return maxNew;
    
        return (minNew + int ((maxNew - minNew)*(value - minVal)/(maxVal-minVal)));
    }
    
    //==============================================================================
    bool on; /** < Boolean that represents if the movement is active or not. */
    bool mapped; /** < Boolean that represents if the movement is mapped or not */
    
    
    //==============================================================================
    float value; /** < Parameter that holds the current "raw" value of the gesture. Should be used and updated by subclasses */
    Range<float> range; /** < */
    
};