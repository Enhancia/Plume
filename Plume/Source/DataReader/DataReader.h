/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef __RAW_DATA_READER__
#define __RAW_DATA_READER__

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"
#include "../../juce_serialport/juce_serialport.h"
#include "../Common/PlumeCommon.h"

#if JUCE_MAC
#include "StatutPipe.h"
#endif

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

/**
 *  \class DataReader DataReader.h
 *
 *  \brief Class that gets the raw data from the ring.
 *
 *  This is a class that is instanciated by the PlumeProcessor object. It inherits from InterprocessConnection
 *  so that it can create a pipe, to which a RawDataReader app can connect to send data.
 */
class DataReader :
	public ActionBroadcaster,
	public ChangeBroadcaster,
	public ChangeListener,
	public Component,
	private InterprocessConnection,
	public MultiTimer
{
public:
    static constexpr int DATA_SIZE = PLUME::data::numDatas;

    // Getters Setters
    float& getBatteryReference ();

    bool getHubIsConnected () const;
    bool getRingIsConnected () const;
    bool getRingIsCharging () const;

    void setHubIsConnected (bool value);
    void setRingIsConnected (bool value);
    void setRingIsCharging (bool value);
    
    // Constructor Destructor
	DataReader();
    ~DataReader() override;

    // Editor logic
    void paint (Graphics&) override;
    void resized() override;

    // Processor logic
    bool readData(String s);
    const String getRawData(int index);
    bool getRawDataAsFloatArray(Array<float>& arrayToFill);
    const float& getFloatValueReference (const PLUME::data::PlumeData data);
    
    //==============================================================================
    void sendString(uint8_t* data, int data_size);

    //==============================================================================
    void timerCallback (int timerID) override;
    
    /**
        @Brief Tracks statutPipe messages

        Calls connectToExistingPipe(int nbPipe) with the pipe ID received by statutPipe
        MacOs only
     */
    void changeListenerCallback(ChangeBroadcaster* source) override;
    
    /**
        @Brief Instantiate StatutPipe object

        Instantiate and add listener on StatutPipe
        MacOs only
     */
    void instantiateStatutPipe();
        
    //==============================================================================
    bool connectToExistingPipe();
    bool connectToExistingPipe(int nbPipe);
    bool isConnected();
    
    //==============================================================================
    void connectionMade() override;
    void connectionLost() override;
    void messageReceived(const MemoryBlock &message) override;
    
    
private:
    //==============================================================================
    bool connected;
    int pipeNumber = -1;
    
    std::unique_ptr<StringArray> data;
	float batteryValue = 0.0f;

    bool hubIsConnected = false;
    bool ringIsConnected = false;
    bool ringIsCharging = false;

  #if JUCE_MAC
    std::unique_ptr<StatutPipe> statutPipe;
  #endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataReader)
};

#endif // __RAW_DATA_READER__
