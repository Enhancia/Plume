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
    gestures.add (new Vibrato ("default_vibrato"));
    
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
        g->updateMappedParameter();
    }
}


void GestureArray::updateAllValues()
{
    float rawData[7] = {0.0f};
    
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
Gesture* GestureArray::getGestureByName (String nameToSearch)
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