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

//==============================================================================
MapperComponent::MapperComponent (Gesture& gest, GestureArray& gestArr)  : gesture (gest), gestureArray (gestArr)
{
    // map button
    addAndMakeVisible (mapButton = new TextButton ("Map Button"));
    mapButton->setButtonText ("Map");
    mapButton->addListener (this);
    mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    
    // clear map button
    addAndMakeVisible (clearMapButton = new TextButton ("Clear Map Button"));
    clearMapButton->setButtonText ("clear map");
    clearMapButton->addListener (this);
    clearMapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    
    gesture.addChangeListener (this);
    
    initializeParamCompArray();
}

MapperComponent::~MapperComponent()
{
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
    
    mapButton->setBounds (getWidth()*2/3 + bttnPanW/8, bttnPanH/8, bttnPanW*3/4, bttnPanH/4);
    clearMapButton->setBounds (getWidth()*2/3 + bttnPanW/4, bttnPanH*5/8, bttnPanW/2, bttnPanH/4);
	
    int w = getWidth()*2/6, h = getHeight()*3/8;
    
    // sets bounds depending on the value in the array
    for (int i=0; i<paramCompArray.size(); i++)
    {
        int x = (i%2) * w;
        int y = (i/2) * h + getHeight()/4;
        
        paramCompArray[i]->setBounds(x, y, w, h);
    }
    
	repaint();
}

//==============================================================================
void MapperComponent::buttonClicked (Button* bttn)
{
    if (bttn == mapButton)
    {
        if (gesture.mapModeOn == false)
        {
            gesture.mapModeOn = true;
            gestureArray.mapModeOn = true;
            mapButton->setColour (TextButton::buttonColourId, Colour (0xff943cb0));
        }
        else
        {
            gestureArray.cancelMapMode();
            mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
        }
    }
    
    else if (bttn == clearMapButton)
    {
        gesture.clearAllParameters();
        gestureArray.cancelMapMode();
        
        paramCompArray.clear();
        mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
    }
}

void MapperComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    // clears then redraws the array.
    paramCompArray.clear();
    initializeParamCompArray();
    
    int w = getWidth()*2/6, h = getHeight()*3/8;
    
    // sets bounds depending on the value in the array
    for (int i=0; i<paramCompArray.size(); i++)
    {
        int x = (i%2) * w;
        int y = (i/2) * h + getHeight()/4;
        
        paramCompArray[i]->setBounds(x, y, w, h);
    }
    
    mapButton->setColour (TextButton::buttonColourId, Colour (0xff505050));
}

//==============================================================================
void MapperComponent::updateDisplay()
{
    for (auto* comp : paramCompArray)
    {
    }
}

void MapperComponent::initializeParamCompArray()
{
    // adds a MappedParameterComponent for each parameter of the gesture, and makes them visible.
    for (auto* gestureParam : gesture.getParameterArray())
    {
        paramCompArray.add (new MappedParameterComponent (*gestureParam));
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
    
}