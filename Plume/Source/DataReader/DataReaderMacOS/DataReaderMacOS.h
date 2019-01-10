/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/
#if 0
#ifndef __RAW_DATA_READER__
#define __RAW_DATA_READER__

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DataReader/DataReaderMacOS/StatutPipe.h"
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
class DataReader   : public Component,
					 public InterprocessConnection,
                     public ChangeBroadcaster,
                     public ChangeListener
{
public:
    static constexpr int DATA_SIZE = 7;
    
    //==============================================================================
    DataReader(StatutPipe& stPipe);
    ~DataReader();

    //==============================================================================
	bool connectNewPipe(int nbPipe);
	bool isConnected();
    String getData();
    
    //==============================================================================
    bool readData(String s);
    const String getRawData(int index);
    bool getRawDataAsFloatArray(Array<float>& arrayToFill);
    
    //==============================================================================
    void connectionMade() override;
    void connectionLost() override;
    void messageReceived(const MemoryBlock &message) override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    
private:
    //==============================================================================
    bool connected;
    ScopedPointer<StringArray> data;
    StatutPipe& statutPipe;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DataReader)
};

#endif
#endif
