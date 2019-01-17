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

    // Creates the main components
    addAndMakeVisible (header = new HeaderComponent (processor));
    addAndMakeVisible (sideBar = new SideBarComponent (processor));
    
	addAndMakeVisible (gesturePanel = new GesturePanel (processor.getGestureArray(), processor.getWrapper(),
	                                                    processor.getParameterTree(), PLUME::UI::FRAMERATE));
	bool sideBarHidden = false;

	addAndMakeVisible (sideBarButton = new ShapeButton ("Side Bar Button",
                                                        PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText),
		                                                PLUME::UI::currentTheme.getColour(PLUME::colour::headerHighlightedText),
		                                                PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText)));
                                                        
	sideBarButton->setToggleState (sideBarHidden, dontSendNotification); // side bar visible at first
    sideBarButton->setClickingTogglesState (true);
	createSideBarButtonPath();
    sideBarButton->addListener (this);
	
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
	//wrapperComp->windowToFront();
    
	
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

    processor.removeActionListener (this);
    gesturePanel = nullptr;
    resizableCorner = nullptr;

    setLookAndFeel (nullptr);
}

//==============================================================================
void PlumeEditor::paint (Graphics& g)
{
    // Background
    g.fillAll (PLUME::UI::currentTheme.getColour (PLUME::colour::basePanelBackground));
    
    if (sideBarButton->getToggleState())
    {
        // Version Text
        g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::sideBarSubText));
        g.setFont (Font (10.0f, Font::italic).withTypefaceStyle ("Regular"));
        g.drawText ("Plume " + String(JucePlugin_VersionString),
		            MARGIN , getHeight() - MARGIN,
		            100, MARGIN,
                    Justification::centredLeft, true);
    }
}

void PlumeEditor::resized()
{
    using namespace PLUME::UI;
    auto area = getLocalBounds();
    
    auto sideBarArea = Rectangle<int> (sideBarButton->getToggleState() ? 0 : SIDEBAR_WIDTH, 0,
	                                   HEADER_HEIGHT, HEADER_HEIGHT);
	sideBarButton->setBounds (sideBarArea.reduced ((3*MARGIN)/2));
	if (!sideBarButton->getToggleState())
	{
		sideBar->setBounds(area.removeFromLeft(SIDEBAR_WIDTH));
	}

	header->setBounds (area.removeFromTop (HEADER_HEIGHT));
	gesturePanel->setBounds(area.reduced (2*MARGIN, 2*MARGIN));
	resizableCorner->setBounds (getWidth() - 20, getHeight() - 20, 20, 20);

	repaint();
}

//==============================================================================
void PlumeEditor::actionListenerCallback (const String &message)
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

void PlumeEditor::buttonClicked (Button* bttn)
{
    if (bttn == sideBarButton)
    {
		sideBar->setVisible (!sideBarButton->getToggleState());
		resized();

        createSideBarButtonPath();
    }
}

void PlumeEditor::createSideBarButtonPath()
{
    using namespace PLUME::UI;
    
	DBG (int (sideBarButton->getToggleState()));

    Path p;
    
    if (sideBarButton->getToggleState())
    {
        p.startNewSubPath (0,0);
        p.lineTo (HEADER_HEIGHT - 2*MARGIN, HEADER_HEIGHT/2 - MARGIN);
        p.lineTo (0, HEADER_HEIGHT - 2*MARGIN);
        p.closeSubPath();
    }
    else
    {
        p.startNewSubPath (HEADER_HEIGHT - 2*MARGIN, 0);
        p.lineTo (0, HEADER_HEIGHT/2 - MARGIN);
        p.lineTo (HEADER_HEIGHT - 2*MARGIN, HEADER_HEIGHT - 2*MARGIN);
        p.closeSubPath();
    }
    
    sideBarButton->setShape (p, false, false, false);
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
    auto gpbounds = gesturePanel->getBounds();
    gesturePanel.reset();

    addAndMakeVisible (gesturePanel = new GesturePanel (processor.getGestureArray(), processor.getWrapper(),
	                                                    processor.getParameterTree(), PLUME::UI::FRAMERATE));
	gesturePanel->setBounds(gpbounds);
	gesturePanel->addMouseListener (this, true);
	header->update();
	
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
	//wrapperComp->windowToFront();
    toFront (true);
}