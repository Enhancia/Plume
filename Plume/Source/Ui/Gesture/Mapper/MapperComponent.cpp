/*
  ==============================================================================

    Mapper.cpp
    Created: 24 Jul 2018 9:56:20pm
    Author:  Alex

  ==============================================================================
*/
#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/Gesture/Mapper/MapperComponent.h"

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
#define TRACE_OUT Logger::writeToLog ("[-FNC]  Leaving: " + String(__FUNCTION__))
//==============================================================================
MapperComponent::MapperComponent (Gesture& gest, GestureArray& gestArr, PluginWrapper& wrap)
    :   gesture (gest), gestureArray (gestArr), wrapper (wrap)
{
    TRACE_IN;
    // map button
    addAndMakeVisible (mapButton = new TextButton ("Map Button"));
    mapButton->setButtonText ("Map");
    mapButton->addListener (this);
    mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    
    // clear map button
    addAndMakeVisible (clearMapButton = new TextButton ("Clear Map Button"));
    clearMapButton->setButtonText ("Clear map");
    clearMapButton->addListener (this);
    clearMapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    
    // midiMode button
    String PlumeDir = File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getFullPathName();
    Image midiOn = ImageFileFormat::loadFrom (File (PlumeDir + "/Resources/Images/Gestures/OnOff.png"));
    
    addAndMakeVisible (midiMapButton = new ImageButton ("Midi Mode Button"));
    midiMapButton->setImages (false, true, true,
								midiOn, 1.0f, Colour (0xffffffff),
								midiOn, 0.5f, Colour (0x80ffffff),
								midiOn, 1.0f, Colour (0x00000000));
    midiMapButton->setToggleState (gesture.isMidiMapped(), dontSendNotification);
    midiMapButton->setClickingTogglesState (true);
	midiMapButton->setState(gesture.isMidiMapped() ? Button::buttonDown
	                                               : Button::buttonNormal);
    midiMapButton->addListener (this);
    
    // CC label
    addAndMakeVisible (ccLabel = new Label ("CC Label", TRANS (String(gesture.getCc()))));
        
    // Label style
    ccLabel->setEditable (true, false, false);
    ccLabel->setFont (Font (13.0f, Font::plain));
    ccLabel->setColour (Label::textColourId, Colour(0xffffffff));
    ccLabel->setColour (Label::backgroundColourId, Colour(0xff000000));
    ccLabel->setJustificationType (Justification::centred);
    ccLabel->addListener (this);
    
    // Adding the mapper as a change listener
    gesture.addChangeListener (this);
    gestureArray.addChangeListener (this);
    wrapper.addChangeListener (this);
    
    initializeParamCompArray();
    setAlphas();
}

MapperComponent::~MapperComponent()
{
    TRACE_IN;
    gesture.removeChangeListener (this);
    gestureArray.removeChangeListener (this);
    wrapper.removeChangeListener (this);
    
    mapButton->removeListener (this);
    clearMapButton->removeListener (this);
    midiMapButton->removeListener (this);
    ccLabel->removeListener (this);
    
    mapButton = nullptr;
    clearMapButton = nullptr;
    midiMapButton = nullptr;
    ccLabel = nullptr;
}

//==============================================================================
void MapperComponent::paint (Graphics& g)
{
}

void MapperComponent::resized()
{
    int bttnPanW = getWidth()/3, bttnPanH = getHeight()/2;
    
    mapButton->setBounds (getWidth()*2/3 + bttnPanW/8, bttnPanH/8, bttnPanW*3/4, bttnPanH/3);
    clearMapButton->setBounds (getWidth()*2/3 + bttnPanW/4, bttnPanH*5/8, bttnPanW/2, bttnPanH/3);
    midiMapButton->setBounds (getWidth()*2/3, bttnPanH, bttnPanW/3, bttnPanH/4);
    ccLabel->setBounds (getWidth()*3/4, getHeight()*3/4 + 2, bttnPanW/2, bttnPanH/3);
	
    resizeArray();
    
	repaint();
}

//==============================================================================
void MapperComponent::buttonClicked (Button* bttn)
{
    TRACE_IN;
    
    if (bttn == mapButton)
    {
        // Map: clears mapMode for every other gesture, puts it on for the right one and changes the button color.
        if (gesture.mapModeOn == false)
        {
            gestureArray.cancelMapMode();
            gesture.mapModeOn = true;
            gestureArray.mapModeOn = true;
            mapButton->setColour (TextButton::buttonColourId, Colour (0xff943cb0));
            wrapper.createWrapperEditor();
        }
        
        // Cancels map mode for the gesture and colours it accordingly
        else
        {
            gestureArray.cancelMapMode();
            mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
        }
    }
    
    else if (bttn == clearMapButton)
    {
        // Clear all parameters and cancels map for the gesture.
        gesture.clearAllParameters();
        if (gesture.mapModeOn) gestureArray.cancelMapMode();
        
        paramCompArray.clear();
        mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    }
    
    else if (bttn == midiMapButton)
    {
        if (midiMapButton->getToggleState() == false) // midiMap off
        {
            gesture.setMidiMapped (false);
            gesture.setMapped (!(paramCompArray.isEmpty()));
            ccLabel->setEditable (false, false, false);
        }
        else // midiMapOn
        {
            gesture.setMidiMapped (true);
            gesture.setMapped (true);
            ccLabel->setEditable (true, false, false);
        }
        
        setAlphas();
        repaint();
    }
}

void MapperComponent::labelTextChanged (Label* lbl)
{
    // checks that the string is numbers only
    if (lbl->getText().containsOnly ("0123456789") == false)
    {
        lbl->setText (String (gesture.getCc()), dontSendNotification);
        return;
    }
    
    int val = lbl->getText().getIntValue();
        
    if (val < 0) val = 0;
    else if (val > 127) val = 127;
        
    gesture.setCc(val);
    
    lbl->setText (String(val), dontSendNotification);
}

void MapperComponent::changeListenerCallback(ChangeBroadcaster* source)
{   
    // if Another gesture wants to be mapped
    // Draws the map button in non-map colour
    if (source == &gestureArray && gestureArray.mapModeOn && gesture.mapModeOn == false)
    {
        mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
        return;
    }
    
    // If gesture mapping changed
    // Recreates the array of parameterComponent, and redraws the mapperComponent
    else if (source == &gesture)
    {
        if (gesture.mapModeOn == false) mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    
        // clears then redraws the array.
        paramCompArray.clear();
        initializeParamCompArray();
        resizeArray();
    }
    
    // If the editor is closed with map mode still on
    // Cancels map mode and colors map button to non-map
    else if (source == &wrapper && gesture.mapModeOn)
    {
        gestureArray.cancelMapMode();
        mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    }
}

//==============================================================================
void MapperComponent::updateDisplay()
{
    for (auto* comp : paramCompArray)
    {
        comp->updateDisplay();
    }
}

void MapperComponent::initializeParamCompArray()
{
    TRACE_IN;
    
	int i = 0;
    // adds a MappedParameterComponent for each parameter of the gesture, and makes them visible.
    for (auto* gestureParam : gesture.getParameterArray())
    {
        paramCompArray.add (new MappedParameterComponent (gesture, *gestureParam, i++));
        addAndMakeVisible (paramCompArray.getLast());
    }
}

void MapperComponent::addAndMakeArrayVisible()
{
    for (int i=0; i++; i<paramCompArray.size())
    {
        addAndMakeVisible (paramCompArray[i]);
    }
}

void MapperComponent::resizeArray()
{
    int w = getWidth()*1/3, h = getHeight()*3/8;
    
    // sets bounds depending on the value in the array
    for (int i=0; i<paramCompArray.size(); i++)
    {
        int x = (i%2) * w;
        int y = (i/2) * h + getHeight()/4;
        
        paramCompArray[i]->setBounds (x, y, w, h);
    }
}


void MapperComponent::setAlphas()
{
    if (midiMapButton->getToggleState() == false)
    {
        mapButton->setAlpha (1.0f);
        clearMapButton->setAlpha (1.0f);
        for (auto* comp : paramCompArray)
        {
            comp->setAlpha (1.0f);
        }
    
        ccLabel->setAlpha (0.5f);
        
    }
    else
    {
        mapButton->setAlpha (0.5f);
        clearMapButton->setAlpha (0.5f);
        for (auto* comp : paramCompArray)
        {
            comp->setAlpha (0.5f);
        }
        
        ccLabel->setAlpha (1.0f);
    }
}