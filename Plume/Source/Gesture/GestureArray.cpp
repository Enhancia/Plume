/*
  ==============================================================================

    GestureArray.cpp
    Created: 9 Jul 2018 2:18:55pm
    Author:  Alex

  ==============================================================================
*/

#include "Gesture/GestureArray.h"


GestureArray::GestureArray(DataReader& reader)  : dataReader (reader)
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
    addGesture ("Vibrato_Default", Gesture::vibrato);
    addGesture ("PitchBend_Default", Gesture::pitchBend);
    addGesture ("Tilt_Default", Gesture::tilt);
}

//==============================================================================
void GestureArray::process (MidiBuffer& midiMessages)
{
    // adds midi to buffer and updates parameters
    addGestureMidiToBuffer(midiMessages);
    updateAllMappedParameters();
}

void GestureArray::addGestureMidiToBuffer (MidiBuffer& midiMessages)
{
    if (shouldMergePitch)
    {
        // Adds non-pitch midi
        for (auto* g : gestures)
        {
            if (g->affectsPitch() == false && g->isMapped() == false)
            {
                g->addGestureMidi (midiMessages);
            }
        }
        
        // Adds pitch midi
        addMergedPitchMessage(midiMessages);
    }
    
    else
    {
        // Adds all midi
        for (auto* g : gestures)
        {
            if (g->isMapped() == false)
            {
                g->addGestureMidi (midiMessages);
            }
        }
    }
}

void GestureArray::updateAllMappedParameters()
{
    for (auto* g : gestures)
    {
        if (mapModeOn == false && g->isMapped())
        {
            g->updateMappedParameters();
        }
    }
}

//==============================================================================
void GestureArray::updateAllValues()
{
    Array<float> rawData;
    
    if (dataReader.getRawDataAsFloatArray(rawData))
    {
        for (auto* g : gestures)
        {
            g->updateValue (rawData);
        }
        return;
    }
    
	DBG ("couldn't get the float values. No vaues were updated.");
}

//==============================================================================
Gesture* GestureArray::getGestureByName (const String nameToSearch)
{
    for (auto* g : gestures)
    {
        if (g->name.compare(nameToSearch) == 0)
        {
            return g;
        }
    }
    
    DBG ("Gesture " << nameToSearch << " doesn't exist");
    return nullptr;
}

Gesture* GestureArray::getGestureById (const int idToSearch)
{
    if (idToSearch >= gestures.size() || idToSearch < 0)
    {
        DBG ("Gesture n°" << idToSearch << " doesn't exist. Number of gestures: " << gestures.size());
        return nullptr;
    }
    
    return gestures[idToSearch];
}

OwnedArray<Gesture>& GestureArray::getArray()
{
    return gestures;
}

int GestureArray::size()
{
    return gestures.size();
}

//==============================================================================
void GestureArray::changeListenerCallback(ChangeBroadcaster* source)
{
    updateAllValues();
}

//==============================================================================
void GestureArray::addGesture (String gestureName, int gestureType)
{
    switch (gestureType)
    {
        case Gesture::vibrato:
            gestures.add (new Vibrato (gestureName));
            break;
        
        case Gesture::pitchBend:
            gestures.add (new PitchBend (gestureName));
            break;
            
        case Gesture::tilt:
            gestures.add (new Tilt (gestureName));
            break;
        /*    
        case Gesture::wave:
            gestures.add (new Wave (gestureName));
            break;
            
        case Gesture::roll:
            gestures.add (new Roll (gestureName));
            break;
        */
    }
    
    gestures.getLast()->setActive(true);
    
    checkPitchMerging();
    
}

void GestureArray::addParameterToMapModeGesture (AudioProcessorParameter& param)
{
    for (auto* g : gestures)
    {
        if (g->mapModeOn == true)
        {
            g->addParameter(param);
            cancelMapMode();
            return;
        }
    }
}

void GestureArray::clearAllGestures()
{
    gestures.clear();
    shouldMergePitch = false;
}

void GestureArray::clearAllParameters()
{
    for (auto* g : gestures)
    {
        g->mapModeOn = false;
        g->clearAllParameters();
    }
}

void GestureArray::cancelMapMode()
{
    for (auto* g : gestures)
    {
        g->mapModeOn = false;
    }
    mapModeOn = false;
}

//==============================================================================
void GestureArray::checkPitchMerging()
{
    bool pitchGest = false;
    
    for (auto* g : gestures)
    {
        if (g->isActive() && g->affectsPitch())
        {
            if (pitchGest == false) pitchGest = true;
			else if (pitchGest == true)
			{
				shouldMergePitch = true;
				return;
			}
        }
    }
    
    shouldMergePitch = false;
}


void GestureArray::addMergedPitchMessage (MidiBuffer& midiMessages)
{
    int pitchVal = 8192;
    
    // Creates a sum of pitch values, centered around 8192
    for (auto* g : gestures)
    {
        if (g->affectsPitch() && g->isActive())
        {
            pitchVal = pitchVal + g->getMidiValue() - 8192;
        }
    }
    
    // Limits the value to be inbounds
    if (pitchVal > 16383) pitchVal = 16383;
    else if (pitchVal < 0) pitchVal = 0;
    
	// Creates the midi message and adds it to the buffer
	MidiMessage message = MidiMessage::pitchWheel (1, pitchVal);
    midiMessages.addEvent(message, 1);
}