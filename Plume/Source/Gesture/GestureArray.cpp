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
    gestures.add (new Vibrato ("Vibrato_Default"));
    
    gestures[0]->setActive(true);
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
    
	DBG ("couldn't get the float values!!");
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
void GestureArray::addGesture (String gestureName, int gestureType)
{
    
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