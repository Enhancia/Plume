/*
==============================================================================

Gesture.cpp
Created: 10 Apr 2019 3:06:34pm
Author:  Alex

==============================================================================
*/

#include "Gesture.h"

Gesture::Gesture (String gestName, int gestType, int gestId, const NormalisableRange<float> maxRange,
         		  AudioProcessorValueTreeState& plumeParameters, const String valueId, String gestureDescription,
         		  float defaultValue, int defaultCc, Range<float> defaultMidiRange)
        
         		  : type (gestType), name (gestName), id (gestId), range (maxRange), description (gestureDescription), plumeParametersRef (plumeParameters)
{
    mapped = false;
    setGestureValue (defaultValue);
    setMidiLow (defaultMidiRange.getStart(), false);
    setMidiHigh (defaultMidiRange.getEnd(), false);
    setMidiReverse (false);
    setGeneratesMidi (gestType == (int) Gesture::vibrato ||
                      gestType == (int) Gesture::pitchBend);
	setCc (defaultCc);
}

Gesture::~Gesture()
{
    dispatchPendingMessages();
    removeAllChangeListeners();

    clearAllParameters();
}

//==============================================================================
// Static Methods to merge messages in a MidiBuffer

void Gesture::addEventAndMergePitchToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer,
										   	 int midiValue, int channel)
{
    MidiBuffer newBuff;
    int time;
    MidiMessage m;
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        if (m.isPitchWheel()) // checks for pitch wheel events
        {   
            int newVal = m.getPitchWheelValue() - 8192 + midiValue;
            if (newVal < 0) newVal = 0;
            else if (newVal > 16383) newVal = 16383;
            
            m = MidiMessage::pitchWheel (m.getChannel(), newVal);
        }
        
        newBuff.addEvent (m, time);
    }
    
    // Adds gesture's initial midi message
    newBuff.addEvent (MidiMessage::pitchWheel (channel, midiValue), 1);
    plumeBuffer.addEvent (MidiMessage::pitchWheel (channel, midiValue), 1);
    
    midiMessages.swapWith (newBuff);
}

void Gesture::addEventAndMergeCCToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer,
												 int midiValue, int ccValue, int channel)
{
    MidiBuffer newBuff;
    int time;
    MidiMessage m;
    
    for (MidiBuffer::Iterator i (midiMessages); i.getNextEvent (m, time);)
    {
        if (m.isController() && m.getControllerValue() == ccValue) // checks if right event
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
    plumeBuffer.addEvent (MidiMessage::controllerEvent (channel, ccValue, midiValue), 1);
    
    midiMessages.swapWith (newBuff);
}

void Gesture::addEventAndMergeAfterTouchToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer,
										                 int midiValue, int channel)
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
    plumeBuffer.addEvent (MidiMessage::channelPressureChange (channel, midiValue), 1);
    
    midiMessages.swapWith (newBuff);
}

//==============================================================================
// Getter to the midiValue taking the midi mode range into account
int Gesture::getMidiValue()
{
    return currentMidi;
}

int Gesture::getRescaledMidiValue()
{
    if (midiType == Gesture::pitch)
    {
        return map (getMidiValue(), 0, 16383, map (midiLow, 0.0f, 1.0f, 0, 16383),
                                      map (midiHigh,   0.0f, 1.0f, 0, 16383));
    }
       
    else
    {
        return map (getMidiValue(), 0, 127, map (midiLow, 0.0f, 1.0f, 0, 127),
                                      map (midiHigh,   0.0f, 1.0f, 0, 127));
    }
}

//==============================================================================
// Getters and setters to the value and range of the gesture. Used by the display among others.

void Gesture::setGestureValue (float newVal)
{
    ScopedLock valuelock (gestureValueLock);

	if (isActive())
    {
        const int roundedNew = roundToInt (range.convertTo0to1 (newVal) * 500);
        const int roundedLast = roundToInt (lastValue * 500);

        if (roundedNew != roundedLast)
        {
            value = range.convertTo0to1 (newVal);
            lastValue = range.convertTo0to1 (newVal);
        }
    }
}

float Gesture::getGestureValue() const
{
	return range.convertFrom0to1 (value);
}

std::atomic<float>& Gesture::getValueReference()
{
	return value;
}

NormalisableRange<float> Gesture::getRangeReference()
{
    return range.getRange();
}

//==============================================================================
// Getters and setters for various parameters

void Gesture::setActive (bool shouldBeOn)
{
    on = shouldBeOn;
}

bool Gesture::isActive() const
{
    return on;
}

void Gesture::setMapped (bool shouldBeMapped)
{
    mapped = shouldBeMapped;
}

void Gesture::setGeneratesMidi (bool shouldGenerateMidi)
{
    midiOnParameterOff = shouldGenerateMidi;
}

void Gesture::setCc (int ccValue)
{
    cc = ccValue;
}

int Gesture::getCc() const
{
    return cc;
}

void Gesture::setMidiReverse (bool shouldBeReversed)
{
    midiReverse = shouldBeReversed;
}

bool Gesture::getMidiReverse() const
{
    return midiReverse;
}

bool Gesture::isMapped() const
{
    return mapped;
}

bool Gesture::generatesMidi() const
{
    return midiOnParameterOff;
}

void Gesture::setMidiLow (float newValue, bool checkOtherValue)
{
    if (newValue > midiHigh && checkOtherValue) newValue = midiHigh;
    else if (newValue > 1.0f) newValue = 1.0f;
    else if (newValue < 0.0f) newValue = 0.0f;
    
    midiLow = newValue;
}

void Gesture::setMidiHigh (float newValue, bool checkOtherValue)
{
    if (newValue < midiLow && checkOtherValue)  newValue = midiLow;
    else if (newValue > 1.0f) newValue = 1.0f;
    else if (newValue < 0.0f) newValue = 0.0f;
    
    midiHigh = newValue;
}

float Gesture::getMidiLow() const
{
    return midiLow;
}

float Gesture::getMidiHigh() const
{
    return midiHigh;
}

String Gesture::getName() const
{
    return name;
}

void Gesture::setName (String nameToSet)
{
    if (!nameToSet.trimEnd().isEmpty())
    {
        name = nameToSet.trimEnd();
    }
}

String Gesture::getDescription() const
{
    return description;
}

void Gesture::setDescription (String descrptionToSet)
{
    description = descrptionToSet;
}

String Gesture::getTypeString (bool capitalized) const
{
    return Gesture::getTypeString (type, capitalized);
}

String Gesture::getTypeString (int gestureType, bool capitalized)
{
    switch (gestureType)
    {
        case (int) Gesture::vibrato:
            return capitalized ? "Vibrato" : "vibrato";
            break;
            
        case (int) Gesture::pitchBend:
            return capitalized ? "Pitch Bend" : "pitchBend";
            break;
            
        case (int) Gesture::tilt:
            return capitalized ? "Tilt" : "tilt";
            break;
            
        case (int) Gesture::wave:
            return capitalized ? "Wave" : "wave";
            break;
            
        case (int) Gesture::roll:
            return capitalized ? "Roll" : "roll";
            break;
        
        default:
            return "unknown";
    }
}

String Gesture::getGestureTypeDescription() const
{
    return Gesture::getGestureTypeDescription (type);
}

String Gesture::getGestureTypeDescription (int gestureType)
{
    switch (gestureType)
    {
        case (int) Gesture::vibrato:
            return "Quickly move your finger back and forth "
                   "to change the pitch in a sine shape.\n";
            break;
            
        case (int) Gesture::pitchBend:
            return "Lean your hand to either side "
                   "to change the pitch of your note\n";
            break;
            
        case (int) Gesture::tilt:
            return "Tilt your hand upwards or downwards to modulate your sound.\n";
            break;
            
        case (int) Gesture::wave:
            return "TBD.\n";
            break;
            
        case (int) Gesture::roll:
            return "Rotate your hand sideways to modulate your sound.\n";
            break;
        
        default:
            return "-";
    }
}

Colour Gesture::getHighlightColour() const
{
	return getHighlightColour (type, isActive());
}

Colour Gesture::getHighlightColour (bool gestureIsActive) const
{
    return getHighlightColour (type, gestureIsActive);
}

Colour Gesture::getHighlightColour (int gestureType, bool gestureIsActive)
{
    switch (gestureType)
    {
        case (int) Gesture::tilt:      return getPlumeColour (tiltHighlight).withAlpha (gestureIsActive ? 1.0f : 0.5f);
        case (int) Gesture::roll:      return getPlumeColour (rollHighlight).withAlpha (gestureIsActive ? 1.0f : 0.5f);
        case (int) Gesture::wave:      return getPlumeColour (waveHighlight).withAlpha (gestureIsActive ? 1.0f : 0.5f);
        case (int) Gesture::vibrato:   return getPlumeColour (vibratoHighlight).withAlpha (gestureIsActive ? 1.0f : 0.5f);
        case (int) Gesture::pitchBend: return getPlumeColour (pitchBendHighlight).withAlpha (gestureIsActive ? 1.0f : 0.5f);
        
        default: return Colour (0xffffffff);
    }
}

bool Gesture::affectsPitch()
{
    // vibrato/pitchBend or any gesture with a pitch midi mode return true
    if (/*type == Gesture::vibrato || type == Gesture::pitchBend ||*/
        (generatesMidi() && midiType == Gesture::pitch))
    {
        return true;
    }
    
    return false;
}

//==============================================================================
// Parameter related methods

void Gesture::addParameter (AudioProcessorParameter& param, 
                            AudioProcessorValueTreeState& stateRef,
                            Range<float> r, bool rev)
{
    PLUME::log::writeToLog ("Gesture " + name + " (Id " + String (id) +
                            ") : Adding parameter " + param.getName (50),
                            PLUME::log::gesture);

    ScopedLock paramlock (parameterArrayLock);
    
    if (parameterArray.size() < PLUME::MAX_PARAMETER)
    {
        const int paramId = findFirstUnusedParameter();
        jassert (paramId != -1);

        parameterArray.add ( new MappedParameter (param, stateRef, r, id, paramId, rev));
        mapped = true;
    }
    
    sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
}

/*void Gesture::addParameterAtId (AudioProcessorParameter& param,
                                AudioProcessorValueTreeState& stateRef,
                                const int paramId,
                                Range<float> r, bool rev)
{
    PLUME::log::writeToLog ("Gesture " + name + " (Id " + String (id) +
                            ") : Adding parameter " + param.getName (50),
                            PLUME::log::gesture);

    ScopedLock paramlock (parameterArrayLock);
    
    if (parameterArray.size() < PLUME::MAX_PARAMETER)
    {
        const int paramId = findFirstUnusedParameter();
        jassert (paramId != -1);

        parameterArray.add ( new MappedParameter (param, stateRef, r, id, paramId, rev));
        mapped = true;
    }
    
    sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
}*/

void Gesture::deleteParameter (int paramId)
{
    PLUME::log::writeToLog ("Gesture " + name + " (Id " + String (id) + ") : removing parameter " + parameterArray[paramId]->wrappedParameter.getName (50),
                            PLUME::log::gesture);

    ScopedLock paramlock (parameterArrayLock);
    
    parameterArray.remove (paramId);
    
    if (parameterArray.isEmpty()) mapped = false;
    
    sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
}

void Gesture::replaceParameter (int paramId,
                                AudioProcessorParameter& param,
                                AudioProcessorValueTreeState& stateRef,
                                Range<float> r, bool rev)
{
    PLUME::log::writeToLog ("Gesture " + name + " (Id " + String (id) + ") : replacing parameter " + parameterArray[paramId]->wrappedParameter.getName (50) + " with " + param.getName (50),
                            PLUME::log::gesture);

    ScopedLock paramlock (parameterArrayLock);
    
    parameterArray.set (paramId, new MappedParameter (param, stateRef, r, id, paramId, rev));
    
    sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
}

void Gesture::clearAllParameters (bool sendNotification)
{
    ScopedLock paramlock (parameterArrayLock);
    
	mapped = false;
    parameterArray.clear();
    if (sendNotification) sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
}

int Gesture::getNumParameters() const
{
    return parameterArray.size();
}

/**
 *  \brief Gets a reference to the parameter array.
 */
OwnedArray<Gesture::MappedParameter>& Gesture::getParameterArray()
{
    return parameterArray;
}

bool Gesture::parameterIsMapped (int parameterId)
{
    ScopedLock paramlock (parameterArrayLock);
    
    for (auto* param : parameterArray)
    {
        if (param->wrappedParameter.getParameterIndex() == parameterId) return true;
    }
    
    return false;
}

void Gesture::swapParametersWithOtherGesture (Gesture& other)
{
    parameterArray.swapWith (other.getParameterArray());
    mapped = !parameterArray.isEmpty();

    sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
}

void Gesture::updateMappedParameters()
{
    if (isActive() && shouldUpdateParameters())
    {
        // Goes through the parameterArray to update each value
        for (auto* param : parameterArray)
        {
            const float newValue = computeMappedParameterValue (param->range, param->reversed);

            if (newValue != param->lastComputedValue)
            {
                param->lastComputedValue = newValue;
                param->wrappedParameter.setValueNotifyingHost (newValue);
            }
        }
    }   
}

bool Gesture::shouldUpdateParameters()
{
    return true;
}

void Gesture::setParametersShouldBeUpdated (const bool shouldBeUpdated)
{
    parametersWereChangedSinceLastUpdate = shouldBeUpdated;
}

bool Gesture::parametersShouldBeUpdated()
{
    return parametersWereChangedSinceLastUpdate;
}

int Gesture::normalizeMidi (float val, float minVal, float maxVal, bool is14BitMidi, bool reversed)
{
    if (minVal == maxVal && val == minVal) return 0;

    if (reversed)
    {
        val = maxVal - (val - minVal);
    }
    
    return Gesture::map (val, minVal, maxVal, 0, is14BitMidi ? 16383 : 127);
}

int Gesture::map (float val, float minVal, float maxVal, int minNew, int maxNew)
{
    if (minVal == maxVal && val == minVal) return minNew;

    if (val < minVal) return minNew;
    if (val > maxVal) return maxNew;

    return (minNew + int ((maxNew - minNew)*(val - minVal)/(maxVal-minVal)));
}

int Gesture::map (int val, int minVal, int maxVal, int minNew, int maxNew)
{
    if (minVal == maxVal && val == minVal) return minNew;
    else if (minVal == minNew && maxVal == maxNew) return val;

    if (val < minVal) return minNew;
    if (val > maxVal) return maxNew;

    return (minNew + (maxNew - minNew)*(val - minVal)/(maxVal-minVal));
}

float Gesture::mapParameter (float val, float minVal, float maxVal, Range<float> paramRange, bool reversed)
{   
    // Prevents dividing by 0
    if (minVal == maxVal && val == minVal) return paramRange.getStart();
    
    // Forces the interval to [0.0f 1.0f]
    if (paramRange.getStart() < 0.0f) paramRange.setStart (0.0f);
    if (paramRange.getEnd() > 1.0f)   paramRange.setEnd (1.0f);
    
    // Checks out of bounds values
    if (val < minVal) return reversed ? paramRange.getEnd() : paramRange.getStart();
    if (val > maxVal) return reversed ? paramRange.getStart() : paramRange.getEnd();
    
    // normal case
    if (!reversed) return (paramRange.getStart() + paramRange.getLength()*(val - minVal)/(maxVal - minVal));
    // reversed parameter
    else           return (paramRange.getStart() + paramRange.getLength()*(maxVal - val)/(maxVal - minVal));
}

void Gesture::addRightMidiSignalToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer, int channel)
{
	addRightMidiSignalToBuffer (midiMessages, plumeBuffer, channel, getMidiValue());
}

void Gesture::addRightMidiSignalToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer, int channel, int midiValue)
{
    if (!generatesMidi()) return; //Does nothing if not in default midi mode

    int newMidi;
    
    if (midiValue != lastMidi) // Prevents to send the same message twice in a row
    {
        // Assigns the right midi value depending on the signal and
        // the midiRange parameter, then adds message to the buffers
        switch (midiType)
        {
            case (Gesture::pitch):
                newMidi = map (midiValue, 0, 16383,
                                  map (midiLow, 0.0f, 1.0f, 0, 16383),
                                  map (midiHigh,   0.0f, 1.0f, 0, 16383));
                                  
                addEventAndMergePitchToBuffer (midiMessages, plumeBuffer, newMidi, channel);
                break;
            
            case (Gesture::controlChange):
                newMidi = map (midiValue, 0, 127,
                                  map (midiLow, 0.0f, 1.0f, 0, 127),
                                  map (midiHigh,   0.0f, 1.0f, 0, 127));
                                  
                addEventAndMergeCCToBuffer (midiMessages, plumeBuffer, newMidi, getCc(), channel);
                break;
            
            case (Gesture::afterTouch):
                newMidi = map (midiValue, 0, 127,
                                  map (midiLow, 0.0f, 1.0f, 0, 127),
                                  map (midiHigh,   0.0f, 1.0f, 0, 127));
                                  
                addEventAndMergeAfterTouchToBuffer (midiMessages, plumeBuffer, newMidi, channel);
                break;
            
            default:
                break;
        }

        lastMidi = midiValue;
    }
}

int Gesture::findFirstUnusedParameter()
{
    int unusedId = 0;

    for (auto* parameter : plumeParametersRef.processor.getParameters())
    {
        if (parameter->getName (100) == "Unmapped Parameter") return unusedId;
        unusedId++;
    }

    return -1;
}

Gesture::MappedParameter::MappedParameter (AudioProcessorParameter& p, AudioProcessorValueTreeState& stateRef, Range<float> pRange, const int gestId, const int paramId, bool rev)
    : wrappedParameter(p),
      plumeParameter (*(stateRef.getParameter ("Parameter_" + String (paramId)))),
      parametersRef(stateRef), range(pRange), reversed (rev), gestureId (gestId), parameterId (paramId)
{
    wrappedParameter.addListener (this);

    if (auto* plumeParamTempPointer = dynamic_cast<PlumeParameter<juce::AudioParameterFloat>*> (&plumeParameter))
    {
        plumeParamTempPointer->setName (wrappedParameter.getName (100));
    }

    parametersRef.addParameterListener ("Parameter_" + String (paramId), this);
}

Gesture::MappedParameter::MappedParameter (const MappedParameter& other)
    : wrappedParameter (other.wrappedParameter), plumeParameter (other.plumeParameter), parametersRef(other.parametersRef), range (other.range), reversed (other.reversed), gestureId (other.gestureId), parameterId (other.parameterId)
{
    wrappedParameter.addListener (this);
    parametersRef.addParameterListener ("Parameter_" + String (parameterId), this);
}

Gesture::MappedParameter::~MappedParameter()
{
    wrappedParameter.removeListener (this);
    parametersRef.removeParameterListener ("Parameter_" + String (parameterId), this);

    if (auto* plumeParamTempPointer = dynamic_cast<PlumeParameter<juce::AudioParameterFloat>*> (&plumeParameter))
    {
        plumeParamTempPointer->setName ("Unmapped Parameter");
    }
}
        
void Gesture::MappedParameter::parameterChanged (const String& parameterID, float newValue)
{
    DBG ("Param changed (pC) : " << parameterID);

    // Change wrappedParam
    if (wrappedParameter.getValue() != plumeParameter.getValue())
    {
        wrappedParameter.setValue (newValue);
    }
}
        
void Gesture::MappedParameter::parameterValueChanged (int parameterIndex, float newValue)
{
    DBG ("Param changed (pVC) : " << wrappedParameter.getName (50) << " (" << parameterIndex << ")");

    // Change plumeParam
    if (wrappedParameter.getValue() != plumeParameter.getValue())
    {
        plumeParameter.setValueNotifyingHost (newValue);
    }
}

void Gesture::MappedParameter::parameterGestureChanged (int, bool)
{
}

AudioProcessorParameter& Gesture::MappedParameter::findRightPlumeParameter (AudioProcessorValueTreeState& stateRef, const int gestId)
{
    const String s = "Gest_" + String (gestId) + "_param_0"; // TODO actual search

    return *(stateRef.getParameter (s));
}