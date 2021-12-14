/*
  ==============================================================================

    GestureArray.cpp
    Created: 9 Jul 2018 2:18:55pm
    Author:  Alex

  ==============================================================================
*/

#include "GestureArray.h"

GestureArray::GestureArray(DataReader& reader, AudioProcessorValueTreeState& params, bool& lastArmValue)
    : dataReader (reader), parameters (params), armValue (lastArmValue)
{
    initializeGestures();
    cancelMapMode();
}

GestureArray::~GestureArray()
{
    gestures.clear();
}

//==============================================================================
void GestureArray::initializeGestures()
{
    // Uncomment those lines to add gestures by default when opening Plume

    //addGesture ("Vibrato", Gesture::vibrato, 0);
    //addGesture ("Pitch Bend", Gesture::pitchBend, 1);
    //addGesture ("Tilt", Gesture::tilt, 2);
    //addGesture ("Roll", Gesture::roll, 3);
}
//==============================================================================
void GestureArray::process (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    // adds midi to buffer and updates parameters
    addGestureMidiToBuffer (midiMessages, plumeBuffer);
    updateAllMappedParameters();
}

void GestureArray::addGestureMidiToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    ScopedLock gestlock (gestureArrayLock);
    
    if (shouldMergePitch)
    {
        // Adds non-pitch midi
        for (auto* g : gestures)
        {
            if (g->generatesMidi() && !g->affectsPitch())
            {
                g->updateMidiValue();
                g->addGestureMidi (midiMessages, plumeBuffer);
            }
        }
        
        // Adds pitch midi
        addMergedPitchMessage (midiMessages, plumeBuffer);
    }
    
    else
    {
        // Adds all midi
        for (auto* g : gestures)
        {
            if (g->generatesMidi())
            {
                g->updateMidiValue();
                g->addGestureMidi (midiMessages, plumeBuffer);
            }
        }
    }
}

void GestureArray::updateAllMappedParameters()
{
    ScopedLock gestlock (gestureArrayLock);
    
    // calls updateMappedParameters for every gestures that isn't in
    // mapMode (to prevent the parameter from changing) and that is mapped
    for (auto* g : gestures)
    {
        if (mapModeOn == false && g->isMapped() && g->generatesMidi() == false)
        {
            g->updateMappedParameters();
        }
    }
}

//==============================================================================
void GestureArray::updateAllValues()
{
    Array<float> rawData;
    int armValue = parameters.getParameter ("track_arm")
                             ->convertFrom0to1 (parameters.getParameter ("track_arm")
                                                          ->getValue());
    
    if (armValue)
    {
        // Gets the rawData in the array, and calls updateValue for each gesture
        if (dataReader.getRawDataAsFloatArray (rawData))
        {
            ScopedLock gestlock (gestureArrayLock);
        
            for (auto* g : gestures)
            {
                g->updateValue (rawData);
            }
        }
    }
}

//==============================================================================
Gesture* GestureArray::getGesture (const String nameToSearch)
{
    ScopedLock gestlock (gestureArrayLock);
    
    // Browses every gesture to compare their name with nameToSearch
    for (auto* g : gestures)
    {
        if (g->getName().compare (nameToSearch) == 0)
        {
            return g;
        }
    }

    return nullptr;
}

Gesture* GestureArray::getGesture (const int idToSearch)
{
    if (idToSearch >= PLUME::NUM_GEST || idToSearch < 0)
    {
        DBG ("Gesture n°" << idToSearch << " cannot exist. \nNumber of gestures: " << gestures.size());
        return nullptr;
    }

    {
        ScopedLock gestlock (gestureArrayLock);

        for (auto* gesture : gestures)
        {
            if (gesture->id == idToSearch)
            {
                return gesture;
            }
        }
    }

    return nullptr;
}

OwnedArray<Gesture>& GestureArray::getArray()
{
    return gestures;
}

int GestureArray::size()
{
    return gestures.size();
}

bool GestureArray::parameterIsMapped (int parameterId)
{
    ScopedLock gestlock (gestureArrayLock);
    
    for (auto* g : gestures)
    {
        if (g->parameterIsMapped (parameterId)) return true;
    }
    
    return false;
}

bool GestureArray::isCCInUse (const int controllerNumber)
{
    ScopedLock gestlock (gestureArrayLock);
    
    for (auto* g : gestures)
    {
        if (g->generatesMidi() && g->midiType == Gesture::controlChange
                               && g->getCc() == controllerNumber)
            return true;
    }
    
    return false;
}

//==============================================================================
void GestureArray::changeListenerCallback(ChangeBroadcaster*)
{
    updateAllValues();
}

//==============================================================================
void GestureArray::addGesture (String gestureName, int gestureType, int gestureId)
{
    if (!isIdAvailable (gestureId))
    {
        jassertfalse;
        return;
    }

    PLUME::log::writeToLog ("New Gesture : " + Gesture::getTypeString (gestureType, true) + " in Id " + String (gestureId), PLUME::log::gesture);

    switch (gestureType)
    {
        case Gesture::vibrato:
            gestures.add (new Vibrato (gestureName, gestureId, parameters));
            break;
        
        case Gesture::pitchBend:
            gestures.add (new PitchBend (gestureName, gestureId, parameters));
            break;
            
        case Gesture::tilt:
            gestures.add (new Tilt (gestureName, gestureId, parameters));
            break;
        /*    
        case Gesture::wave:
            gestures.add (new Wave (gestureName, gestureId, parameters));
            break;
        */
        case Gesture::roll:
            gestures.add (new Roll (gestureName, gestureId, parameters));
            break;
    }
    
    gestures.getLast()->setActive(true);
    
    checkPitchMerging();
}

void GestureArray::addGestureCopyingOther (Gesture* other, int gestureId, String gestureName)
{
    if (!isIdAvailable (gestureId))
    {
        jassertfalse;
        return;
    }

    if (gestureName.isEmpty()) gestureName = other->getName();

    switch (other->type)
    {
        case Gesture::vibrato:
            if (auto* vibrato = dynamic_cast<Vibrato*> (other))
            {
                gestures.add (new Vibrato (gestureName, gestureId, parameters,
                                           vibrato->gainDisplayRange.convertFrom0to1 (vibrato->gain.getValue()),
                                           vibrato->thresholdDisplayRange.convertFrom0to1 (vibrato->threshold.getValue()),
                                           other->getDescription()));
            }
            break;
        
        case Gesture::pitchBend:
            if (auto* pitchBend = dynamic_cast<PitchBend*> (other))
            {
                gestures.add (new PitchBend (gestureName, gestureId, parameters,
                                             pitchBend->pitchBendDisplayRange.convertFrom0to1 (pitchBend->rangeLeftLow.getValue()),
                                             pitchBend->pitchBendDisplayRange.convertFrom0to1 (pitchBend->rangeLeftHigh.getValue()),
                                             pitchBend->pitchBendDisplayRange.convertFrom0to1 (pitchBend->rangeRightLow.getValue()),
                                             pitchBend->pitchBendDisplayRange.convertFrom0to1 (pitchBend->rangeRightHigh.getValue()),
                                             other->getDescription()));
            }
            break;
            
        case Gesture::tilt:
            if (auto* tilt = dynamic_cast<Tilt*> (other))
            {
                gestures.add (new Tilt (gestureName, gestureId, parameters,
                                        tilt->tiltDisplayRange.convertFrom0to1 (tilt->rangeLow.getValue()),
                                        tilt->tiltDisplayRange.convertFrom0to1 (tilt->rangeHigh.getValue()),
                                        other->getDescription()));
            }
            break;
        /*    
        case Gesture::wave:
            if (auto* wave = dynamic_cast<Wave*> (other))
            {
                gestures.add (new Wave (gestureName, gestureId, parameters,
                                        wave->rangeLow.convertFrom0to1 (wave->rangeLow.getValue()),
                                        wave->rangeHigh.convertFrom0to1 (wave->rangeHigh.getValue()),
                                        other->getDescription()));
            }
            break;
        */
        case Gesture::roll:
            if (auto* roll = dynamic_cast<Roll*> (other))
            {
                gestures.add (new Roll (gestureName, gestureId, parameters,
                                        roll->rollDisplayRange.convertFrom0to1 (roll->rangeLow.getValue()),
                                        roll->rollDisplayRange.convertFrom0to1 (roll->rangeHigh.getValue()),
                                        other->getDescription()));
            }
            break;
    }
    
    gestures.getLast()->setActive (other->isActive());
    gestures.getLast()->setGeneratesMidi (other->generatesMidi());
    gestures.getLast()->midiType = other->midiType;
    gestures.getLast()->setCc (other->getCc());
    gestures.getLast()->setMidiLow (other->getMidiLow());
    gestures.getLast()->setMidiHigh (other->getMidiHigh());
    
    checkPitchMerging();
}

void GestureArray::removeGesture (const int gestureId)
{
    if (auto* gestureToRemove = getGesture (gestureId))
    {
        ScopedLock gestlock (gestureArrayLock);

        gestures.removeObject (gestureToRemove, true);
    }

    checkPitchMerging();
}

void GestureArray::removeGesture (const String gestureName)
{
    if (auto* gestureToRemove = getGesture (gestureName))
    {
        ScopedLock gestlock (gestureArrayLock);

        gestures.removeObject (gestureToRemove, true);
    }

    checkPitchMerging();
}

void GestureArray::addParameterToMapModeGesture (AudioProcessorParameter& param)
{
    ScopedLock gestlock (gestureArrayLock);

    // Does nothing if the parameter is already mapped to any gesture
    if (parameterIsMapped (param.getParameterIndex()))
    {
        cancelMapMode();
        sendActionMessage (PLUME::commands::mappingOverwrite);
        return;
    }
    
    
    // else adds the parameter and cancels mapMode
    for (auto* g : gestures)
    {
        if (g->mapModeOn == true)
        {
            g->addParameter(param);
            //cancelMapMode();
            return;
        }
    }
}

void GestureArray::addAndSetParameter (AudioProcessorParameter& param, int gestureId, float start = 0.0f, float end = 1.0f, bool rev = false)
{
    // Does nothing if the parameter is already mapped to any gesture
    if (parameterIsMapped (param.getParameterIndex())) return;
    
    // else adds the parameter and cancels mapMode
    if (gestureId < size())
    {
        ScopedLock gestlock (gestureArrayLock);
        gestures[gestureId]->addParameter (param, Range<float> (start, end), rev);
    }
}

void GestureArray::clearAllGestures()
{
    gestures.clear();
    shouldMergePitch = false;
}

void GestureArray::clearAllParameters()
{
    ScopedLock gestlock (gestureArrayLock);
    
    for (auto* g : gestures)
    {
        g->mapModeOn = false;
        g->clearAllParameters();
    }
}

void GestureArray::cancelMapMode()
{
    ScopedLock gestlock (gestureArrayLock);
    
    for (auto* g : gestures)
    {
        if (g->mapModeOn) g->mapModeOn = false;
    }
    mapModeOn = false;
    sendChangeMessage();
}

bool GestureArray::isIdAvailable (int idToCheck)
{
    if (idToCheck < 0 || idToCheck >= PLUME::NUM_GEST) return false;

    ScopedLock gestlock (gestureArrayLock);

    for (auto* g : gestures)
    {
        if (g->id == idToCheck)
			return false;
    }

    return true;
}

//==============================================================================

void GestureArray::moveGestureToId (int idToMoveFrom, int idToMoveTo)
{
    ScopedLock gestlock (gestureArrayLock);
    
    Gesture* gestureToMove = getGesture (idToMoveFrom);

    if (gestureToMove == nullptr || !isIdAvailable(idToMoveTo)) return;

    addGestureCopyingOther (gestureToMove, idToMoveTo);
    gestures.getLast()->swapParametersWithOtherGesture (*gestureToMove);
    removeGesture (idToMoveFrom);
    
    //gestures.getLast()->sendChangeMessage(); // Alert to update Ui
}

void GestureArray::duplicateGesture (int idToDuplicateFrom, bool prioritizeHigherId)
{
    ScopedLock gestlock (gestureArrayLock);
    
    Gesture* gestureToMove = getGesture (idToDuplicateFrom);
    int idToDuplicateTo = findClosestIdToDuplicate (idToDuplicateFrom, prioritizeHigherId);

    if (gestureToMove == nullptr || idToDuplicateTo == -1) return;

    addGestureCopyingOther (gestureToMove, idToDuplicateTo, createDuplicateName (gestureToMove->getName()));
}

int GestureArray::findClosestIdToDuplicate (int idToDuplicateFrom, bool prioritizeHigherId)
{
    int lowerAvailableId = -1;

    for (int offset=1;
        (idToDuplicateFrom + offset < PLUME::NUM_GEST || idToDuplicateFrom - offset >= 0);
        offset++)
    {
        if (isIdAvailable (idToDuplicateFrom + offset))
        {
            return idToDuplicateFrom + offset;
        }

        if (lowerAvailableId == -1 && isIdAvailable (idToDuplicateFrom - offset))
        {
            if (!prioritizeHigherId)
            {
                return idToDuplicateFrom - offset;
            }

            lowerAvailableId = idToDuplicateFrom - offset;
        }
    }

    return lowerAvailableId;
}


String GestureArray::createDuplicateName (String originalGestureName)
{
    String endChars = originalGestureName.trimEnd().fromLastOccurrenceOf (" (", false, false);

    if (endChars.getLastCharacter() == ')' && endChars.dropLastCharacters (1).containsOnly ("0123456789"))
    {
        int newNum = endChars.dropLastCharacters (1).getIntValue() + 1; 
        DBG ("New num " << newNum);

        return originalGestureName.upToLastOccurrenceOf ("(", true, false) + String(newNum) + String (")");
    }
    else
    {
        return originalGestureName + String (" (1)");
    }
}

void GestureArray::swapGestures (int firstId, int secondId)
{
    if (firstId == secondId || firstId  < 0 || firstId  >= PLUME::NUM_GEST ||
                               secondId < 0 || secondId >= PLUME::NUM_GEST)
        return;

	ScopedLock gestlock(gestureArrayLock);

    Gesture* secondGesture = gestures.removeAndReturn (gestures.indexOf (getGesture (secondId)));
    Array<float> secondGestureParameters = { parameters.getParameterAsValue (String (secondId) + PLUME::param::paramIds[0]).getValue(),
                                             parameters.getParameterAsValue (String (secondId) + PLUME::param::paramIds[1]).getValue(),
                                             parameters.getParameterAsValue (String (secondId) + PLUME::param::paramIds[2]).getValue(),
                                             parameters.getParameterAsValue (String (secondId) + PLUME::param::paramIds[3]).getValue() };

    // Replaces second gesture with first
    moveGestureToId (firstId, secondId);

    // Copies second gesture to first Id
    addGestureCopyingOther (secondGesture, firstId);
    getGesture (firstId)->swapParametersWithOtherGesture (*secondGesture);
    parameters.getParameterAsValue (String (firstId) + PLUME::param::paramIds[0]).setValue (secondGestureParameters[0]);
    parameters.getParameterAsValue (String (firstId) + PLUME::param::paramIds[1]).setValue (secondGestureParameters[1]);
    parameters.getParameterAsValue (String (firstId) + PLUME::param::paramIds[2]).setValue (secondGestureParameters[2]);
    parameters.getParameterAsValue (String (firstId) + PLUME::param::paramIds[3]).setValue (secondGestureParameters[3]);

    delete secondGesture;
}

//==============================================================================
void GestureArray::checkPitchMerging()
{
    ScopedLock gestlock (gestureArrayLock);
    
    bool pitchGest = false;
    
    for (auto* g : gestures)
    {
        if (g->isActive() && g->affectsPitch())
        {
			if (pitchGest == true)
			{
				shouldMergePitch = true;
				return;
			}
			
			pitchGest = true;
        }
    }
    
    shouldMergePitch = false;
}


void GestureArray::addMergedPitchMessage (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer)
{
    ScopedLock gestlock (gestureArrayLock);
    
    int pitchVal = 8192;
    bool send = false;
    
    // Creates a sum of pitch values, centered around 8192
    for (auto* g : gestures)
    {
        // First check: gesture is active and affects pitch
        if (g->affectsPitch() && g->isActive())
        {
            // Checks if each specific gesture should send a midi signal, before adding it to pitchVal
            int gestValue;
            g->updateMidiValue();
            
            // Vibrato
            if (g->type == Gesture::vibrato)
            {
				if (Vibrato* vib = dynamic_cast <Vibrato*> (g))
				{
					gestValue = vib->getRescaledMidiValue();

					if (vib->getSend())
					{
						send = true;
						pitchVal += gestValue - 8192;
					}
				}
            }
            
            // Pitch Bend
            else if (g->type == Gesture::pitchBend)
            {
				if (PitchBend* pb = dynamic_cast <PitchBend*> (g))
				{
					gestValue = pb->getRescaledMidiValue();

					if (pb->getSend())
					{
						send = true;
						pitchVal += gestValue - 8192;
					}
				}
            }

            // Midi mode on pitch for tilt or roll
            else
            {
                send = true;
                pitchVal += g->getRescaledMidiValue() - 8192;
            }
        }
    }

    if (!send) return; // Does nothing if no pitch midi should be sent
    
    // Limits the value to be inbounds
    if (pitchVal > 16383) pitchVal = 16383;
    else if (pitchVal < 0) pitchVal = 0;
    
	// Creates the midi message and adds it to the buffer
    Gesture::addEventAndMergePitchToBuffer (midiMessages, plumeBuffer, pitchVal, 1);
}

//==============================================================================
void GestureArray::addGestureFromXml (XmlElement& gesture)
{
    ScopedLock gestlock (gestureArrayLock);
    
    // Adds the right gesture
    switch (gesture.getIntAttribute ("type", -1))
    {
        case Gesture::vibrato:
            gestures.add (new Vibrato (gesture.getTagName().compare ("gesture") == 0 ? gesture.getStringAttribute ("name", "Vibrato")
                                                                                     : gesture.getTagName(),
                                       gesture.getIntAttribute ("id", 0), parameters,
									   float(gesture.getDoubleAttribute ("gain", PLUME::gesture::VIBRATO_RANGE_DEFAULT)),
									   float(gesture.getDoubleAttribute ("thresh", PLUME::gesture::VIBRATO_THRESH_DEFAULT)),
                                       gesture.getStringAttribute ("desc", "")));
            break;
        
        case Gesture::pitchBend:
            gestures.add (new PitchBend (gesture.getTagName().compare("gesture") == 0 ? gesture.getStringAttribute("name", "PitchBend")
                                                                                      : gesture.getTagName(),
                                         gesture.getIntAttribute("id", 1), parameters,
				                         float(gesture.getDoubleAttribute ("startLeft", PLUME::gesture::PITCHBEND_DEFAULT_LEFTMIN)),
                                         float(gesture.getDoubleAttribute ("endLeft", PLUME::gesture::PITCHBEND_DEFAULT_LEFTMAX)),
                                         float(gesture.getDoubleAttribute ("startRight", PLUME::gesture::PITCHBEND_DEFAULT_RIGHTMIN)),
                                         float(gesture.getDoubleAttribute ("endRight", PLUME::gesture::PITCHBEND_DEFAULT_RIGHTMAX)),
                                         gesture.getStringAttribute ("desc", "")));
            break;
            
        case Gesture::tilt:
            gestures.add (new Tilt (gesture.getTagName().compare("gesture") == 0 ? gesture.getStringAttribute("name", "Tilt")
                                                                                 : gesture.getTagName(),
                                    gesture.getIntAttribute("id", 2), parameters,
				                    float(gesture.getDoubleAttribute ("start", PLUME::gesture::TILT_DEFAULT_MIN)),
                                    float(gesture.getDoubleAttribute ("end", PLUME::gesture::TILT_DEFAULT_MAX)),
                                    gesture.getStringAttribute ("desc", "")));
            break;

        /* TODO WAVE
        case Gesture::wave:
            gestures.add (new Wave (gesture.getTagName().compare ("gesture") == 0 ? gesture.getStringAttribute ("name", "Wave")
                                                                                  : gesture.getTagName(),
                                    gesture.getIntAttribute("id", 0), parameters,
				                    float(gesture.getDoubleAttribute ("start", PLUME::gesture::WAVE_DEFAULT_MIN)),
                                    float(gesture.getDoubleAttribute ("end", PLUME::gesture::WAVE_DEFAULT_MAX)),
                                    gesture.getStringAttribute ("desc", "")));
            break;
        */
        case Gesture::roll:
            gestures.add (new Roll (gesture.getTagName().compare("gesture") == 0 ? gesture.getStringAttribute("name", "Roll")
                                                                                 : gesture.getTagName(),
                                    gesture.getIntAttribute("id", 3), parameters,
				                    float(gesture.getDoubleAttribute ("start", PLUME::gesture::ROLL_DEFAULT_MIN)),
                                    float(gesture.getDoubleAttribute ("end", PLUME::gesture::ROLL_DEFAULT_MAX)),
                                    gesture.getStringAttribute ("desc", "")));
            break;
        
        default:
            return;
    }
    
    // Sets the gesture parameters to the Xlm's values
    gestures.getLast()->setActive (gesture.getBoolAttribute ("on", true));
    gestures.getLast()->setMapped (gesture.getBoolAttribute ("mapped", false));
    gestures.getLast()->setMidiReverse (gesture.getBoolAttribute ("midiReverse", false));
    gestures.getLast()->setCc (gesture.getIntAttribute ("cc", 1));
    gestures.getLast()->setMidiLow (float(gesture.getDoubleAttribute ("midiStart", 0.0)), false);
    gestures.getLast()->setMidiHigh (float(gesture.getDoubleAttribute ("midiEnd", 1.0)), false);
    gestures.getLast()->setGeneratesMidi (gesture.getBoolAttribute ("midiMap", false));
    gestures.getLast()->midiType = gesture.getIntAttribute ("midiType", Gesture::controlChange);

    checkPitchMerging();
}

void GestureArray::createGestureXml (XmlElement& gesturesData)
{
    ScopedLock gestlock (gestureArrayLock);
    
    for (auto* g : gestures)
    {
        auto gestXml = new XmlElement ("gesture");
        
        // General attributes
        gestXml->setAttribute ("name", g->getName());
        gestXml->setAttribute ("type", g->type);
        gestXml->setAttribute ("desc", g->getDescription());
		gestXml->setAttribute ("id", g->id);
        gestXml->setAttribute ("on", g->isActive());
        gestXml->setAttribute ("mapped", g->isMapped());
        gestXml->setAttribute ("midiMap", g->generatesMidi());
        gestXml->setAttribute ("cc", g->getCc());
        gestXml->setAttribute ("midiReverse", g->getMidiReverse());
        gestXml->setAttribute ("midiStart", g->getMidiLow());
        gestXml->setAttribute ("midiEnd", g->getMidiHigh());
        gestXml->setAttribute ("midiType", g->midiType);
        
        // Gesture Specific attributes
        if (g->type == Gesture::vibrato)
        {
            Vibrato& v = dynamic_cast<Vibrato&> (*g);
            gestXml->setAttribute ("gain", double (v.gainDisplayRange.convertFrom0to1 (v.gain.getValue())));
			gestXml->setAttribute("thresh", double(v.thresholdDisplayRange.convertFrom0to1 (v.threshold.getValue())));
        }
        
        else if (g->type == Gesture::pitchBend)
        {
            PitchBend& pb = dynamic_cast<PitchBend&> (*g);
            gestXml->setAttribute ("startLeft", double (pb.pitchBendDisplayRange.convertFrom0to1 (pb.rangeLeftLow.getValue())));
            gestXml->setAttribute ("endLeft", double (pb.pitchBendDisplayRange.convertFrom0to1 (pb.rangeLeftHigh.getValue())));
            
            gestXml->setAttribute ("startRight", double (pb.pitchBendDisplayRange.convertFrom0to1 (pb.rangeRightLow.getValue())));
            gestXml->setAttribute ("endRight", double (pb.pitchBendDisplayRange.convertFrom0to1 (pb.rangeRightHigh.getValue())));
        }
        
        else if (g->type == Gesture::tilt)
        {
            Tilt& t = dynamic_cast<Tilt&> (*g);
            gestXml->setAttribute ("start", double (t.tiltDisplayRange.convertFrom0to1 (t.rangeLow.getValue())));
            gestXml->setAttribute ("end", double (t.tiltDisplayRange.convertFrom0to1 (t.rangeHigh.getValue())));
        }
        /* TODO WAVE
        else if (g->type == Gesture::wave)
        {
            Wave& w = dynamic_cast<Wave&> (*g);
            gestXml->setAttribute ("start", double (w.rangeLow.convertFrom0to1 (w.rangeLow.getValue())));
            gestXml->setAttribute ("end", double (w.rangeHigh.convertFrom0to1 (w.rangeHigh.getValue())));
        }
        */
        else if (g->type == Gesture::roll)
        {
            Roll& r = dynamic_cast<Roll&> (*g);
            gestXml->setAttribute ("start", double (r.rollDisplayRange.convertFrom0to1 (r.rangeLow.getValue())));
            gestXml->setAttribute ("end", double (r.rollDisplayRange.convertFrom0to1 (r.rangeHigh.getValue())));
        }
        
		createParameterXml (*gestXml, g->getParameterArray());
        
        gesturesData.addChildElement (gestXml); // Adds the element
    }
}

void GestureArray::createParameterXml(XmlElement& gestureXml, OwnedArray<Gesture::MappedParameter>& mParams)
{
    for (auto* mParam : mParams)
    {
        auto paramXml = new XmlElement (mParam->parameter.getName(30).replace (" ", "_"));
        
        paramXml->setAttribute ("id", mParam->parameter.getParameterIndex());
        paramXml->setAttribute ("start", mParam->range.getStart());
        paramXml->setAttribute ("end", mParam->range.getEnd());
        paramXml->setAttribute ("reversed", mParam->reversed);
        
        gestureXml.addChildElement (paramXml); // Adds the element
    }
}