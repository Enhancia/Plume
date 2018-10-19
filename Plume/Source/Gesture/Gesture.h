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

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
#define TRACE_OUT Logger::writeToLog ("[-FNC]  Leaving: " + String(__FUNCTION__))

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
     *  \enum MidiType
     *
     *  \brief Holds the several midi messages that can be sent to the wrapped plugin using midi Mapping.
     *
     */
    enum MidiType
    {
        controlChange =1,
        pitch,
        afterTouch,
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
        {
            TRACE_IN;
        }
        
        ~MappedParameter()
        {
            TRACE_IN;
        }
        
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
    Gesture(String gestName, int gestType, Range<float> maxRange,
            float defaultValue = 0.0f, int defaultCc = 1,
            Range<float> defaultMidiRange = Range<float> (0.0f, 1.0f))	: name (gestName), type (gestType)
    {
        TRACE_IN;
        on = false;
        mapped = false;
        midiMap = false;
        range = maxRange;
        value = defaultValue;
        cc = defaultCc;
        midiRange = defaultMidiRange;
    }
    
    /**
     *  \brief Destructor.
     */
    ~Gesture()
    {
        clearAllParameters();
    }

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
    // Static Methods to merge messages in a MidiBuffer
    
    /**
     *  \brief Helper function to prevent concurrent pitchWheel MIDI messages in a buffer.
     *
     *  First, the method will add an pitchWheel midi event to the buffer at time 1 (for a vibrato or pitchBend).
     *  Additionnaly, goes through the buffer to change all pitchWheel messages. This method will change their
     *  values by adding the difference to a neutral pitch value (8192).
     *
     *  \param midiMessages Reference to a MidiBuffer in which the pitch messages will be changed. 
     */
    static void addEventAndMergePitchToBuffer (MidiBuffer& midiMessages, int midiValue, int channel/*, int& basePitch*/)
    {
        MidiBuffer newBuff;
        int time;
        MidiMessage m;
        
        for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
        {
            if (m.isPitchWheel()) // checks for pitch wheel events
            {
                // Changes the reference to the event's pitch message value
                //basePitch = m.getPitchWheelValue();
                
                // Creates a pitch message with the right value
                //int newVal = basePitch - 8192 + midiValue;
                
                int newVal = m.getPitchWheelValue() - 8192 + midiValue;
                if (newVal < 0) newVal = 0;
                else if (newVal > 16383) newVal = 16383;
                
                DBG ("OldVal = " << m.getPitchWheelValue() << "| NewVal = " << newVal);
                
                m = MidiMessage::pitchWheel (m.getChannel(), newVal);
            }
            
            newBuff.addEvent (m, time);
        }
        /*
        // Computes the right value (uses midi value along with basePitch)
        midiValue = basePitch - 8192 + midiValue;
        if (midiValue < 0) midiValue = 0;
        else if (midiValue > 16383) midiValue = 16383;
        */
        
        // Adds gesture's initial midi message
        newBuff.addEvent (MidiMessage::pitchWheel (channel, midiValue), 1);
        
        midiMessages.swapWith (newBuff);
    }
    
    /**
     *  \brief Helper function to prevent concurrent cc (mostly modWheel) MIDI messages in a buffer.
     *
     *  First, the method will add an cc midi event to the buffer at time 1 (for a tilt or any midiMapped gesture).
     *  Additionnaly, goes through the buffer to change all the cc messages. This method will change their
     *  values by adding the parameter midiValue.
     *
     *  \param midiMessages Reference to a MidiBuffer in which the modWheel messages will be changed.
     */
    static void addEventAndMergeCCToBuffer (MidiBuffer& midiMessages, int midiValue, int ccValue, int channel)
    {
        MidiBuffer newBuff;
        int time;
        MidiMessage m;
        
        for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
        {
            if (m.isAftertouch()) // checks if right event
            {
                // Creates a cc message with the new value
                int newVal = m.getControllerValue() + midiValue;
                if (newVal > 127) newVal = 127;
                
                m = MidiMessage::controllerEvent (m.getChannel(), ccValue, newVal);
            }
            
            newBuff.addEvent (m, time);
        }
        
        // Adds gesture's initial cc message
        newBuff.addEvent (MidiMessage::controllerEvent (channel, ccValue, midiValue), 1);
        
        midiMessages.swapWith (newBuff);
    }
    
    /**
     *  \brief Helper function to send after touch MIDI messages in a buffer.
     *
     *  First, the method will add an channel midi event to the buffer at time 1.
     *  Additionnaly, goes through the buffer to change all the aftertouch/channel pressure messages.
     *  This method will change their values by adding the parameter midiValue.
     *
     *  \param midiMessages Reference to a MidiBuffer in which the modWheel messages will be changed.
     */
    static void addEventAndMergeAfterTouchToBuffer (MidiBuffer& midiMessages, int midiValue, int channel)
    {
        MidiBuffer newBuff;
        int time;
        MidiMessage m;
        
        for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
        { 
            // AfterTouchMessage
            if (m.isAftertouch())
            {
                // Creates a cc message with the new value
                int newVal = m.getAfterTouchValue() + midiValue;
                if (newVal > 127) newVal = 127;
                
                m = MidiMessage::aftertouchChange (m.getChannel(), m.getNoteNumber(), newVal);
            }
            
            // Channel pressure message (ie AT for all every note)
            if (m.isChannelPressure())
            {
                // Creates a cc message with the new value
                int newVal = m.getChannelPressureValue()  + midiValue;
                if (newVal > 127) newVal = 127;
                
                m = MidiMessage::channelPressureChange (m.getChannel(), newVal);
            }
            
            newBuff.addEvent (m, time);
        }
        
        // Adds gesture's initial cc message
        newBuff.addEvent (MidiMessage::channelPressureChange (channel, midiValue), 1);
        DBG ("AT Value: " << midiValue);
        
        midiMessages.swapWith (newBuff);
    }
    
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
     *  \brief Setter for the "midiMap" boolean value.
     *
     *  \param shouldBeMidiMapped The boolean value to set.
     */
    void setMidiMapped (bool shouldBeMidiMapped)
    {
        midiMap = shouldBeMidiMapped;
    }
    
    /**
     *  \brief Setter for the "cc" integer value.
     *
     *  \param ccValue The integer value to set.
     */
    void setCc (int ccValue)
    {
        cc = ccValue;
    }
    
    /**
     *  \brief Getter for the "cc" integer value.
     *
     *  \returns The cc value used by the gesture.
     */
    int getCc()
    {
        return cc;
    }
    
    /**
     *  \brief Getter for the "mapped" boolean value.
     */
    bool isMapped()
    {
        return mapped;
    }
    
    /**
     *  \brief Getter for the "midiMap" boolean value.
     */
    bool isMidiMapped()
    {
        return midiMap;
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
        TRACE_IN;
                                        
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
        TRACE_IN;
        parameterArray.remove (paramId);
        
        if (parameterArray.isEmpty()) mapped = false;
        
        sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
    }
    
    /**
     *  \brief Deletes all MappedParameter in the array.
     */
    void clearAllParameters()
    {
        TRACE_IN;
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
    
    /**
     *  \brief Checks if a specific parameter is already mapped to the gesture.
     *  
     *  \return True if the parameter is found for this gesture.
     */
    bool parameterIsMapped (int parameterId)
    {
        for (auto* param : parameterArray)
        {
            if (param->parameter.getParameterIndex() == parameterId) return true;
        }
        
        return false;
    }
    
    //==============================================================================
    const String name; /**< Specific name of the gesture. By default [gesture_type]_default */
    const int type; /**< Type of Gesture. Int value from gestureType enum */
    bool mapModeOn = false; /**< Boolean that indicates if the gesture looks for a new parameter to map */
    int midiType = Gesture::controlChange; /**< \brief Integer value that represents the midi type the gesture should provide if it is in midi map mode */
    Range<float> midiRange; /**< \brief Holds the range of values that the midi message should access. Between 0.0 and 1.0*/
    
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
        if (minVal == maxVal && value == minVal) return 0;
        
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
        if (minVal == maxVal && value == minVal) return minNew;
    
        if (value < minVal) return minNew;
        if (value > maxVal) return maxNew;
    
        return (minNew + int ((maxNew - minNew)*(value - minVal)/(maxVal-minVal)));
    }
    
    /**
     *  \brief Helper function to map an integer value to a new interval.
     *
     *  Used mostly in midi Mode for custom range midi messages.
     * 
     *  \param minVal Low value of the range
     *  \param maxVal High value of the range
     *  \param value  Current value inside the range
     *  \param minNew low value of the new range
     *  \param maxNew high value of the new range
     */
    static int mapInt (int value, int minVal, int maxVal, int minNew, int maxNew)
    {
        if (minVal == maxVal && value == minVal) return minNew;
    
        if (value < minVal) return minNew;
        if (value > maxVal) return maxNew;
    
        return (minNew + (maxNew - minNew)*(value - minVal)/(maxVal-minVal));
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
        if (minVal == maxVal && value == minVal) return paramRange.getStart();
        
        // Forces the interval to [0.0f 1.0f]
        if (paramRange.getStart() < 0.0f) paramRange.setStart (0.0f);
        if (paramRange.getEnd() > 1.0f)   paramRange.setEnd (1.0f);
        
        // Checks out of bounds values
        if (value < minVal) return paramRange.getStart();
        if (value > maxVal) return paramRange.getEnd();
        
        return (paramRange.getStart() + paramRange.getLength()*(value - minVal)/(maxVal - minVal));
    }
    
     /**
     *  \brief Helper function to handle the different MIDI messages that can be sent in midiMap mode.
     *
     *  The method will check the current type of MIDI signal the gesture is sending, and will process
     *  the according midi value depending on that and the midiRange parameter. The method also gets the
     *  current min and max midi values to handle both 7 bits (midiMax 127) and 14 bits (midiMAx 16383) MIDI messages.
     *
     *  \param midiMessages Reference to a MidiBuffer in which to write.
     *  \param value midi value to fit in the new range.
     *  \param midiMin minimum of "value"'s range. Pretty much always 0.
     *  \param midiMin maximum of "value"'s range. Can be 127 or 16383.
     *  \param channel midi channel.
     */
    void addMidiModeSignalToBuffer (MidiBuffer& midiMessages, int value, int midiMin, int midiMax, int channel)
    {
		if (!midiMap) return; //Does nothing if not in midi map mode

        int newMidi;
        
        // assigns the right midi value depending on the signal and the midiRange parameter, then adds message to buffer
        switch (midiType)
        {
			case (Gesture::pitch):
                newMidi = mapInt (value, midiMin, midiMax,
                                  map (midiRange.getStart(), 0.0f, 1.0f, 0, 16383),
                                  map (midiRange.getEnd(),   0.0f, 1.0f, 0, 16383));
                                  
                addEventAndMergePitchToBuffer (midiMessages, newMidi, channel);
                break;
            
			case (Gesture::controlChange):
                newMidi = mapInt (value, midiMin, midiMax,
                                  map (midiRange.getStart(), 0.0f, 1.0f, 0, 127),
                                  map (midiRange.getEnd(),   0.0f, 1.0f, 0, 127));
                                  
                addEventAndMergeCCToBuffer (midiMessages, newMidi, cc, channel);
                break;
            
			case (Gesture::afterTouch):
                newMidi = mapInt (value, midiMin, midiMax,
                                  map (midiRange.getStart(), 0.0f, 1.0f, 0, 127),
                                  map (midiRange.getEnd(),   0.0f, 1.0f, 0, 127));
                                  
                addEventAndMergeAfterTouchToBuffer (midiMessages, newMidi, channel);
                break;
            
            default:
                break;
        }
    }
    
    //==============================================================================
    bool on; /**< \brief Boolean that represents if the gesture is active or not. */
    bool mapped; /**< \brief Boolean that represents if the gesture is mapped or not. */
    bool midiMap; /**< \brief Boolean that tells if the gesture is currently in midiMap mode */
    int cc; /**< \brief Integer value that represents the CC used for the gesture in midiMap mode (default 1: modwheel) */
    
    //==============================================================================
    float value; /**< \brief Parameter that holds the current "raw" value of the gesture. Should be used and updated by subclasses. */
    Range<float> range; /**< \brief Attribute that holds the maximum range of values that the attribute "value" can take. */
	//int pitchReference = 8192; /**< \brief Base pitch value, that comes from external midi controllers */
    
    OwnedArray<MappedParameter> parameterArray;  /**< \brief Array of all the MappedParameter that the gesture controls. */
};