/*
  ==============================================================================

    GestureArray.h
    Created: 9 Jul 2018 2:18:55pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../Common/PlumeCommon.h"

#include "../DataReader/DataReader.h"
#include "../Plugin/PlumeParameter.h"

#include "Gesture.h"
#include "Vibrato.h"
#include "PitchBend.h"
#include "Tilt.h"
#include "Roll.h"

/**
 *  \class GestureArray GestureArray.h
 *
 *  \brief Class that holds an array of Gesture, with several methods to use them.
 *  \sa Gesture, GestureArray(), process()
 *
 */
class GestureArray	: public ChangeListener,
                      public ChangeBroadcaster,
                      public ActionBroadcaster,
                      public AudioProcessorValueTreeState
::Listener
{
public:
    GestureArray(AudioProcessor& proc, DataReader& reader, AudioProcessorValueTreeState& params, bool& lastArmValue);
    ~GestureArray();

    //==============================================================================
    // Helper methods for processor
    
    /**
     *  \brief Main method for the processor. Will either add Midi or modify parameters for each gestures.
     *
     *  This method is called by the processor processBlock() method to create Neova's effects. It will use
     *  addGestureMidiToBuffer() or updateAllMappedParameters
     */
    void process (MidiBuffer& MidiMessages, MidiBuffer& plumeBuffer);
    
    /**
     *  \brief Method that creates the Midi for each gesture and adds it to the parameter buffer.
     *
     *  This method is called by the process() method to create Neova's MIDI. It uses each gesture
     *  Gesture::addGestureMidi() method.
     */
    void addGestureMidiToBuffer (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer);
    
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
    Gesture* getGesture (const String nameToSearch);
    
    /**
     *  \brief Getter for a Gesture object, searched using it's id.
     *
     *  The method will search the array at specified id. If the id is in range, returns a pointer to the Gesture.
     *  If the id is not in range, returns nullptr. You should check the return value after using this method!
     *
     *  \param idToSearch Id of the Gesture to look for.
     *  \return Pointer to the searched Gesture object, or nullptr if not found.
     */
    Gesture* getGesture (const int idToSearch);
    
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
     *  If specified, the String reference will hold the name of the gesture that already has the parameter.
     * 
     *  \return True if the parameter is found in any of the gestures.
     */
    bool parameterIsMapped (int parameterId, String& gestureToWhichTheParameterIsMapped);

    /**
     *  \brief Helper method to know if a specific CC is already in use.
     *
     *  \return True if any of the gestures generate this CC.
     */
    bool isCCInUse (int controllerNumber);
     
    /**
     *  \brief Helper method to know if Pitch midi is already in use.
     *
     *  \return True if any of the gestures generate pitch.
     */
    bool isPitchInUse();

    /**
     *  \brief Getter for the Plume Processor (as an AudioProcessor) 
     */
    AudioProcessor& getOwnerProcessor(); 
    
    //==============================================================================
    // Modifiers
    
    /**
     *  \brief Method to add gestures to the array
     *
     *  This method will create, add and enable a new Gesture object at the end of the array.
     *  The Gesture will have the specified name and type (chosen with the GestureType enum).
     */
    void addGesture (String gestureName, int gestureType, int gestureId);

    /**
     *  \brief Method to remove a gesture from the array
     *
     *  This method will remove a Gesture from the array, for the specified id.
     */
    void removeGesture (const int gestureId);

    /**
     *  \brief Method to remove a gesture from the array
     *
     *  This method will remove a Gesture from the array, for the specified name.
     */
    void removeGesture (const String gestureName);
    
    /**
     *  \brief Method to add a parameter to the gesture in mapMode.
     *
     */
    void addParameterToMapModeGesture (AudioProcessorParameter& param);
    
    /**
     *  \brief Method to add a parameter to the gesture in mapMode.
     *
     */
    void addAndSetParameter (AudioProcessorParameter& param, int gestureId, float start, float end, bool rev);
    
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
    // Methods to move gestures around

    bool isIdAvailable (int idToCheck);
    void moveGestureToId (int idToMoveFrom, int idToMoveTo);
    void duplicateGesture (int idToDuplicateFrom, bool prioritizeHigherId = true);
    void swapGestures (int firstId, int secondId);

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
    
    AudioProcessorValueTreeState& getParametersReference();

    //==============================================================================
    //Callbacks 
    
    /**
     *  \brief Callback called when the reader receives new data.
     *
     *  The only change broadcaster linked the this object is the DataReader object. Therefore,
     *  This method will call updateValues() to change all the gestures' values to the updated ones.
     */
    void changeListenerCallback(ChangeBroadcaster* source) override;


    void parameterChanged (const String &parameterID, float newValue) override;
    
    bool mapModeOn = false; /**< \brief Boolean to assess if one of the gestures in currently in mapMode*/
    
private:
    //==============================================================================
    /**
     *  \brief Method to merge the pitch of all pitch related gestures and add the resulting message to a MidiBuffer.
     *
     *  
     */
    void addMergedPitchMessage (MidiBuffer& midiMessages, MidiBuffer& plumeBuffer);
    
    //==============================================================================
    /**
     *  \brief Initializer method.
     *
     *  Creates the initial array of gestures.
     *  Initializes Plume with the default gestures, that eventually will be changeable / renable.
     */
    void initializeGestures();

    void addGestureCopyingOther (Gesture* other, int gestureId, String gestureName = String());
    int findClosestIdToDuplicate (int idToDuplicateFrom, bool prioritizeHigherId = true);
    String createDuplicateName (String originalGestureName);

    void notifyGestureParametersShouldBeUpdatedForType (Gesture::GestureType);
    
    //==============================================================================
    bool shouldMergePitch = false;
    
    //==============================================================================
    OwnedArray<Gesture> gestures; /**< \brief OwnedArray that holds all gesture objects*/
    DataReader& dataReader; /**< \brief Reference to the data reader object, to access the raw data from the ring*/
    AudioProcessorValueTreeState& parameters;
    AudioProcessor& owner;

    bool& armValue;
    
    CriticalSection gestureArrayLock;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GestureArray)
};
