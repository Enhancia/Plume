/*
  ==============================================================================

    MappedParameterComponent.h
    Created: 24 Jul 2018 8:43:58pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Gesture/Gesture.h"

//==============================================================================
/*
*/
class MappedParameterComponent    : public Component,
                                    private Label::Listener,
									private Button::Listener
{
public:
    //==============================================================================
    MappedParameterComponent(Gesture::MappedParameter& mappedParam)  : mappedParameter (mappedParam)
    {
        // Creates the close button
        Image close = ImageFileFormat::loadFrom (File ("D:/Workspace/GitWorkspace/Plume/Plume/Ressources/Images/Gestures/Close.png"));
    
        addAndMakeVisible (closeButton = new ImageButton ("Close Button"));
        closeButton->setImages (false, true, true,
								close, 1.0f, Colour (0x00000000),
								close, 0.5f, Colour (0x00000000),
								close, 0.7f, Colour (0x501600f0));
        closeButton->addListener (this);
    }
    
    ~MappedParameterComponent()
    {
    }

    //==============================================================================
    void paint (Graphics& g) override
    {   
        // Parameter Name text
        {
			int x = 0,
				y = 0,
				width = getWidth()*2/3,
				height = getHeight()/3;
				
            g.setColour (Colours::black);
                            
            String text (TRANS(mappedParameter.parameter.getName(20)));
            g.setFont (Font (10.0f, Font::plain).withTypefaceStyle ("Regular"));
            g.drawText (text, x, y, width, height,
                        Justification::centred, true);
        }
    }
    
	void resized() override
	{
	    closeButton->setBounds (getWidth()*2/3, getHeight()/3, 10, 10);
	    repaint();
	}
	
    //==============================================================================
    void buttonClicked (Button* bttn) override
    {
    }
    
    void labelTextChanged (Label* lbl) override
    {
    }
    
private:
    //==============================================================================
    Gesture::MappedParameter& mappedParameter;

    //==============================================================================
    ScopedPointer<ImageButton> closeButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MappedParameterComponent)
};
