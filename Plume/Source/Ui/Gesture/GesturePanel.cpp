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
#include "Ui/Gesture/Tuner/GesturesTuner.h"
#include "Ui/Gesture/Mapper/MapperComponent.h"

#define MARGIN 8
#define NUM_GEST 3

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
//==============================================================================
class  GesturePanel::GestureComponent   : public Component,
										  private Button::Listener
{
public:
    GestureComponent(Gesture& gest, GestureArray& gestArray): gesture (gest), gestureArray (gestArray)
    {
        // Creates the on/off button
        String PlumeDir = File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getFullPathName();
        Image onOff = ImageFileFormat::loadFrom (File (PlumeDir + "/Resources/Images/Gestures/OnOff.png"));
    
        addAndMakeVisible (onOffButton = new ImageButton ("On Off Button"));
        onOffButton->setImages (false, true, true,
								onOff, 1.0f, Colour (0xffffffff),
								onOff, 0.5f, Colour (0x80ffffff),
								onOff, 1.0f, Colour (0x00000000));
        onOffButton->setToggleState (gesture.isActive(), dontSendNotification);
        onOffButton->setClickingTogglesState (true);
		onOffButton->setState(Button::buttonNormal);
        onOffButton->addListener (this);
        
        // Creates the right Tuner Object
        createTuner();
        
        // Creates the mapper object
        addAndMakeVisible( gestMapper = new MapperComponent(gesture, gestureArray));
    }
    
    ~GestureComponent()
    {
        onOffButton = nullptr;
        gestMapper = nullptr;
        gestTuner = nullptr;
    }
    
    void paint (Graphics& g) override
    {
		if (getWidth() == 0) return; // Nothing is painted if the component isn't set to it's right size

        int tunerWidth = getWidth()*5/8 - MARGIN;
        int mapperWidth = getWidth()*3/8 - MARGIN;
        int x, y, width, height;
        Colour fillColour;
        
        // Fills the area for the Tuner and Mapper
        { 
			if (onOffButton->getToggleState() == true)	g.setColour(Colour(0xffa0a0a0));
			else										g.setColour(Colour(0xff606060));

            g.fillRect (0, 0, tunerWidth, getHeight());
            g.fillRect (tunerWidth+2*MARGIN, 0, mapperWidth, getHeight());
            g.fillRect (tunerWidth, getHeight()*9/20, 2*MARGIN, getHeight()/10);
        }
        
        // Loads the gesture image
		{
            x = MARGIN;
            y = MARGIN;
            width = tunerWidth/8 - 2*MARGIN;
            height = getHeight() - 2*MARGIN;

            String PlumeDir = File::getSpecialLocation (File::currentApplicationFile).getParentDirectory().getFullPathName();
            File f = File (PlumeDir + "/Ressources/Images/Gestures/" + gesture.getTypeString() + ".png");
            
            Image gest = ImageFileFormat::loadFrom (f);
			g.drawImageWithin(gest, x, y, width, height,
							  RectanglePlacement(RectanglePlacement::centred));
        }
        
        // Gesture Name text
        {
            x = tunerWidth/8 + 2*MARGIN;
            y = 0;
            width = (tunerWidth - tunerWidth/8 - 2*MARGIN)*3/4;
            height = getHeight()/4;

            if (onOffButton->getToggleState() == true)	fillColour = Colour(0xffffffff);
			else										fillColour = Colour(0x80ffffff);
            g.setColour (fillColour);
                            
            String text (TRANS(gesture.name));
            g.setFont (Font (15.0f, Font::plain).withTypefaceStyle ("Regular"));
            g.drawText (text, x, y, width, height,
                        Justification::centred, true);
        }
        
        // "Values" text
        {
            x = tunerWidth*3/4 + (tunerWidth/8 + 2*MARGIN)/4;
            y = 0;
            width = (tunerWidth - tunerWidth/8 - 2*MARGIN)/4;
            height = getHeight()/4;
            
            if (onOffButton->getToggleState() == true)	fillColour = Colour(0xffffffff);
			else										fillColour = Colour(0x80ffffff);
            g.setColour (fillColour);
            
            String text (TRANS("Values"));
            g.setFont (Font (15.0f, Font::plain).withTypefaceStyle ("Regular"));
            g.drawText (text, x, y, width, height,
                        Justification::centred, true);
        }
        
        // "Parameters" text
        {
            x = getWidth()*5/8 + MARGIN;
            y = 0;
            width = mapperWidth*3/4;
            height = getHeight()/4;
                
            if (onOffButton->getToggleState() == true)	fillColour = Colour(0xffffffff);
			else										fillColour = Colour(0x80ffffff);
            g.setColour (fillColour);
            
            String text (TRANS("Parameters"));
            g.setFont (Font (15.0f, Font::plain).withTypefaceStyle ("Regular"));
            g.drawText (text, x, y, width, height,
                        Justification::centred, true);
        }
    }
    
    void resized() override
    {
		int tunerWidth = getWidth() * 5/8 - MARGIN;
		int mapperWidth = getWidth() * 3/8 - MARGIN;

        onOffButton->setBounds (tunerWidth/8 + 2*MARGIN, MARGIN, 20, 20);
        gestTuner->setBounds(tunerWidth/8 + 2*MARGIN, getHeight()/4, tunerWidth*7/8 - 2*MARGIN, getHeight()*3/4);
        gestMapper->setBounds(getWidth() * 5/8 + MARGIN, 0, mapperWidth, getHeight());
        repaint();
    }
    
    //==============================================================================
    void buttonClicked (Button* bttn) override
    {
		// Sets all subcomponents active/inactive depending of the button state
		
		gesture.setActive (onOffButton->getToggleState()); // Sets gesture active or inactive.
		gestureArray.checkPitchMerging();
		
		onOffButton->setState(onOffButton->getState() ? Button::buttonNormal:
														Button::buttonDown);
														
		repaint(); // repaints to get the inactive/active appearance
    }
    
    
    //==============================================================================
    void updateDisplay()
    {
        if (gesture.isActive()) gestTuner->updateDisplay();
        if (gesture.isMapped()) gestMapper->updateDisplay();
    }
    
private:
    void createTuner()
    {
		if      (gesture.type == Gesture::vibrato)
        {
            Vibrato& vib = dynamic_cast<Vibrato&> (gesture);
            addAndMakeVisible (gestTuner = new VibratoTuner (vib));
        }
        
        else if (gesture.type == Gesture::pitchBend)
        {
            PitchBend& pitchBend = dynamic_cast<PitchBend&> (gesture);
            addAndMakeVisible (gestTuner = new PitchBendTuner (pitchBend));
        }
        
        else if (gesture.type == Gesture::tilt)
        {
            Tilt& tilt = dynamic_cast<Tilt&> (gesture);
            addAndMakeVisible (gestTuner = new TiltTuner (tilt));
        }
        /*
        else if (gesture.type == Gesture::wave)
        {
            Wave& wave = dynamic_cast<Wave&> (gesture);
            addAndMakeVisible (gestTuner = new WaveTuner (wave));
        }
        */
        else if (gesture.type == Gesture::roll)
        {
            Roll& roll = dynamic_cast<Roll&> (gesture);
            addAndMakeVisible (gestTuner = new RollTuner (roll));
        }
        else
        {
            DBG ("Unknown Gesture type. No tuner was created.");
        }
    }
    
    Gesture& gesture;
    GestureArray& gestureArray;
    ScopedPointer<ImageButton> onOffButton;
    
    ScopedPointer<Tuner> gestTuner;
    ScopedPointer<MapperComponent> gestMapper;
};

//==============================================================================
GesturePanel::GesturePanel(GestureArray& gestArray, int freqHz) : gestureArray (gestArray)
{
    TRACE_IN;
    freq = freqHz;
}

GesturePanel::~GesturePanel()
{
    TRACE_IN;
}

void GesturePanel::paint (Graphics& g)
{
}

void GesturePanel::resized()
{
}

void GesturePanel::initialize()
{  
    startTimerHz (freq);
    
    int gestureHeight = (getHeight() - (NUM_GEST - 1) * 2*MARGIN) / NUM_GEST; // gets the height of each gesture component
    
    for (int i=0; i<NUM_GEST && i<gestureArray.size(); i++)
    {
        // Loop that creates and places a gestureComponent for each existing gesture.
        gestureComponents.add (new GestureComponent (*gestureArray.getGestureById (i), gestureArray));
        addAndMakeVisible (gestureComponents[i]);
        gestureComponents[i]->setBounds (0, i*(gestureHeight + MARGIN), getWidth(), gestureHeight);
    }
}

void GesturePanel::timerCallback()
{
    for (auto* gComp : gestureComponents)
    {
        gComp->updateDisplay();
    }
}