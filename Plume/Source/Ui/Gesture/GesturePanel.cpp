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


#define MARGIN 8
#define NUM_GEST 3

//==============================================================================
class  GesturePanel::GestureComponent   : public Component,
										  private Button::Listener
{
public:
    GestureComponent(Gesture& gest): gesture (gest)
    {
        // Creates the on/off button
        Image onOff = ImageFileFormat::loadFrom (File ("D:/Workspace/GitWorkspace/Plume/Plume/Ressources/Images/Gestures/onOff.png"));
    
        // Creates the buttons and the label
        addAndMakeVisible (onOffButton = new ImageButton ("On Off Button"));
        onOffButton->setImages (false, true, true,
								onOff, 1.0f, Colour (0xffffffff),
								onOff, 0.5f, Colour (0x80ffffff),
								onOff, 1.0f, Colour (0x00000000));
        onOffButton->setToggleState (gesture.isActive(), dontSendNotification);
        onOffButton->setClickingTogglesState (true);
		onOffButton->setState(Button::buttonNormal);
        onOffButton->addListener (this);
    }
    
    ~GestureComponent()
    {
    }
    
    void paint (Graphics& g) override
    {
		if (getWidth() == 0) return; // Nothing is painted if the component isn't set to it's right size

        int tunerWidth = getWidth()*5/8 - MARGIN;
        int mapperWidth = getWidth()*3/8 - MARGIN;
        
        {
            // Fills the area for the Tuner and Mapper 
			if (onOffButton->getToggleState() == true)	g.setColour(Colour(0xffa0a0a0));
			else										g.setColour(Colour(0xff606060));

            g.fillRect (0, 0, tunerWidth, getHeight());
            g.fillRect (tunerWidth+2*MARGIN, 0, mapperWidth, getHeight());
            g.fillRect (tunerWidth, getHeight()*9/20, 2*MARGIN, getHeight()/10);
        }
        
		{
		    // Loads the gesture image
            int x = MARGIN,
                y = MARGIN,
                width = tunerWidth/8 - 2*MARGIN,
                height = getHeight() - 2*MARGIN;

            Image gest = ImageFileFormat::loadFrom (File ("D:/Workspace/GitWorkspace/Plume/Plume/Ressources/Images/Gestures/"+
                                                    gesture.getTypeString()+
                                                    ".png"));
			g.drawImageWithin(gest, x, y, width, height,
							  RectanglePlacement(RectanglePlacement::centred));
        }

        {
            // Gesture Name text
            int x = tunerWidth/8 + 2*MARGIN,
                y = 0,
                width = (tunerWidth - tunerWidth/8 - 2*MARGIN)*3/4,
                height = getHeight()/4;
                
            String text (TRANS(gesture.name));
            Colour fillColour;
            if (onOffButton->getToggleState() == true)	fillColour = Colour(0xffffffff);
			else										fillColour = Colour(0x80ffffff);

            g.setColour (fillColour);
            g.setFont (Font (15.0f, Font::plain).withTypefaceStyle ("Regular"));
            g.drawText (text, x, y, width, height,
                        Justification::centred, true);
        }
        
        {
            // "Values" text
            int x = tunerWidth*3/4 + (tunerWidth/8 + 2*MARGIN)/4,
                y = 0,
                width = (tunerWidth - tunerWidth/8 - 2*MARGIN)/4,
                height = getHeight()/4;
                
            String text (TRANS("Values"));
            Colour fillColour;
            if (onOffButton->getToggleState() == true)	fillColour = Colour(0xffffffff);
			else										fillColour = Colour(0x80ffffff);

            g.setColour (fillColour);
            g.setFont (Font (15.0f, Font::plain).withTypefaceStyle ("Regular"));
            g.drawText (text, x, y, width, height,
                        Justification::centred, true);
        }
    }
    
    void resized() override
    {
		int tunerWidth = getWidth() * 5/8 - MARGIN;

        onOffButton->setBounds (tunerWidth/8 + 2*MARGIN, MARGIN, 20, 20);
        repaint();
    }
    
    //==============================================================================
    void buttonClicked (Button* bttn) override
    {
		// Sets all subcomponents active/inactive depending of the button state
		
		gesture.setActive (onOffButton->getToggleState()); // Sets gesture active or inactive.
		
		onOffButton->setState(onOffButton->getState() ? Button::buttonNormal:
														Button::buttonDown);
														
		repaint(); // repaints to get the inactive/active appearance
    }
    
    
private:
    Gesture& gesture;
    ScopedPointer<ImageButton> onOffButton;
};

//==============================================================================
GesturePanel::GesturePanel(GestureArray& gestArray) : gestureArray (gestArray)
{
}

GesturePanel::~GesturePanel()
{
}

void GesturePanel::paint (Graphics& g)
{
    //g.fillAll(Colour (0xffffffff));
}

void GesturePanel::resized()
{
}

void GesturePanel::initialize()
{  
    int gestureHeight = (getHeight() - (NUM_GEST - 1) * 2*MARGIN) / NUM_GEST; // gets the height of each gesture component
    
    for (int i=0; i<NUM_GEST && i<gestureArray.size(); i++)
    {
        // Loop that creates and places a gestureComponent for each existing gesture.
        gestureComponents.add (new GestureComponent (*gestureArray.getGestureById (i)));
        addAndMakeVisible (gestureComponents[i]);
        gestureComponents[i]->setBounds (0, i*(getHeight() + MARGIN), getWidth(), gestureHeight);
    }
}
