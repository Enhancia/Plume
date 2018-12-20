/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"
#include "Plugin/PluginEditor.h"

#define TOP_PANEL 70
#define MARGIN PLUME::UI::MARGIN


#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
//==============================================================================
PlumeEditor::PlumeEditor (PlumeProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    TRACE_IN;
    
    setLookAndFeel (&plumeLookAndFeel);
	setMouseClickGrabsKeyboardFocus(false);
	setBroughtToFrontOnMouseClick (true);

    // Creates the 3 main components
    addAndMakeVisible (wrapperComp = new WrapperComponent (processor.getWrapper()));
    addAndMakeVisible (presetComp = new PresetComponent (processor, *this));
	addAndMakeVisible (gesturePanel = new GesturePanel (processor.getGestureArray(), processor.getWrapper(),
	                                                    processor.getParameterTree(), PLUME::UI::FRAMERATE));
	                                                    
	
	
    // Adds itself as a change listener for plume's processor
    processor.addActionListener (this);

	// Base size and resize settings
	setResizable (true, false);
	addAndMakeVisible (resizableCorner = new ResizableCornerComponent (this, getConstrainer()));
	
	setSize(PLUME::UI::DEFAULT_WINDOW_WIDTH, PLUME::UI::DEFAULT_WINDOW_HEIGHT);
	setResizeLimits (getWidth()*2/3, getHeight()*2/3, getWidth()*3, getHeight()*3);

	for (Component* comp : getChildren())
    {
        comp->addMouseListener(this, true);
    }

	// if a WrappedEditor currently exists, puts it in front (useful because hosts actually deletes the editor when not shown)
	wrapperComp->windowToFront();

	PLUME::UI::ANIMATE_UI_FLAG = true;
}

PlumeEditor::~PlumeEditor()
{
    TRACE_IN;
    PLUME::UI::ANIMATE_UI_FLAG = false;
	for (Component* comp : getChildren())
	{
		comp->removeMouseListener(this);
	}

	//wrapperComp->removeMouseListener(this);
	//presetComp->removeMouseListener(this);
	//gesturePanel->removeMouseListener(this);
	//resizableCorner->removeMouseListener(this);

    processor.removeActionListener (this);
    wrapperComp = nullptr;
    presetComp = nullptr;
    gesturePanel = nullptr;
    resizableCorner = nullptr;

    setLookAndFeel (nullptr);
}

//==============================================================================
void PlumeEditor::paint (Graphics& g)
{
    // Background
    g.fillAll (plumeLookAndFeel.getPlumeColour (PlumeLookAndFeel::background));
    
    // Version Text
    g.setColour (Colour (0xff000000));
    g.setFont (Font (10.0f, Font::italic).withTypefaceStyle ("Regular"));
    g.drawText ("Plume " + String(JucePlugin_VersionString),
                MARGIN, getHeight() - MARGIN, 100, MARGIN,
                Justification::centredLeft, true);
}

void PlumeEditor::resized()
{
	wrapperComp->setBounds(MARGIN, MARGIN, getWidth() * 3 / 4, TOP_PANEL);
	presetComp->setBounds(getWidth() * 3 / 4 + 2 * MARGIN, MARGIN, getWidth() - getWidth() * 3 / 4 - 3 * MARGIN, TOP_PANEL);
	gesturePanel->setBounds(2 * MARGIN, TOP_PANEL + 3 * MARGIN, getWidth() - 4 * MARGIN, getHeight() - TOP_PANEL - 5 * MARGIN);
	resizableCorner->setBounds (getWidth() - 20, getHeight() - 20, 20, 20);
}

//==============================================================================
void PlumeEditor::actionListenerCallback(const String &message)
{
    TRACE_IN;
    
    if (message.compare (PLUME::commands::updateInterface) == 0)
    {
        updateFullInterface();
    }
    
    else if (message.compare (PLUME::commands::lockInterface) == 0)
    {
        //gesturePanel.reset();
        //gesturePanel->stopTimer();
    }
    
    else if (message.compare (PLUME::commands::unlockInterface) == 0)
    {
        if (!gesturePanel->isTimerRunning())
        {
            gesturePanel->startTimerHz (PLUME::UI::FRAMERATE);
        }
    }
}

//==============================================================================
PlumeProcessor& PlumeEditor::getProcessor()
{
    return processor;
}

//==============================================================================
void PlumeEditor::updateFullInterface()
{
    TRACE_IN;
    
    gesturePanel.reset();
    
    wrapperComp->update();
    presetComp->update();

    addAndMakeVisible (gesturePanel = new GesturePanel (processor.getGestureArray(), processor.getWrapper(),
	                                                    processor.getParameterTree(), PLUME::UI::FRAMERATE));
	gesturePanel->setBounds(2 * MARGIN, TOP_PANEL + 3 * MARGIN, getWidth() - 4 * MARGIN, getHeight() - TOP_PANEL - 5 * MARGIN);
	gesturePanel->addMouseListener (this, true);
	
	PLUME::UI::ANIMATE_UI_FLAG = true;
}

void PlumeEditor::setInterfaceUpdates (bool shouldUpdate)
{
    if (shouldUpdate)
    {
        if (gesturePanel == nullptr)
        {
            updateFullInterface();
        }
        else
        {
            gesturePanel->startTimerHz (PLUME::UI::FRAMERATE);
        }
    }
    else
    {
        gesturePanel->stopTimer();
        gesturePanel.reset();
    }
}

void PlumeEditor::setWindowsToFront()
{
	wrapperComp->windowToFront();
    toFront (true);
}