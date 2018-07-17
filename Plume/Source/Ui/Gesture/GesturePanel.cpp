/*
  ==============================================================================

    GesturePanel.cpp
    Created: 11 Jul 2018 3:42:22pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Gesture/GestureArray.h"
#include "Ui/Gesture/GesturePanel.h"

//==============================================================================
class  GesturePanel::GestureComponent   : public Component
{
public:
    GestureComponent(Gesture& gest): gesture (gest)
    {
    }
    
    ~GestureComponent()
    {
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll(Colour (0xffeeeeee));
    }
    
    void resized() override
    {
    }
    
private:
    Gesture& gesture;
};

//==============================================================================
GesturePanel::GesturePanel(GestureArray& gestArray) : gestureArray (gestArray)
{
    initialize();
}

GesturePanel::~GesturePanel()
{
}

void GesturePanel::paint (Graphics& g)
{
}

void GesturePanel::resized()
{
}

void GesturePanel::initialize()
{
}
