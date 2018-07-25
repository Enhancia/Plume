/*
  ==============================================================================

    Gesture.h
    Created: 2 Jul 2018 3:15:48pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#define MAX_PARAMETER 4

/**
 *  \class Gesture Gesture.h
 *
 *  \brief Gesture base class.
 *
 *  Base class to create gestures, with virtual mehtods to create MIDI or map the gesture to a parameter.
 */
class Gesture   : public ChangeBroadcaster
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
    
     /**
     *  \struct MappedParameter
     *
     *  \brief Struct that holds a reference to an AudioProcessorParameter from the wrapped plugin and a range.
     *
     *  This struct is used by the Gesture class to control a parameter from the wrapped plugin, and the range of values that the
     *  gesture should control. The Gesture class implements several methods to modify an array of MappedParameter that it holds.
     */
    struct MappedParameter
    {
        MappedParameter(AudioProcessorParameter& p, Range<float> pRange)
            : parameter (p), range(pRange)
        {}
        
        ~MappedParameter() {}
        
        AudioProcessorParameter& parameter; /**< \brief Reference to a mapped Parameter from the wrapped Plugin. */
        Range<float> range; /**< \brief Range of values from the parameter that the Gesture controls. */
    };
    
    //==============================================================================
    /**
     *  \brief Constructor.
     *
     *  Sets the gesture name and type, sets the "active" and "mapped" booleans to false.
     *
     *  \param gestName A string that holds the gesture name
     *  \param gestType The gesture type. Uses the GestureType enum.
     *  \param maxRange The maximum values that the gesture's value can take.
     *  \param defaultValue The default value of the gesture's value attribute.
     */
    Gesture(String gestName, int gestType, Range<float> maxRange, float defaultValue = 0.0f)	: name (gestName), type (gestType)
    {
        on = false;
        mapped = false;
        midiMap = false;
        range = maxRange;
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
     *  by gestures to create the right MIDI. Generally you should use getMidiValue() to get the value and only create the
     *  Midi message to add it to the buffer. The midi messages are all by default placed in the buffer first timestamp.
     *
     *  \param midiMessages Reference to a MidiBuffer on which the messages should be added. 
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
     *
     *  Updates all parameters of this Gesture with their right values. Uses getValueForMappedParameters() to get each value.
     *
     */
    virtual void updateMappedParameters() =0;
    
    /**
     *  \brief Method that returns the value for a specific mapped parameter.
     *
     *  Override this function in derived classes to return a float between whithin the specified range.
     * 
     */
    virtual float getValueForMappedParameter(Range<float> paramRange) =0;
    
    //==============================================================================
    /**
     *  \brief Method that uses raw data to update the gestures raw value accordingly.
     *
     *  Override this function in derived classes to update the right variable (or processor parameter), with the right raw value.
     *
     *  \param rawData Array of floating point numbers representing Neova pre-processed values. 
     */
    virtual void updateValue (const Array<float> rawData) =0;
    
    /**
     *  \brief Adds the gesture settings as parameters to the processor's parameters array.
     *
     */
    virtual void addGestureParameters() {}
    
    //==============================================================================
    // Getters to get const references to the value and range of the gesture. Used by the display.
    
    /**
     *  \brief Getter for a reference to the raw value used by the gesture.
     */
    const float& getValueReference()
    {
        return value;
    }
    
    /**
     *  \brief Getter for the range of the raw value.
     */
    const Range<float>& getRangeReference()
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
    bool isMapped()
    {
        return mapped;
    }
    
    /**
     *  \brief Getter for the "active" boolean value.
     */
    bool isActive()
    {
        return on;
    }
    
    /**
     *  \brief Returns a string corresponding to the gesture's type.
     */
    String getTypeString()
    {
        switch (type)
        {
            case Gesture::vibrato:
                return "vibrato";
                break;
                
            case Gesture::pitchBend:
                return "pitchBend";
                break;
                
            case Gesture::tilt:
                return "tilt";
                break;
                
            case Gesture::wave:
                return "wave";
                break;
                
            case Gesture::roll:
                return "roll";
                break;
            
            default:
                return "unknown";
        }
    }
    
    /**
     *  \brief Method to know if the gesture currently creates a pitchWheel midi message.
     *
     *  \return true if the gesture creates a PitchWheel message.
     */
    bool affectsPitch()
    {
        if ((type == Gesture::vibrato || type == Gesture::pitchBend) && mapped == false)
        {
            return true;
        }
        
        return false;
    }
    
    //==============================================================================
    // Parameter related methods
    /**
     *  \brief Creates a new MappedParameter.
     */
    void addParameter (AudioProcessorParameter& param)
    {
        if (parameterArray.size() < MAX_PARAMETER)
        {
            parameterArray.add ( new MappedParameter (param, Range<float> (0.0f, 1.0f)));
            mapped = true;
        }
        sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
    }
    
    /**
     *  \brief Deletes a MappedParameter.
     */
    void deleteParameter(int paramId)
    {
        parameterArray.remove (paramId);
        
        if (parameterArray.isEmpty()) mapped = false;
        
        sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
    }
    
    /**
     *  \brief Deletes all MappedParameter in the array.
     */
    void clearAllParameters()
    {
        parameterArray.clear();
        mapped = false;
        sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
    }
    
    /**
     *  \brief Gets a reference to the parameter array.
     */
    OwnedArray<MappedParameter>& getParameterArray()
    {
        return parameterArray;
    }
    
    //==============================================================================
    const String name; /**< Specific name of the gesture. By default [gesture_type]_default */
    const int type; /**< Type of Gesture. Int value from gestureType enum */
    bool mapModeOn = false; /**< Boolean that indicates if the gesture looks for a new parameter to map */
    
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
    
    /**
     *  \brief Helper function to map a floating point value to the specified interval within [0.0f 1.0f].
     *
     *  Used to map the value of the gesture to one of the gestures parameters.
     * 
     *  \param minVal Low value of the range
     *  \param maxVal High value of the range
     *  \param value  Current value inside the range
     *  \param paramRange Range used by the parameter
     */
    static float mapParameter (float value, float minVal, float maxVal, Range<float> paramRange)
    {
        // Prevents dividing by 0
        if (minVal == maxVal) return paramRange.getStart();
        
        // Forces the interval to [0.0f 1.0f]
        if (paramRange.getStart() < 0.0f) paramRange.setStart (0.0f);
        if (paramRange.getEnd() > 1.0f)   paramRange.setEnd (1.0f);
        
        // Checks out of bounds values
        if (value < minVal) return paramRange.getStart();
        if (value > maxVal) return paramRange.getEnd();
    
        return (paramRange.getStart() + paramRange.getLength()*(value - minVal)/(maxVal - minVal));
    }
    
    //==============================================================================
    bool on; /**< \brief Boolean that represents if the gesture is active or not. */
    bool mapped; /**< \brief Boolean that represents if the gesture is mapped or not. */
    bool midiMap; /**< \brief Boolean that tells if the gesture is currently in midiMap mode */
    
    //==============================================================================
    float value; /**< \brief Parameter that holds the current "raw" value of the gesture. Should be used and updated by subclasses. */
    Range<float> range; /**< \brief Attribute that holds the maximum range of values that the attribute "value" can take. */
    
    OwnedArray<MappedParameter> parameterArray;  /**< \brief Array of all the MappedParameter that the gesture controls. */
};