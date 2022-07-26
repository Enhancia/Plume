/*
==============================================================================

Gesture.cpp
Created: 10 Apr 2019 3:06:34pm
Author:  Alex

==============================================================================
*/

#include "Gesture.h"

Gesture::Gesture (String gestName, int gestType, int gestId, const NormalisableRange<float> maxRange,
         		  AudioProcessorValueTreeState& plumeParameters, const String valueId, String gestureDescription, const int midiParameterId,
         		  float defaultValue, int defaultCc, Range<float> defaultMidiRange)
        
         		  : type (gestType), name (gestName), id (gestId), range (maxRange), description (gestureDescription), plumeParametersRef (plumeParameters)
{

    mapped = false;
    midiParameter.reset (new MidiParameter (plumeParametersRef, *this));
    if (midiParameterId > 0) setMidiParameter (midiParameterId);
    
    currentMidi = -1;
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
    
    for (const MidiMessageMetadata metadata : midiMessages)
    {
        MidiMessage m = metadata.getMessage();
        
        if (m.isPitchWheel()) // checks for pitch wheel events
        {   
            int newVal = m.getPitchWheelValue() - 8192 + midiValue;
            if (newVal < 0) newVal = 0;
            else if (newVal > 16383) newVal = 16383;
            
            m = MidiMessage::pitchWheel (m.getChannel(), newVal);
        }
        
        newBuff.addEvent (m, metadata.samplePosition);
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
    
    for (const MidiMessageMetadata metadata : midiMessages)
    {
        MidiMessage m = metadata.getMessage();

        if (m.isController() && m.getControllerValue() == ccValue) // checks if right event
        {
            // Creates a cc message with the new value
            int newVal = m.getControllerValue() + midiValue;
            if (newVal > 127) newVal = 127;
            
            m = MidiMessage::controllerEvent (m.getChannel(), ccValue, newVal);
        }
        
        newBuff.addEvent (m, metadata.samplePosition);
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
    
    for (const MidiMessageMetadata metadata : midiMessages)
    {
        MidiMessage m = metadata.getMessage();

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
        
        newBuff.addEvent (m, metadata.samplePosition);
    }
    
    // Adds gesture's initial cc message
    newBuff.addEvent (MidiMessage::channelPressureChange (channel, midiValue), 1);
    plumeBuffer.addEvent (MidiMessage::channelPressureChange (channel, midiValue), 1);
    
    midiMessages.swapWith (newBuff);
}

//==============================================================================
void Gesture::updateMidiValue()
{
    const int newComputedMidi = getRescaledMidiValue (computeMidiValue());
    
    if (newComputedMidi != computedMidi)
    {
        computedMidi = newComputedMidi;
        currentMidi = newComputedMidi;

        if (midiOnParameterOff && midiParameter && midiParameter->plumeParameterPtr)
        {
            if (!midiParameterIsPerformingGesture && currentMidi != lastMidi)
            {
                midiParameterIsPerformingGesture = true;
                midiParameter->plumeParameterPtr->beginChangeGesture();
            }
            else if (midiParameterIsPerformingGesture && currentMidi == lastMidi)
            {
                midiParameterIsPerformingGesture = false;
                midiParameter->plumeParameterPtr->endChangeGesture();
            }

            if (midiParameterIsPerformingGesture)
            {
                const float normalizedMidi = NormalisableRange<float> (0.0f, (midiType == MidiType::pitch) ? 16383.0f : 127.0f, 1.0f)
                                                .convertTo0to1 (static_cast<float> (currentMidi));

                midiParameter->plumeParameterPtr->setValueNotifyingHost (normalizedMidi);
            }
        }

    }
}

int Gesture::getMidiValue()
{
    return currentMidi;
}

void Gesture::setMidiValue (const int newMidiValue)
{
    currentMidi = newMidiValue;
}

int Gesture::getRescaledMidiValue (const int midiValueToUse)
{
    if (midiType == Gesture::pitch)
    {
        return map (midiValueToUse, 0, 16383, map (midiLow, 0.0f, 1.0f, 0, 16383),
                                      map (midiHigh,   0.0f, 1.0f, 0, 16383));
    }
       
    else
    {
        return map (midiValueToUse, 0, 127, map (midiLow, 0.0f, 1.0f, 0, 127),
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

    handleModeChange();
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
    PLUME::log::writeToLog ("Gesture " + name + " (Id " + String (id) + ") : Adding parameter " + param.getName (50),
                            PLUME::log::LogCategory::gesture);

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

void Gesture::addParameterAtId (AudioProcessorParameter& param,
                                const int plumeIdToAddParameterTo,
                                AudioProcessorValueTreeState& stateRef,
                                Range<float> r, bool rev)
{
    PLUME::log::writeToLog ("Gesture " + name + " (Id " + String (id) +
                            ") : Adding parameter " + param.getName (50) +
                            " at ID " + String (plumeIdToAddParameterTo),
                            PLUME::log::LogCategory::gesture);

    ScopedLock paramlock (parameterArrayLock);
    
    // Parameter is unused
    if (stateRef.processor.getParameters()[plumeIdToAddParameterTo]->getName (50) == PLUME::param::defaultParameterName)
    {
        parameterArray.add (new MappedParameter (param, stateRef, r, id, plumeIdToAddParameterTo, rev));
        mapped = true;
        sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
    }
    else
    {
        // If you're creating a gesture parameter at a specific plume parameter id,
        // the id must not be used by another parameter...
        jassertfalse;

        // Adds parameter at first unused location instead
        addParameter (param, stateRef, r, rev);
    }
}

void Gesture::deleteParameter (int paramId)
{
    PLUME::log::writeToLog ("Gesture " + name + " (Id " + String (id) + ") : removing parameter " + parameterArray[paramId]->plumeParameter.getName (50),
                            PLUME::log::LogCategory::gesture);

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
    PLUME::log::writeToLog ("Gesture " + name + " (Id " + String (id) + ") : replacing parameter " + parameterArray[paramId]->plumeParameter.getName (50) + " with " + param.getName (50),
                            PLUME::log::LogCategory::gesture);

    ScopedLock paramlock (parameterArrayLock);
    
    parameterArray.remove (paramId);
    parameterArray.insert (paramId, new MappedParameter (param, stateRef, r, id, paramId, rev));
    
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

bool Gesture::plumeParameterIsUsedByThisGesture (const int parameterId)
{
    ScopedLock paramlock (parameterArrayLock);
    
    for (auto* param : parameterArray)
    {
        if (param->plumeParameter.getParameterIndex() == parameterId) return true;
    }
    
    return false;
}

void Gesture::swapParametersWithOtherGesture (Gesture& other)
{
    parameterArray.swapWith (other.getParameterArray());
    mapped = !parameterArray.isEmpty();

    if (midiParameter->plumeParameterPtr || other.getMidiParameterReference().plumeParameterPtr)
    {
        const int thisParamId = midiParameter->plumeParameterPtr ? midiParameter->parameterId : -1;
        const int otherParamId = other.getMidiParameterReference().plumeParameterPtr ? other.getMidiParameterReference().parameterId : -1;

        midiParameter->resetParameter();
        other.getMidiParameterReference().resetParameter();

        if (thisParamId != -1)  setMidiParameter (thisParamId, true);
        if (otherParamId != -1)  other.setMidiParameter (otherParamId, true);
    }

    sendChangeMessage(); // Alerts the gesture's mapperComponent to update it's Ui
}

Gesture::MidiParameter Gesture::getMidiParameter()
{
    // Make sure your midi parameter is initialized if you wanna get a reference for it...
    jassert (midiParameter);

    return *midiParameter.get();
}

Gesture::MidiParameter& Gesture::getMidiParameterReference()
{
    // Make sure your midi parameter is initialized if you wanna get a reference for it...
    jassert (midiParameter);

    return *midiParameter.get();
}

void Gesture::setMidiParameter (int paramIdToUse, const bool cleanFormerParameter)
{
    if (paramIdToUse == -1)
        paramIdToUse = findFirstUnusedParameter();

    if (RangedAudioParameter* plumeParameterPtrTemp = plumeParametersRef.getParameter ("Parameter_" + String (paramIdToUse)))
    {
        // Make sure you're not overriding an existing parameter to avoid unhandled behaviour and bad access errors...
        jassert (plumeParameterPtrTemp->getParameterID() != PLUME::param::defaultParameterName);

        // Cleans parameter (if it was already set)
        if (cleanFormerParameter && midiParameter->parameterId != -1)
        {
            removeMidiParameter();
        }

        // Find id to put parameter in
        midiParameter->parameterId = paramIdToUse;

        //add listener for parameter && store parameter ptr
        midiParameter->plumeParameterPtr = plumeParameterPtrTemp;
        plumeParametersRef.addParameterListener (plumeParameterPtrTemp->getParameterID(), midiParameter.get());

        // change parameter name to midi name
        if (auto* plumeParamTempPointerRecast = dynamic_cast<PlumeParameter<juce::AudioParameterFloat>*> (plumeParameterPtrTemp))
        {
            plumeParamTempPointerRecast->setName (name + " MIDI");
        }
    }
}

void Gesture::removeMidiParameter()
{
    if (midiParameter->plumeParameterPtr)
    {
        midiParameter->resetParameter();
    }
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
                // Send begin change gesture if 1st time moved
                if (!param->isBeingChanged)
                {
                    param->isBeingChanged = true;
                    param->plumeParameter.beginChangeGesture();
                }

                param->lastComputedValue = newValue;
                param->displayValue = newValue;
                param->plumeParameter.setValueNotifyingHost (newValue);
                param->wrappedParameter.setValueNotifyingHost (newValue);
            }
            else if (param->isBeingChanged) // Gesture wasnt changed. sens end change gesture if necessary
            {
                param->isBeingChanged = false;
                param->plumeParameter.endChangeGesture();
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
    if (!generatesMidi()) return; //Does nothing if not in default midi mode
    
    if ((currentMidi != lastMidi) == (currentMidi != -1)) // Prevents to send the same message twice in a row
    {
        // Assigns the right midi value depending on the signal and
        // the midiRange parameter, then adds message to the buffers
        switch (midiType)
        {
            case (Gesture::pitch):
                addEventAndMergePitchToBuffer (midiMessages, plumeBuffer, currentMidi, channel);
                break;
            
            case (Gesture::controlChange):
                addEventAndMergeCCToBuffer (midiMessages, plumeBuffer, currentMidi, getCc(), channel);
                break;
            
            case (Gesture::afterTouch):
                addEventAndMergeAfterTouchToBuffer (midiMessages, plumeBuffer, currentMidi, channel);
                break;
            
            default:
                break;
        }

        lastMidi = currentMidi;
    }
}

void Gesture::handleModeChange()
{
    if (midiOnParameterOff) // Just switched to midi mode
    {
        if (midiParameter->parameterId == -1) //parameter is not initialized yet
        {
            setMidiParameter();
        }
    }
    else // Just switched to MAP mode
    {        
    }
}

int Gesture::findFirstUnusedParameter()
{
    int unusedId = 0;

    for (auto* parameter : plumeParametersRef.processor.getParameters())
    {
        if (parameter->getName (100) == PLUME::param::defaultParameterName) return unusedId;
        unusedId++;
    }

    return -1;
}

int Gesture::findFirstUnusedParameterForMidiParam()
{
    int unusedId = 0;

    for (auto* parameter : plumeParametersRef.processor.getParameters())
    {
        if (plumeParameterIsUsedByThisGesture (parameter->getParameterIndex()) ||
            parameter->getName (100) == PLUME::param::defaultParameterName)
        {
            return unusedId;
        }
        
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
    if (isBeingChanged) plumeParameter.endChangeGesture(); // If gesture was currently moving, sends end change message before deletion

    wrappedParameter.removeListener (this);
    parametersRef.removeParameterListener ("Parameter_" + String (parameterId), this);

    if (auto* plumeParamTempPointer = dynamic_cast<PlumeParameter<juce::AudioParameterFloat>*> (&plumeParameter))
    {
        plumeParamTempPointer->setName (PLUME::param::defaultParameterName);
    }
}
        
void Gesture::MappedParameter::parameterChanged (const String&, float newValue)
{
    // Change wrappedParam
    if (wrappedParameter.getValue() != plumeParameter.getValue())
    {
        // Change wrapped plugins param
        wrappedParameter.setValue (newValue);

        // Change value for interface display
        displayValue = newValue;
    }
}
        
void Gesture::MappedParameter::parameterValueChanged (int, float newValue)
{
    // Change plumeParam
    if (wrappedParameter.getValue() != plumeParameter.getValue())
    {
        // Change wrapped plugins param
        plumeParameter.setValueNotifyingHost (newValue);
        
        // Change value for interface display
        displayValue = newValue;
    }
}

void Gesture::MappedParameter::parameterGestureChanged (int, bool gestureIsStarting)
{
    // Change plumeParam
    if (isBeingChanged != gestureIsStarting)
    {
        isBeingChanged = gestureIsStarting;

        // Send plume param notification
        if (isBeingChanged) plumeParameter.beginChangeGesture();
        else                plumeParameter.endChangeGesture();
    }
}

Gesture::MidiParameter::MidiParameter (AudioProcessorValueTreeState& stateRef, Gesture& currentGesture) : parametersRef (stateRef), gestureRef (currentGesture)
{
}

Gesture::MidiParameter::MidiParameter (const MidiParameter& other)
            : parametersRef (other.parametersRef), gestureRef (other.gestureRef),
              parameterId (other.parameterId), isBeingChanged (other.isBeingChanged),
              plumeParameterPtr (other.plumeParameterPtr)
{
}

Gesture::MidiParameter::~MidiParameter()
{
    resetParameter();
}

Gesture::MidiParameter& Gesture::MidiParameter::operator= (const Gesture::MidiParameter& other) noexcept
{
    plumeParameterPtr = other.plumeParameterPtr;
    parameterId = other.parameterId;
    
    return *this;
}
        
void Gesture::MidiParameter::parameterChanged (const String&, float newValue)
{
    const int denormalizedNewValue = static_cast<int> (NormalisableRange<float> (0.0f, (gestureRef.midiType == Gesture::MidiType::pitch) ? 16383.0f : 127.0f, 1.0f).convertFrom0to1 (newValue));
    
    // Change plumeParam
    if (denormalizedNewValue != gestureRef.getMidiValue())
    {
        gestureRef.setMidiValue (denormalizedNewValue);
    }
}

void Gesture::MidiParameter::resetParameter()
{
    if (plumeParameterPtr)
    {
        if (isBeingChanged) plumeParameterPtr->endChangeGesture(); // If gesture was currently moving, sends end change message before deletion

        parametersRef.removeParameterListener ("Parameter_" + String (parameterId), this);
        
        if (auto* plumeParamTempPointer = dynamic_cast<PlumeParameter<juce::AudioParameterFloat>*> (plumeParameterPtr))
        {
            plumeParamTempPointer->setName (PLUME::param::defaultParameterName);
        }
    }
}