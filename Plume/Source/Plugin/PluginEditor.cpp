/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "Plugin/PluginProcessor.h"
#include "Plugin/PluginEditor.h"

//==============================================================================
PlumeEditor::PlumeEditor (PlumeProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    TRACE_IN;
	setComponentID ("plumeEditor");

	setLookAndFeel (&plumeLookAndFeel);
	setMouseClickGrabsKeyboardFocus(false);
	setBroughtToFrontOnMouseClick (true);

	// Creates the Top Panels
	addAndMakeVisible (optionsPanel = new OptionsPanel (processor));
	optionsPanel->setVisible (false);
	optionsPanel->setAlwaysOnTop (true);
	optionsPanel->setBounds (getBounds());
	
	addAndMakeVisible (newPresetPanel = new NewPresetPanel (processor));
	newPresetPanel->setVisible (false);
	newPresetPanel->setAlwaysOnTop (true);
	newPresetPanel->setBounds (getBounds());

    // Creates the main components
    addAndMakeVisible (header = new HeaderComponent (processor));
    addAndMakeVisible (sideBar = new SideBarComponent (processor, *optionsPanel, *newPresetPanel));
    sideBar->addInfoPanelAsMouseListener (this);
    
	addAndMakeVisible (gesturePanel = new GesturePanel (processor.getGestureArray(), processor.getWrapper(),
	                                                    processor.getParameterTree(), PLUME::UI::FRAMERATE));
	// SideBarButton
	bool sideBarHidden = false;

	addAndMakeVisible (sideBarButton = new ShapeButton ("Side Bar Button",
                                                        PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText),
		                                                PLUME::UI::currentTheme.getColour(PLUME::colour::headerHighlightedText),
		                                                PLUME::UI::currentTheme.getColour(PLUME::colour::headerStandartText)));
                                                        
	sideBarButton->setToggleState (sideBarHidden, dontSendNotification); // side bar visible at first
    sideBarButton->setClickingTogglesState (true);
	createSideBarButtonPath();
	sideBarButton->addListener(this);
    
    // Adds itself as a change listener for plume's processor
    processor.addActionListener (this);

	// Base size and resize settings
	setResizable (true, false);
	addAndMakeVisible (resizableCorner = new ResizableCornerComponent (this, getConstrainer()));
	
	setSize(PLUME::UI::DEFAULT_WINDOW_WIDTH, PLUME::UI::DEFAULT_WINDOW_HEIGHT);
	setResizeLimits (getWidth()*3/4, getHeight()*3/4, getWidth()*3, getHeight()*3);

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
    optionsPanel = nullptr;
    newPresetPanel = nullptr;
    setLookAndFeel (nullptr);

	//deleteAllChildren(); // Quick & dirty way to get rid of the wrapped editor object
}

//==============================================================================
void PlumeEditor::paint (Graphics& g)
{
    // Background
    g.fillAll (PLUME::UI::currentTheme.getColour (PLUME::colour::basePanelBackground));
}

void PlumeEditor::resized()
{
    using namespace PLUME::UI;
    auto area = getLocalBounds();
    
    auto sideBarArea = juce::Rectangle<int> (sideBarButton->getToggleState() ? 0 : SIDEBAR_WIDTH, 0,
	                                   HEADER_HEIGHT, HEADER_HEIGHT);
	sideBarButton->setBounds (sideBarArea.reduced ((3*MARGIN)/2));
	if (!sideBarButton->getToggleState())
	{
		sideBar->setBounds(area.removeFromLeft(SIDEBAR_WIDTH));
	}

	header->setBounds (area.removeFromTop (HEADER_HEIGHT));
    //if (auto* wrapEd = findChildWithID ("wrapEd"))
    //{
        //wrapEd->setBounds(area);
    //}
	gesturePanel->setBounds(area.reduced (2*MARGIN, 2*MARGIN));
	resizableCorner->setBounds (getWidth() - 20, getHeight() - 20, 20, 20);
	optionsPanel->setBounds (0, 0, getWidth(), getHeight());
	newPresetPanel->setBounds (0, 0, getWidth(), getHeight());

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
	sideBar->update();
	
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

void PlumeEditor::broughtToFront()
{
    if (processor.getWrapper().isWrapping())
    {
        if (const auto* wrapperWin = processor.getWrapper().getWrapperEditorWindow())
        {
          #if JUCE_WINDOWS
            // Sets the editor window right behind
            SetWindowPos(static_cast <HWND> (wrapperWin->getPeer()->getNativeHandle()),//HWND hWnd
                         static_cast <HWND> (getPeer()->getNativeHandle()),            //HWND hWndInsertAfter
                         0, 0, 0, 0,                                                   //X, Y, cx, cy (all ignored)
                         SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);                    //UINT uFlags
          #endif
        }
    }
}