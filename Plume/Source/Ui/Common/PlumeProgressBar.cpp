/*
  ==============================================================================

    PlumeProgressBar.cpp
    Created: 29 Jan 2019 3:54:30pm
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "PlumeProgressBar.h"

//==============================================================================
PlumeProgressBar::PlumeProgressBar (float& prog, String& message, const String prefix, const String finish)
    : progress (prog), progressMessage (message), messagePrefix (prefix), finishMessage (finish)
{
}

PlumeProgressBar::~PlumeProgressBar()
{
}

void PlumeProgressBar::paint (Graphics& g)
{
    bool smallBar = getHeight() < 2*BAR_H;
    int height = (!smallBar) ? BAR_H : jmax (getHeight(), BAR_H); // bar fills the component height if too small
    
    auto compArea = smallBar ? Rectangle<int> (0, jmin (0, getHeight()/2 - height), getWidth(), height)
                             : Rectangle<int> (0, getHeight()/2 - height, getWidth(), 2*height);
    
    auto barArea = (!smallBar) ? compArea.removeFromTop (height)
                               : compArea;
    // Bar Background
    g.setColour (Colours::black);
    g.fillRect (barArea.reduced (0, height/3));

    if (progress != 0.0f)
    {
        // Bar progress
        g.setColour (Colours::white);
        g.fillRect (barArea.reduced (0, height/3).withWidth (int (progress*barArea.getWidth())));
        
        if (!smallBar)
        {
            g.setColour (Colours::white);
            g.setFont (Font (PLUME::UI::font, 10.0f, Font::plain));
            
            if (progress != 1.0f)
            {
                g.drawText (messagePrefix + progressMessage + " | " + String (100*progress) + "%",
                            compArea,
                            Justification::centred, true);
            }
            else // Scan finished
            {
                g.drawText (finishMessage,
                            compArea,
                            Justification::centred, true);
            }
        }
    }
}

void PlumeProgressBar::resized()
{
}
