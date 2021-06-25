/*
  ==============================================================================

    Gesture.h
    Created: 2 Jul 2018 3:15:48pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/PlumeCommon.h"

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

        numGestureTypes
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
        MappedParameter(AudioProcessorParameter& p, Range<float> pRange, bool rev = false)
            : parameter (p), range(pRange), reversed (rev)
        {
                    }

        MappedParameter (const MappedParameter& other)
            : parameter (other.parameter), range(other.range), reversed (other.reversed)
        {
                    }
        
        ~MappedParameter()
        {
                    }
        
        AudioProcessorParameter& parameter; /**< \brief Reference to a mapped Parameter from the wrapped Plugin. */
        Range<float> range; /**< \brief Range of values from the parameter that the Gesture controls. */
        bool reversed; /**< \brief Boolean that tells if the parameter's range should be inverted. */
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
    Gesture (String gestName, int gestType, int gestId, const NormalisableRange<float> maxRange,
             AudioProcessorValueTreeState& plumeParameters, const String valueId = String(),
             String gestureDescription = String(),
             float defaultValue = 0.0f, int defaultCc = 1,
             Range<float> defaultMidiRange = Range<float> (0.0f, 1.0f));
    
    /**
     *  \brief Destructor.
     */
    ~Gesture();

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
    virtual void addGestureMidi (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer) =0;
    
    /**
     *  \brief Getter for the last midi value
     *
     *  \return The correct value to create a midi message, between 0 and 127.
     */
    int getMidiValue();
    
    /**
     *  \brief Method that stores the value that will be used to create a MIDI message.
     *
     *  Override this method to store the right value, according to gesture current value.
     */
    virtual void updateMidiValue() =0;

    /**
     *  \brief Helper function to write the correct values for the mapped parameters.
     *
     *  Updates all mapped parameters of this Gesture with their right values. Uses computeMappedParameterValues() to get each value.
     *
     */
    virtual void updateMappedParameters() =0;
    
    /**
     *  \brief Method that returns the value for a specific mapped parameter.
     *
     *  Override this method in derived classes to return a float within the specified range.
     * 
     */
    virtual float computeMappedParameterValue (Range<float> paramRange, bool reversed = false) =0;
    
    //==============================================================================
    /**
     *  \brief Method that uses raw data to update the gestures raw value accordingly.
     *
     *  Override this method in derived classes to update the right variable (or processor parameter), with the right raw value.
     *
     *  \param rawData Array of floating point numbers representing Neova pre-processed values. 
     */
    virtual void updateValue (const Array<float> rawData) =0;
    
    
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
    static void addEventAndMergePitchToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer,
                                               int midiValue, int channel);
    
    /**
     *  \brief Helper function to prevent concurrent cc (mostly modWheel) MIDI messages in a buffer.
     *
     *  First, the method will add an cc midi event to the buffer at time 1 (for a tilt or any midiMapped gesture).
     *  Additionnaly, goes through the buffer to change all the cc messages. This method will change their
     *  values by adding the parameter midiValue.
     *
     *  \param midiMessages Reference to a MidiBuffer in which the modWheel messages will be changed.
     */
    static void addEventAndMergeCCToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer,
                                            int midiValue, int ccValue, int channel);
    
    /**
     *  \brief Helper function to send after touch MIDI messages in a buffer.
     *
     *  First, the method will add an channel midi event to the buffer at time 0.296875
     *  Additionnaly, goes through the buffer to change all the aftertouch/channel pressure messages.
     *  This method will change their values by adding the parameter midiValue.
     *
     *  \param midiMessages Reference to a MidiBuffer in which the modWheel messages will be changed.
     */
    static void addEventAndMergeAfterTouchToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer,
                                                    int midiValue, int channel);
    
    //==============================================================================
    // Getter to the midiValue taking the midi mode range into account
    
    /**
     *  \brief Getter to the midi value taking the midiRange parameter into account.
     */
    int getRescaledMidiValue();
    
    //==============================================================================
    // Getters and setters to the value and range of the gesture. Used by the display among others.
    /**
     *  \brief Getter for a reference to the raw value used by the gesture.
     */
    void setGestureValue (float newVal);
    
    float getGestureValue() const;
    
    /**
     *  \brief Getter for a reference to the raw value used by the gesture.
     */
    std::atomic<float>& getValueReference();
    
    /**
     *  \brief Getter for the range of the raw value.
     */
    NormalisableRange<float> getRangeReference();
    
    //==============================================================================
    // Getters and setters for various parameters
    /**
     *  \brief Setter for the "on" boolean value.
     *
     *  \param shouldBeOn The boolean value to set. 
     */
    void setActive (bool shouldBeOn);
    
    /**
     *  \brief Setter for the "mapped" boolean value.
     *
     *  \param shouldBeMapped The boolean value to set.
     */
    void setMapped (bool shouldBeMapped);
    
    /**
     *  \brief Setter for the "midiOnParameterOff" boolean value.
     *
     *  \param shouldBeSendingMidi The boolean value to set. If true the gesture will affect midi.
                                   If false it will affect parameters.
     */
    void setGeneratesMidi (bool shouldBeSendingMidi);
    
    /**
     *  \brief Setter for the "cc" integer value.
     *
     *  \param ccValue The integer value to set.
     */
    void setCc (int ccValue);
    
    /**
     *  \brief Getter for the "cc" integer value.
     *
     *  \returns The cc value used by the gesture.
     */
    int getCc() const;
    /**
     *  \brief Setter for the "midiReverse" boolean value.
     *
     *  \param ccValue The integer value to set.
     */
    void setMidiReverse (bool shouldBeReversed);
    
    /**
     *  \brief Getter for the "midiReverse" boolean value.
     *
     *  \returns The cc value used by the gesture.
     */
    bool getMidiReverse() const;
    
    /**
     *  \brief Getter for the "mapped" boolean value.
     */
    bool isMapped() const;
    
    /**
     *  \brief Getter for the "midiMap" boolean value.
     */
    bool generatesMidi() const;
    
    /**
     *  \brief Setter for midiLow parameter float value.
     *
     *  \param newValue The float value to set.
     */
    void setMidiLow (float newValue, bool checkOtherValue = true);
    
    /**
     *  \brief Setter for midiHigh parameter float value.
     *
     *  \param newValue The float value to set.
     */
    void setMidiHigh (float newValue, bool checkOtherValue = true);
    
    /**
     *  \brief Getter for midiLow parameter float value.
     */
    float getMidiLow() const;
    
    /**
     *  \brief Getter for midiHigh parameter float value.
     */
    float getMidiHigh() const;
    
    /**
     *  \brief Getter for the "active" boolean value.
     */
    bool isActive() const;
    
    /**
     *  \brief Returns a string corresponding to the gesture's type.
     */
    String getName() const;
    /**
     *  \brief Returns a string corresponding to the gesture's type.
     */
    void setName (String nameToSet);
    
    /**
     *  \brief Returns a string corresponding to the gesture's description.
     */
    String getDescription() const;
    /**
     *  \brief Returns a string corresponding to the gesture's description.
     */
    void setDescription (String descrptionToSet);

    /**
     *  \brief Returns a string corresponding to the gesture's type.
     */
    String getTypeString (bool capitalized = false) const;
    /**
     *  \brief Returns a string corresponding to a specified gesture type.
     */
    static String getTypeString (int gestureType, bool capitalized = false);

    /**
       \brief  Returns a string corresponding to the gesture's type.

               Call this method to return the gesture instance's type description.
               This can be used by the interface to get a description while manipulating
               a Gesture pointer or reference.
     */
    String getGestureTypeDescription() const;

    /**
       \brief  Returns a string corresponding to a specified gesture type.

               Call this method to return a specific gesture type description.
               This can be used by the interface to describe a gesture while
               not manipulating gestures (ie the gesture type selection on the 
               newGesturePanel)
     */
    static String getGestureTypeDescription (int gestureType);

    /**
       \brief  Returns the highlight colour corresponding to the gesture's type and active state.

               Call this method to return a specific gesture type description.
               This can be used by the interface to draw the gesture's panels
     */
    Colour getHighlightColour() const;

    /**
       \brief  Returns the highlight colour corresponding to the gesture's type, for a specified ON or OFF state.

               Call this method to return a specific gesture type description.
               This can be used by the interface to draw the gesture's panels
     */
    Colour getHighlightColour (bool gestureIsActive) const;

    /**
       \brief  Returns the highlight colour corresponding to a specified gesture type.

               Call this method to return a specific gesture type colour.
               This can be used by the interface to draw the gesture's panels.
     */
    static Colour getHighlightColour (int gestureType, bool gestureIsActive = true);

    /**
     *  \brief Method to know if the gesture currently creates a pitchWheel midi message.
     *
     *  \return true if the gesture creates a PitchWheel message.
     */
    bool affectsPitch();
    
    //==============================================================================
    // Parameter related methods
    /**
     *  \brief Creates a new MappedParameter.
     */
    void addParameter (AudioProcessorParameter& param,
                       Range<float> r = Range<float> (0.0f, 1.0f), bool rev = false);
    
    /**
     *  \brief Deletes a MappedParameter.
     */
    void deleteParameter(int paramId);

    /**
     *  \brief Replaces a MappedParameter with a new one.
     */
    void replaceParameter (int paramIdToReplace,
                           AudioProcessorParameter& param,
                           Range<float> r = Range<float> (0.0f, 1.0f), bool rev = false);
    
    /**
     *  \brief Deletes all MappedParameter in the array.
     */
    void clearAllParameters (bool sendNotification = true);

    /**
     *  \brief Getter for the number of mapped parameters.
     */
    int getNumParameters() const;

    /**
     *  \brief Gets a reference to the parameter array.
     */
    OwnedArray<MappedParameter>& getParameterArray();
    
    /**
     *  \brief Checks if a specific parameter is already mapped to the gesture.
     *  
     *  \return True if the parameter is found for this gesture.
     */
    bool parameterIsMapped (int parameterId);

    /**
     *  \brief Replaces the mapped parameters with another gesture's.
     */
    void swapParametersWithOtherGesture (Gesture& other);
    
    //==============================================================================
    const int type; /**< \brief Type of Gesture. Int value from gestureType enum */
    const int id; /**< \brief Int that represents the number of the gesture */
    
    bool mapModeOn = false; /**< \brief Boolean that indicates if the gesture looks for a new parameter to map */
    int midiType = Gesture::controlChange; /**< \brief Integer value that represents the midi type the gesture should provide if it is in midi map mode */
    float midiHigh; /**< \brief Holds the lower end of the range of values that the midi message should access. Between 0.0 and 1.0*/
    float midiLow; /**< \brief Holds the higher end of the range of values that the midi message should access. Between 0.0 and 1.0*/
    
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
     *  \param val  Current value inside the range
     */
    static int normalizeMidi (float val, float minVal, float maxVal, bool is14BitMidi = false, bool reversed = false);
    
    /**
     *  \brief Helper function to map a floating point value to an int interval.
     *
     *  Used mostly to get the pitchBend value for Vibrato and PitchBend gestures. If the value is outside the range
     *  the return value will either be minNew or maxNew.
     * 
     *  \param minVal Low value of the range
     *  \param maxVal High value of the range
     *  \param val  Current value inside the range
     *  \param minNew low value of the new range
     *  \param maxNew high value of the new range
     */
    static int map (float val, float minVal, float maxVal, int minNew, int maxNew);
    
    /**
     *  \brief Helper function to map an integer value to a new interval.
     *
     *  Used mostly in midi Mode for custom range midi messages.
     * 
     *  \param minVal Low value of the range
     *  \param maxVal High value of the range
     *  \param val  Current value inside the range
     *  \param minNew low value of the new range
     *  \param maxNew high value of the new range
     */
    static int map (int val, int minVal, int maxVal, int minNew, int maxNew);
    
    /**
     *  \brief Helper function to map a floating point value to the specified interval within [0.0f 1.0f].
     *
     *  Used to map the value of the gesture to one of the gestures parameters.
     * 
     *  \param minVal Low value of the range
     *  \param maxVal High value of the range
     *  \param val  Current value inside the range
     *  \param paramRange Range used by the parameter
     */
    static float mapParameter (float val, float minVal, float maxVal,
                               Range<float> paramRange, bool reversed = false);
    
     /**
     *  \brief Helper function to handle the different MIDI messages that can be sent in midiMap mode.
     *
     *  The method will check the current type of MIDI signal the gesture is sending, and will process
     *  the according midi value depending on that and the midiRange parameter. The method also gets the
     *  current min and max midi values to handle both 7 bits (midiMax 127) and 14 bits (midiMAx 16383) MIDI messages.
     *
     *  \param midiMessages Reference to a MidiBuffer in which to write.
     *  \param val midi value to fit in the new range.
     *  \param midiMin minimum of "value"'s range. Pretty much always 0.
     *  \param midiMin maximum of "value"'s range. Can be 127 or 16383.
     *  \param channel midi channel.
     */
    void addRightMidiSignalToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer, int channel);
    void addRightMidiSignalToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer, int channel, int value);
    
    //==============================================================================
    String name; /**< \brief Specific name of the gesture. By default it is the gesture type*/
    String description; /**< \brief User specified description of the gesture. By default it is empty*/
    bool mapped; /**< \brief Boolean that represents if the gesture is mapped or not. */
    int currentMidi = -1; /**< \brief Integer value that represents the midiValue to send to the next midi buffer*/
    int lastMidi = -1; /**< \brief Integer value that represents the midiValue supposedely sent to the previous midiBuffer */
    
    //==============================================================================
    std::atomic<float>* valueRef; /**< \brief Parameter that holds the current "raw" value of the gesture. Should be used and updated by subclasses. */
    const NormalisableRange<float> range; /**< \brief Attribute that holds the maximum range of values that the gesture can take. */
	//int pitchReference = 8192; /**< \brief Base pitch value, that comes from external midi controllers */
	
    //==============================================================================
	RangedAudioParameter& value; /**< \brief Float parameter that holds the gesture's raw value in the [0.0f 1.0f]. Should be normalized using "range". */
    bool wasBeingChanged = false;
    float lastValue = -1.0f;

    //==============================================================================
	bool on; /**< \brief Boolean parameter that represents if the gesture is active or not] range. */
    bool midiReverse; /**< \brief Boolean parameter that represents if the gesture midi reverse is active or not. */
	bool midiOnParameterOff; /**< \brief Boolean parameter that represents if the gesture is set to midi mode or not. */
	int cc; /**< \brief Float parameter with an integer value for CC used by the gesture in midiMap mode (default 1: modwheel). */
	
    OwnedArray<MappedParameter> parameterArray;  /**< \brief Array of all the MappedParameter that the gesture controls. */
    CriticalSection parameterArrayLock;
    CriticalSection gestureValueLock;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Gesture)
    //JUCE_DECLARE_WEAK_REFERENCEABLE (Gesture)
};
