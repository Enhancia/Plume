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
    
    gesture.addChangeListener (this);
    gestureArray.addChangeListener (this);
    
    initializeParamCompArray();
}

MapperComponent::~MapperComponent()
{
    TRACE_IN;
    gesture.removeChangeListener (this);
    gestureArray.removeChangeListener (this);
    mapButton = nullptr;
    clearMapButton = nullptr;
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
	
    resizeArray();
    
	repaint();
}

//==============================================================================
void MapperComponent::buttonClicked (Button* bttn)
{
    TRACE_IN;
    
    if (bttn == mapButton)
    {
        // Map: clears mapMode for every other gesture, puts it on for the right one and chages the button color.
        if (gesture.mapModeOn == false)
        {
            gestureArray.cancelMapMode();
            gesture.mapModeOn = true;
            gestureArray.mapModeOn = true;
            mapButton->setColour (TextButton::buttonColourId, Colour (0xff943cb0));
            wrapper.createWrapperEditor();
        }
        
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
        gesture.mapModeOn = false;
        
        paramCompArray.clear();
        mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    }
}

void MapperComponent::changeListenerCallback(ChangeBroadcaster* source)
{   
    if (source == &gestureArray && gestureArray.mapModeOn && gesture.mapModeOn == false)
    {
        mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
        return;
    }
    
    else if (source == &gesture)
    {
        if (gesture.mapModeOn == false) mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    
        // clears then redraws the array.
        paramCompArray.clear();
        initializeParamCompArray();
        resizeArray();
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
    int w = getWidth()*2/6, h = getHeight()*3/8;
    
    // sets bounds depending on the value in the array
    for (int i=0; i<paramCompArray.size(); i++)
    {
        int x = (i%2) * w;
        int y = (i/2) * h + getHeight()/4;
        
        paramCompArray[i]->setBounds(x, y, w, h);
    }
}