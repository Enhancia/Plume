/*
  ==============================================================================

    GestureArray.cpp
    Created: 9 Jul 2018 2:18:55pm
    Author:  Alex

  ==============================================================================
*/

#include "GestureArray.h"


GestureArray::GestureArray(DataReader& reader)  : dataReader (reader)
{
    initializeGestures();
}

GestureArray::~GestureArray()
{
    gestures.clear();
}

//==============================================================================
void GestureArray::initializeGestures()
{
    addGesture("Vibrato_Default", Gesture::vibrato);
}

void GestureArray::addGestureMidiToBuffer (MidiBuffer& MidiMessages)
{
    for (auto* g : gestures)
    {
        g->addGestureMidi (MidiMessages);
    }
}


//==============================================================================
void GestureArray::updateAllMappedParameters ()
{
    for (auto* g : gestures)
    {
        g->updateMappedParameters();
    }
}


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
}

void GestureArray::clearAllGestures()
{
    gestures.clear();
}

//==============================================================================
bool GestureArray::requiresPitchMerging()
{
    return false;
}

/*
MidiMessage GestureArray::mergePitchMessages()
{
}
*/