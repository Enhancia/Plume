/*
  ==============================================================================

    GestureArray.h
    Created: 9 Jul 2018 2:18:55pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DataReader/DataReader.h"
#include "Gesture/Gesture.h"
#include "Gesture/Vibrato.h"

/**
 *  \class GestureArray GestureArray.h
 *
 *  \brief Gesture super class.
 *
 *  Base class to create gestures, with virtual mehtods to create MIDI or map the gesture to a parameter.
 */
class GestureArray
{
public:
    GestureArray(DataReader& reader);
    ~GestureArray();

    //==============================================================================
    void addGestureMidiToBuffer (MidiBuffer& MidiMessages);
    void updateAllMappedParameters();
    void updateAllValues();
    
    //==============================================================================
    Gesture* getGestureByName (String nameToSearch);
    void addGesture (String gestureName, int gestureType);
    void clearAllGestures ();
    
private:
    //==============================================================================
    bool requiresPitchMerging();
    MidiMessage mergePitchMessages();
    
    void initializeGestures();
    
    //==============================================================================
    OwnedArray<Gesture> gestures;
    DataReader& dataReader;
};