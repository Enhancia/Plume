/*
  ==============================================================================

    Mapper.cpp
    Created: 24 Jul 2018 9:56:20pm
    Author:  Alex

  ==============================================================================
*/

#include "Ui/Gesture/Mapper/MapperComponent.h"

#define W getWidth()
#define H getHeight()
#define MARGIN PLUME::UI::MARGIN

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
    
    // midiMode Component
    addAndMakeVisible (midiModeComp = new MidiModeComponent (gesture));
    
    // midiMode button
    Image midiOn = ImageFileFormat::loadFrom (PlumeData::OnOff_png, PlumeData::OnOff_pngSize);
    
    addAndMakeVisible (midiMapButton = new ImageButton ("Midi Mode Button"));
    midiMapButton->setImages (false, true, true,
								midiOn, 1.0f, Colour (0xffffffff),
								midiOn, 0.5f, Colour (0x80ffffff),
								midiOn, 1.0f, Colour (0x00000000));
    midiMapButton->setToggleState (gesture.isMidiMapped(), dontSendNotification);
    midiMapButton->setClickingTogglesState (true);
	midiMapButton->setState(gesture.isMidiMapped() ? Button::buttonDown
	                                               : Button::buttonNormal);
    midiMapButton->addListener (this);
    
    // Adding the mapper as a change listener
    gesture.addChangeListener (this);
    gestureArray.addChangeListener (this);
    wrapper.addChangeListener (this);
    
    initializeParamCompArray();
    setAlphas();
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
    midiMapButton->removeListener (this);
    
    mapButton = nullptr;
    clearMapButton = nullptr;
    midiMapButton = nullptr;
    
    midiModeComp = nullptr;
}

//==============================================================================
void MapperComponent::paint (Graphics& g)
{
    // Interface colour depending on MIDI Mode state
    Colour c1, c2;
    
    if (midiMapButton->getToggleState() == false)
    {
		c1 = PLUME::UI::currentTheme.getColour(PLUME::colour::basePanelGestureHighlightedBackground);
		c2 = PLUME::UI::currentTheme.getColour(PLUME::colour::basePanelGestureBackground);
    }
    else
    {
        c1 = PLUME::UI::currentTheme.getColour(PLUME::colour::basePanelGestureBackground);
		c2 = PLUME::UI::currentTheme.getColour(PLUME::colour::basePanelGestureHighlightedBackground);
    }
    
    g.setColour (c1);
    g.fillRoundedRectangle (0, 0, W, H, MARGIN/2);
    g.setColour (c2);
    g.fillRect (W*2/3, H/2, W - W*2/3, H/2);
    
    // "Parameters" text        
    drawMapperText (g, "Parameters",
                    0,
                    0,
                    W*3/4,
                    H/4);
                        
    // "MIDI Mode" text
    drawMapperText (g, "MIDI Mode",
                    W*7/9,
                    H/2,
                    W*2/9,
                    H/8,
                    true, 11.0f);
}

void MapperComponent::resized()
{
    int bttnPanW = W/3, bttnPanH = H/2;
    
    mapButton->setBounds (W*2/3 + bttnPanW/8, bttnPanH/8, bttnPanW*3/4, bttnPanH/3);
    clearMapButton->setBounds (W*2/3 + bttnPanW/6, bttnPanH*5/8, bttnPanW*2/3, bttnPanH/3);
    midiMapButton->setBounds (W*2/3, bttnPanH, bttnPanW/3, bttnPanH/4);
    midiModeComp->setBounds (W*2/3, bttnPanH, bttnPanW, bttnPanH);
	
    resizeArray();
    
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
            
            wrapper.createWrapperEditor();
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
    
    else if (bttn == midiMapButton)
    {
        if (midiMapButton->getToggleState() == false) // midiMap off
        {
            gesture.setMidiMapped (false);
            gesture.setMapped (!(paramCompArray.isEmpty()));
        }
        else // midiMapOn
        {
            gesture.setMidiMapped (true);
            gesture.setMapped (true);
        }
        
        setAlphas();
        getParentComponent()->repaint(); // repaints the whole gesture area
        repaint(); // repaints mapper component
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
        resizeArray();
        
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
    // midi mode button
    midiMapButton->setToggleState (gesture.isMidiMapped(), dontSendNotification);
	midiMapButton->setState(gesture.isMidiMapped() ? Button::buttonDown
	                                               : Button::buttonNormal);
	
    midiModeComp->updateComponents();
	setAlphas();
	
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

void MapperComponent::resizeArray()
{
    int w = getWidth()*1/3, h = getHeight()*3/8;
    
    // sets bounds depending on the value in the array
    for (int i=0; i<paramCompArray.size(); i++)
    {
        int x = (i%2) * w;
        int y = (i/2) * h + getHeight()/4;
    
        paramCompArray[i]->setBounds (x, y, w, h);
    }
}


void MapperComponent::setAlphas()
{
    if (midiMapButton->getToggleState() == false)
    {
        // Sets midi mode panel to transparent
        mapButton->setAlpha (1.0f);
        clearMapButton->setAlpha (1.0f);
        for (auto* comp : paramCompArray)
        {
            comp->setAlpha (1.0f);
        }
        
        midiModeComp->setAlpha (0.5f);
    
    }
    else
    {
        // Sets parameter panel and buttons to transparent
        mapButton->setAlpha (0.5f);
        clearMapButton->setAlpha (0.5f);
        for (auto* comp : paramCompArray)
        {
            comp->setAlpha (0.5f);
        }
        
        midiModeComp->setAlpha (1.0f);
    }
}

void MapperComponent::drawMapperText (Graphics& g, String text, int x, int y, int width, int height, bool opaqueWhenMidiMode, float fontSize)
{
    if (opaqueWhenMidiMode)     g.setColour (midiMapButton->getToggleState() ? Colour (0xffffffff) :
                                                                               Colour (0x80ffffff));
                                                                               
    else                        g.setColour (midiMapButton->getToggleState() ? Colour (0x80ffffff) :
                                                                               Colour (0xffffffff));
    
    g.setFont (Font (PLUME::UI::font, fontSize, Font::plain).withTypefaceStyle ("Regular"));
    g.drawText (TRANS(text), x, y, width, height,
                Justification::centred, true);
}