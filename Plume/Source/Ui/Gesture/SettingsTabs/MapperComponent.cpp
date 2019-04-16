/*
  ==============================================================================

    Mapper.cpp
    Created: 24 Jul 2018 9:56:20pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/SettingsTabs/MapperComponent.h"

#ifndef W 
#define W Component::getWidth()
#endif

#ifndef H 
#define H Component::getHeight()
#endif

//==============================================================================
MapperComponent::MapperComponent (Gesture& gest, GestureArray& gestArr, PluginWrapper& wrap)
    :   gesture (gest), gestureArray (gestArr), wrapper (wrap)
{
    TRACE_IN;
    
    // map button
    addAndMakeVisible (mapButton = new TextButton ("Map Button"));
    mapButton->setButtonText ("Map");
    mapButton->addListener (this);
    
    // clear map button
    addAndMakeVisible (clearMapButton = new TextButton ("Clear Map Button"));
    clearMapButton->setButtonText ("Clear map");
    clearMapButton->addListener (this);
    
    // Adding the mapper as a change listener
    gesture.removeAllChangeListeners(); // In case the gesture had a previous listener
    gesture.addChangeListener (this);
    gestureArray.addChangeListener (this);
    wrapper.addChangeListener (this);
    
    initializeParamCompArray();
}

MapperComponent::~MapperComponent()
{
    TRACE_IN;
    //gesture.removeChangeListener (this);
    gestureArray.removeChangeListener (this);
    wrapper.removeChangeListener (this);
    paramCompArray.clear();
    
    mapButton->removeListener (this);
    clearMapButton->removeListener (this);
    
    mapButton = nullptr;
    clearMapButton = nullptr;
}

//==============================================================================
void MapperComponent::paint (Graphics& g)
{
}

void MapperComponent::resized()
{
    auto area = getLocalBounds();

    auto buttonsArea = area.removeFromBottom (area.getHeight()/6);

    mapButton->setBounds (buttonsArea.removeFromLeft (buttonsArea.getWidth()/2)
                                     .reduced (getWidth()/8, 0));
    clearMapButton->setBounds (buttonsArea.reduced (getWidth()/8, 0));
	
    resizeArray (area.reduced (PLUME::UI::MARGIN), NUM_COLUMNS, NUM_ROWS);
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
            mapButton->setColour (TextButton::buttonColourId, PLUME::UI::currentTheme.getColour(PLUME::colour::detailPanelActiveMapping));
            
            wrapper.createWrapperEditor (findParentComponentOfClass<AudioProcessorEditor> ());
        }
        
        // Cancels map mode for the gesture and colours it accordingly
        else
        {
            gestureArray.cancelMapMode();
            mapButton->setColour (TextButton::buttonColourId, getLookAndFeel().findColour (TextButton::buttonColourId));
        }
    }
    
    else if (bttn == clearMapButton)
    {
        // Clear all parameters and cancels map for the gesture.
        gesture.clearAllParameters();
        if (gesture.mapModeOn) gestureArray.cancelMapMode();
        
        paramCompArray.clear();
        mapButton->setColour (TextButton::buttonColourId, getLookAndFeel().findColour (TextButton::buttonColourId));
        
        getParentComponent()->repaint(); // repaints the whole gesture area
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
        mapButton->setColour (TextButton::buttonColourId, getLookAndFeel().findColour (TextButton::buttonColourId));
        return;
    }
    
    // If gesture mapping changed
    // Recreates the array of parameterComponent, and redraws the mapperComponent
    else if (source == &gesture)
    {
        if (gesture.mapModeOn == false) mapButton->setColour (TextButton::buttonColourId, getLookAndFeel().findColour (TextButton::buttonColourId));
    
        // clears then redraws the array.
        paramCompArray.clear();
        initializeParamCompArray();
        resized();
        
        getParentComponent()->repaint(); // repaints the whole gesture area
    }
    
    // If the editor is closed with map mode still on
    // Cancels map mode and colors map button to non-map
    else if (source == &wrapper && gesture.mapModeOn)
    {
        gestureArray.cancelMapMode();
        mapButton->setColour (TextButton::buttonColourId, getLookAndFeel().findColour (TextButton::buttonColourId));
    }
}

//==============================================================================
void MapperComponent::updateDisplay()
{
    if (paramCompArray.isEmpty()) return;
    
    for (auto* comp : paramCompArray)
    {
        comp->updateDisplay();
    }
}

void MapperComponent::updateComponents()
{
    if (PLUME::UI::ANIMATE_UI_FLAG)
    {
        getParentComponent()->repaint(); // repaints the whole gesture area
        repaint(); // repaints mapper component
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
    for (int i=0; i<paramCompArray.size(); i++)
    {
        addAndMakeVisible (paramCompArray[i]);
    }
}

void MapperComponent::resizeArray (juce::Rectangle<int> bounds, const int numColumns, const int numRows)
{
    int marginX = bounds.getWidth()/12;
    int marginY = bounds.getHeight()/8;

    int paramCompWidth = (bounds.getWidth() - marginX * (numColumns - 1))/numColumns;
    int paramCompHeight = (bounds.getHeight() - marginY * (numRows - 1))/numRows;
    
    // sets bounds depending on the value in the array
    for (int i=0; i<paramCompArray.size(); i++)
    {
        int paramCompX = bounds.getX() + (i % numColumns) * (paramCompWidth + marginX);
        int paramCompY = bounds.getY() + (i / numColumns)    * (paramCompHeight + marginY);

        paramCompArray[i]->setBounds (paramCompX, paramCompY, paramCompWidth, paramCompHeight);
    }
}

void MapperComponent::drawMapperText (Graphics& g, String text, int x, int y, int width, int height, bool opaqueWhenMidiMode, float fontSize)
{
    g.setFont (PLUME::font::plumeFont.withHeight (fontSize));
    g.drawText (TRANS(text), x, y, width, height,
                Justification::centred, true);
}