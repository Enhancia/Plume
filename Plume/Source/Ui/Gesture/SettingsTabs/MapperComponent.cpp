/*
  ==============================================================================

    Mapper.cpp
    Created: 24 Jul 2018 9:56:20pm
    Author:  Alex

  ==============================================================================
*/

#include "mappercomponent.h"

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
    resizeArray (getLocalBounds(), NUM_COLUMNS, NUM_ROWS);
}

//==============================================================================
void MapperComponent::updateDisplay()
{
    if (paramCompArray.isEmpty())
    {
        return;
    }
    
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
    ScopedLock paramComplock (paramCompArrayLock);

    for (auto* comp : paramCompArray)
    {
        comp->updateHighlightColour();
    }
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
        paramCompArray.add (new MappedParameterComponent (gesture, gestureArray, wrapper, *gestureParam, i++));
        addAndMakeVisible (paramCompArray.getLast());
    }

    resized();
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
            paramCompArray.add (new MappedParameterComponent (gesture, gestureArray, wrapper,
                                                              *gesture.getParameterArray()[i], i));
            addAndMakeVisible (paramCompArray.getLast());
        }
    }
    else if (paramCompArray.size() >= gesture.getParameterArray().size()) // Parameters were removed
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
                    paramCompArray.set (i, new MappedParameterComponent (gesture, gestureArray, wrapper,
                                                                         *gestureParam, i));
                    addAndMakeVisible (paramCompArray[i]);

                    if (paramCompArray.size() != gesture.getParameterArray().size())
                        shouldCheckIfUpdateIsNecessary = false;
                }
            }
            else
            {
                paramCompArray.set (i, new MappedParameterComponent (gesture, gestureArray, wrapper,
                                                                     *gestureParam, i));
                addAndMakeVisible (paramCompArray[i]);
            }

            i++;
        }

        // Trims outdated parameterComponents
        if (paramCompArray.size() > gesture.getParameterArray().size())
        {
            paramCompArray.removeLast (paramCompArray.size() - gesture.getParameterArray().size());
        }
    }

    resized();
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
    int marginX = PLUME::UI::MARGIN;
    int marginY = PLUME::UI::MARGIN;

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
    mapButton->setButtonText ("MAP");
    mapButton->setColour (TextButton::buttonColourId, gesture.getHighlightColour());
    mapButton->setColour (TextButton::buttonOnColourId, gesture.getHighlightColour()
                                                               .interpolatedWith (Colour (0xffffffff), 0.4f));
    mapButton->addListener (this);
}

MapperBanner::~MapperBanner()
{
    mapButton->removeListener (this);
    mapButton = nullptr;
}

void MapperBanner::paint (Graphics& g)
{
	paintParameterSlotDisplay(g, getLocalBounds().withTrimmedLeft (mapButton->getWidth() + PLUME::UI::MARGIN)
                                                 .withTrimmedRight (50),
                                 1, 6, 12);
}

void MapperBanner::resized()
{
    using namespace PLUME::UI;
    auto area = getLocalBounds();

    mapButton->setBounds (area.removeFromLeft (area.getWidth()/4));
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
            mapButton->setColour (TextButton::buttonColourId,
                                  gesture.getHighlightColour().interpolatedWith (Colour (0xffffffff),
                                                                                 0.4f));
            
            wrapper.createWrapperEditor (findParentComponentOfClass<AudioProcessorEditor> ());
        }
        
        // Cancels map mode for the gesture and colours it accordingly
        else
        {
            gestureArray.cancelMapMode();
            mapButton->setColour (TextButton::buttonColourId,
                                  gesture.getHighlightColour());
        }
    }
}

void MapperBanner::updateComponents()
{
    using namespace PLUME::colour;

    mapButton->setColour (TextButton::buttonColourId,
                          gesture.mapModeOn ? gesture.getHighlightColour()
                                                     .interpolatedWith (Colour (0xffffffff), 0.4f)
                                            : gesture.getHighlightColour());
    repaint();
}


void MapperBanner::paintParameterSlotDisplay  (Graphics& g, juce::Rectangle<int> area,
                                                            const int numRows,
                                                            const int numColumns,
                                                            const int sideLength)
{
    /*  Hitting this assert means you're trying to paint this object with a number of
        parameters that doesn't match the actual maximum number of parameters allowed
        for a gesture.
    */
    jassert (numRows * numColumns == PLUME::MAX_PARAMETER);

    int rowHeight = area.getHeight()/numRows;
    int columnWidth = area.getWidth()/numColumns;

    for (int row=0; row < numRows; row++)
    {
        auto columnArea = area.removeFromTop (rowHeight);

        for (int column=0; column < numColumns; column++)
        {
            int slotSide = jmin (sideLength, rowHeight - 8, columnWidth - 8);
            auto slotArea = columnArea.removeFromLeft (columnWidth)
                                      .withSizeKeepingCentre (slotSide, slotSide);

            g.setColour ((row*numColumns) + column < gesture.getParameterArray().size() ?
                            gesture.getHighlightColour() :
                            getPlumeColour (plumeBackground));
            g.fillRoundedRectangle (slotArea.toFloat(), sideLength / 3.5f);
        }
    }
}
