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
#include "Gesture/PitchBend.h"
#include "Gesture/Tilt.h"
#include "Gesture/Roll.h"

/**
 *  \class GestureArray GestureArray.h
 *
 *  \brief Class that holds an array of Gesture, with several methods to use them.
 *
 */
class GestureArray	: public ChangeListener,
                      public ChangeBroadcaster
{
public:
    GestureArray(DataReader& reader);
    ~GestureArray();

    //==============================================================================
    // Helper methods for processor
    
    /**
     *  \brief Main method for the processor. Will either add Midi or modify parameters for each gestures.
     *
     *  This method is called by the processor processBlock() method to create Neova's effects. It will use
     *  addGestureMidiToBuffer() or updateAllMappedParameters
     */
    void process (MidiBuffer& MidiMessages);
    
    /**
     *  \brief Method that creates the Midi for each gesture and adds it to the parameter buffer.
     *
     *  This method is called by the process() method to create Neova's MIDI. It uses each gesture
     *  Gesture::addGestureMidi() method.
     */
    void addGestureMidiToBuffer (MidiBuffer& midiMessages);
    
    /**
     *  \brief Method that changes the value of every mapped parameter.
     *
     *  This method is called by the processor processBlock() method to update every mapped parameter's value.
     *  It uses each gesture Gesture::updateMappedParameters() method.
     */
    void updateAllMappedParameters();
    
    /**
     *  \brief Method that updates each gesture's raw value.
     *
     *  This method can be called whenever all values need to be updated.
     *  This will be called in changeListenerCallback(), that detects when new raw data was received.
     */
    void updateAllValues();
    
    //==============================================================================
    // Getters
    
    /**
     *  \brief Getter for a Gesture object, searched using it's name.
     *
     *  The method will search the array for the specific gesture name. If found, returns a pointer to the Gesture.
     *  If not found, returns nullptr. You should check the return value after using this method!
     *
     *  \param nameToSearch Name of the Gesture to look for.
     *  \return Pointer to the searched Gesture object, or nullptr if not found.
     */
    Gesture* getGestureByName (const String nameToSearch);
    
    /**
     *  \brief Getter for a Gesture object, searched using it's id.
     *
     *  The method will search the array at specified id. If the id is in range, returns a pointer to the Gesture.
     *  If the id is not in range, returns nullptr. You should check the return value after using this method!
     *
     *  \param nameToSearch Name of the Gesture to look for.
     *  \return Pointer to the searched Gesture object, or nullptr if not found.
     */
    Gesture* getGestureById(const int idToSearch);
    
    /**
     *  \brief Getter for the array's reference.
     *
     *  This gets a reference to the OwnedArray object holding the Gesture objetcs.
     *
     */
    OwnedArray<Gesture>& getArray();
    
    /**
     *  \brief Getter for the array's size.
     *
     *  Calls the OwnedArray::size() method to get the number of gestures held by the object.
     *  
     *  \return The number of gestures currently held by the object.
     */
    int size();
    
    /**
     *  \brief Helper method to know if a pitch merge is needed.
     *  
     */
    void checkPitchMerging();
    
    /**
     *  \brief Helper method to know if a specific parameter is already mapped.
     *
     *  \return True if the parameter is found in any of the gestures.
     */
    bool parameterIsMapped (int parameterId);
     
    
    //==============================================================================
    // Modifiers
    
    /**
     *  \brief Method to add gestures to the array
     *
     *  This method will create, add and enable a new Gesture object at the end of the array.
     *  The Gesture will have the specified name and type (chosen with the GestureType enum).
     */
    void addGesture (String gestureName, int gestureType);
    
    /**
     *  \brief Method to add a parameter to the gesture in mapMode.
     *
     */
    void addParameterToMapModeGesture (AudioProcessorParameter& param);
    
    /**
     *  \brief Method to add a parameter to the gesture in mapMode.
     *
     */
    void addAndSetParameter (AudioProcessorParameter& param, int gestureId, float start, float end);
    
    /**
     *  \brief Deletes all gestures in the array.
     *
     *  Used when there is a need to reset the gestures, and in the GestureArray destructor.
     */
    void clearAllGestures ();
    
    /**
     *  \brief Deletes all mapped parameters from every gesture.
     *
     *  Useful when loading a new plugin or a preset.
     *
     */
    void clearAllParameters();
    
    /**
     *  \brief Method that puts every Gesture's to a non mapping state.
     *
     */
    void cancelMapMode();
    
    //==============================================================================
    // Xml related methods
    
    /**
     *  \brief Loader method allowing to initialize the array using an Xml Element.
     *  
     */
    void addGestureFromXml(XmlElement& gestureXml);
    
    /**
     *  \brief Method that creates an Xml element for all the gestures and adds it to the parameter element.
     *  
     */
    void createGestureXml(XmlElement& gesturesData);
    
    /**
     *  \brief Helper method that creates an Xml element for all the parameters of a gesture.
     *  
     */
    void createParameterXml(XmlElement& gestureXml, OwnedArray<Gesture::MappedParameter>& mParams);
    
    //==============================================================================
    //Callbacks 
    
    /**
     *  \brief Callback called when the reader receives new data.
     *
     *  The only change broadcaster linked the this object is the DataReader object. Therefore,
     *  This method will call updateValues() to change all the gestures' values to the updated ones.
     */
    void changeListenerCallback(ChangeBroadcaster* source) override;
    
    bool mapModeOn = false; /**< \brief Boolean to assess if one of the gestures in currently in mapMode*/
    
private:
    //==============================================================================
    /**
     *  \brief Method to merge the pitch of all pitch related gestures and add the resulting message to a MidiBuffer.
     *
     *  
     */
    void addMergedPitchMessage (MidiBuffer& midiMessages);
    
    //==============================================================================
    /**
     *  \brief Initializer method.
     *
     *  Creates the initial array of gestures.
     *  Initializes Plume with the default gestures, that eventually will be changeable / renable.
     */
    void initializeGestures();
    
    //==============================================================================
    bool shouldMergePitch = false;
    
    //==============================================================================
    OwnedArray<Gesture> gestures; /**< \brief OwnedArray that holds all gesture objects*/
    DataReader& dataReader; /**< \brief Reference to the data reader object, to access the raw data from the ring*/
    AudioProcessorValueTreeState& parameters;
};