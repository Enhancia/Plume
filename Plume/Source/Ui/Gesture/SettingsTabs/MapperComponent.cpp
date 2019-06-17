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

    initializeParamCompArray();
}

MapperComponent::~MapperComponent()
{
    TRACE_IN;
    paramCompArray.clear();
}

//==============================================================================
void MapperComponent::paint (Graphics& g)
{
}

void MapperComponent::resized()
{
    resizeArray (getLocalBounds().reduced (PLUME::UI::MARGIN), NUM_COLUMNS, NUM_ROWS);
	//repaint();
}

//==============================================================================
void MapperComponent::updateDisplay()
{
    if (paramCompArray.isEmpty()) return;
    
    if (PLUME::UI::ANIMATE_UI_FLAG)
    {
        for (auto* comp : paramCompArray)
        {
            comp->updateDisplay();
        }
    }
}

void MapperComponent::updateComponents()
{
}

void MapperComponent::initializeParamCompArray()
{
    TRACE_IN;
    ScopedLock paramComplock (paramCompArrayLock);
    paramCompArray.clear();
    
	int i = 0;
    // adds a MappedParameterComponent for each parameter of the gesture, and makes them visible.
    for (auto* gestureParam : gesture.getParameterArray())
    {
        paramCompArray.add (new MappedParameterComponent (gesture, *gestureParam, i++));
        addAndMakeVisible (paramCompArray.getLast());
    }
}

void MapperComponent::updateParamCompArray()
{
    TRACE_IN;
    ScopedLock paramComplock (paramCompArrayLock);
    
    if (paramCompArray.size() < gesture.getParameterArray().size()) // Parameters were added
    {
        // Adds every new parameter as a parameterComp at the end of the array
        for (int i=paramCompArray.size(); i<gesture.getParameterArray().size(); i++)
        {
            paramCompArray.add (new MappedParameterComponent (gesture, *gesture.getParameterArray()[i], i));
            addAndMakeVisible (paramCompArray.getLast());
        }
    }
    else if (paramCompArray.size() > gesture.getParameterArray().size()) // Parameters were removed
    {
        int i = 0;
        bool shouldCheckIfUpdateIsNecessary = true;

        // Checks for each gesture parameter if there should be an update. If so, updates it.
        // As soon as one parameter is updated, updates all the subsequent ones without checking.
        for (auto* gestureParam : gesture.getParameterArray())
        {
            if (shouldCheckIfUpdateIsNecessary)
            {
                if (&(paramCompArray[i]->getMappedParameter()) != gestureParam)
                {
                    paramCompArray.set (i, new MappedParameterComponent (gesture, *gestureParam, i));
                    addAndMakeVisible (paramCompArray[i]);
                    shouldCheckIfUpdateIsNecessary = false;
                }
            }
            else
            {
                paramCompArray.set (i, new MappedParameterComponent (gesture, *gestureParam, i));
                addAndMakeVisible (paramCompArray[i]);
            }

            i++;
        }

        if (!shouldCheckIfUpdateIsNecessary)
        {
            // Trims outdated parameterComponents
            paramCompArray.removeLast (paramCompArray.size() - gesture.getParameterArray().size());
        }
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
        int paramCompY = bounds.getY() + (i / numColumns) * (paramCompHeight + marginY);

        paramCompArray[i]->setBounds (paramCompX, paramCompY, paramCompWidth, paramCompHeight);
    }
}

//==============================================================================
MapperBanner::MapperBanner (Gesture& gest, GestureArray& gestArr, PluginWrapper& wrap)
    :   gesture (gest), gestureArray (gestArr), wrapper (wrap)
{
    TRACE_IN;
    
    // map button
    addAndMakeVisible (mapButton = new TextButton ("Map Button"));
    mapButton->setButtonText ("Map");
    mapButton->addListener (this);
}

MapperBanner::~MapperBanner()
{
    mapButton->removeListener (this);
    mapButton = nullptr;
}

void MapperBanner::paint (Graphics& g)
{
    g.setColour (Colour (0xff202020));
    g.setFont (PLUME::font::plumeFont.withHeight(13));
    g.drawText (String ("Parameters : ") + String (gesture.getParameterArray().size())
                                         + String ("/")
                                         + String (PLUME::MAX_PARAMETER),
                        getLocalBounds().withSizeKeepingCentre (getWidth()/2, getHeight()),
                        Justification::centred);
}

void MapperBanner::resized()
{
    using namespace PLUME::UI;
    auto area = getLocalBounds();

    mapButton->setBounds (area.removeFromLeft (area.getWidth()/3).reduced (MARGIN/2));
}


void MapperBanner::buttonClicked (Button* bttn)
{
    if (bttn == mapButton)
    {
        // Map: clears mapMode for every other gesture, puts it on for the right one and changes the button color.
        if (gesture.mapModeOn == false && gesture.getParameterArray().size() < PLUME::MAX_PARAMETER)
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
            mapButton->setColour (TextButton::buttonColourId,
                                  getLookAndFeel().findColour (TextButton::buttonColourId));
        }
    }
}

void MapperBanner::updateComponents()
{
    using namespace PLUME::colour;

    mapButton->setColour (TextButton::buttonColourId,
                          gesture.mapModeOn ? PLUME::UI::currentTheme.getColour (detailPanelActiveMapping)
                                            : getLookAndFeel().findColour (TextButton::buttonColourId));
    repaint();
}