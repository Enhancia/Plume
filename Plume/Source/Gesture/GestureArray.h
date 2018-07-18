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
    // Helper methods for processor
    void addGestureMidiToBuffer (MidiBuffer& MidiMessages);
    void updateAllMappedParameters();
    void updateAllValues();
    
    //==============================================================================
    // Getters
    Gesture* getGestureByName (const String nameToSearch);
    Gesture* getGestureById(const int idToSearch);
    OwnedArray<Gesture>& getArray();
    int size();
    
    //==============================================================================
    // Modifiers
    void addGesture (String gestureName, int gestureType);
    void clearAllGestures ();
    
private:
    //==============================================================================
    // Pitch message issue handler methods
    bool requiresPitchMerging();
    MidiMessage mergePitchMessages();
    
    //==============================================================================
    /**
     *  \brief Initializer method.
     *
     *  Creates the initial array of gestures.
     *  Initializes Plume with the default gestures, that eventually will be changeable / renable.
     */
    void initializeGestures();
    
    //==============================================================================
    OwnedArray<Gesture> gestures; /** < OwnedArray that holds all gesture objects*/
    DataReader& dataReader; /** < Reference to the data reader object, to access the raw data from the ring*/
};