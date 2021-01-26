/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PlumeEditor::PlumeEditor (PlumeProcessor& p)
    : AudioProcessorEditor (&p), processor (p), ComponentMovementWatcher (this)
{
    TRACE_IN;
	setComponentID ("plumeEditor");

	setLookAndFeel (&plumeLookAndFeel);
	setMouseClickGrabsKeyboardFocus(false);
	setBroughtToFrontOnMouseClick (true);

	// Creates the Top Panels
    addAndMakeVisible (newGesturePanel = new NewGesturePanel (processor));
    newGesturePanel->hidePanel();

	addAndMakeVisible (optionsPanel = new OptionsPanel (processor));
	optionsPanel->setVisible (false);
	optionsPanel->setAlwaysOnTop (true);
	
	addAndMakeVisible (newPresetPanel = new NewPresetPanel (processor));
	newPresetPanel->setVisible (false);
	newPresetPanel->setAlwaysOnTop (true);

    // Creates the main components
    addAndMakeVisible (header = new HeaderComponent (processor, *newPresetPanel));
    addAndMakeVisible (sideBar = new SideBarComponent (processor, *optionsPanel));
    sideBar->addInfoPanelAsMouseListener (this);
    
    
	addAndMakeVisible (gesturePanel = new GesturePanel (processor.getGestureArray(), processor.getWrapper(),
	                                                    processor.getParameterTree(), *newGesturePanel,
                                                        PLUME::UI::FRAMERATE));
                                                        
	// SideBarButton
	bool sideBarHidden = false;

	addAndMakeVisible (sideBarButton = new HideSideBarButton());
                                                        
	sideBarButton->setToggleState (sideBarHidden, dontSendNotification); // side bar visible at first
    sideBarButton->setClickingTogglesState (true);
	sideBarButton->addListener(this);
    

    // Adds itself as a change listener for plume's processor
    processor.addActionListener (this);
    if (auto* infoPanel = dynamic_cast<InfoPanel*> (sideBar->findChildWithID ("infoPanel")))
        PlumeComponent::listenToAllChildrenPlumeComponents (this, infoPanel, false);

	// Base size and resize settings
	setResizable (true, false);
	addAndMakeVisible (resizableCorner = new ResizableCornerComponent (this, getConstrainer()));

	setSize(PLUME::UI::DEFAULT_WINDOW_WIDTH, PLUME::UI::DEFAULT_WINDOW_HEIGHT);
	setResizeLimits (getWidth(), getHeight(), getWidth()*3, getHeight()*3);


	for (Component* comp : getChildren())
    {
        comp->addMouseListener(this, true);
    }

	PLUME::UI::ANIMATE_UI_FLAG = true;

  #if JUCE_WINDOWS
    if (auto messageManagerPtr = MessageManager::getInstanceWithoutCreating())
    {
    	plumeWindowHook = SetWindowsHookExA(WH_CALLWNDPROC, PLUME::messageHook,
                                            NULL, (DWORD) messageManagerPtr->getCurrentMessageThread());

    	jassert (plumeWindowHook != NULL);
    }
  #endif

    newGesturePanel->toFront (false);
    sideBarButton->toFront (false);
}

PlumeEditor::~PlumeEditor()
{
    TRACE_IN;

	if (processor.getWrapper().isWrapping())
	{
		processor.getWrapper().clearWrapperEditor();
	}

    PLUME::UI::ANIMATE_UI_FLAG = false;
	for (Component* comp : getChildren())
	{
		comp->removeMouseListener(this);
	}

  #if JUCE_WINDOWS
    if (plumeWindowHook != NULL)
    {
        int unhookResult = UnhookWindowsHookEx (plumeWindowHook);
        jassert (unhookResult != 0);
    }

    plumeWindowHook = NULL;
    instanceHWND = NULL;
  #endif

    processor.removeActionListener (this);
    gesturePanel = nullptr;
    resizableCorner = nullptr;
    optionsPanel = nullptr;
    newPresetPanel = nullptr;
    newGesturePanel = nullptr;
    setLookAndFeel (nullptr);
}

//==============================================================================
void PlumeEditor::paint (Graphics& g)
{
    // Background
    g.fillAll (getPlumeColour (plumeBackground));
}

void PlumeEditor::resized()
{
    using namespace PLUME::UI;
    auto area = getLocalBounds();
    
    optionsPanel->setBounds (area);
    newPresetPanel->setBounds (area);
    
    auto sideBarButtonArea = juce::Rectangle<int> (sideBarButton->getToggleState() ? 0 : SIDEBAR_WIDTH, 0,
	                                               HEADER_HEIGHT - 2*MARGIN, HEADER_HEIGHT);
	//sideBarButton->setBounds (sideBarButtonArea.reduced (MARGIN, MARGIN*3/2));

	if (!sideBarButton->getToggleState())
	{
		sideBar->setBounds(area.removeFromLeft (SIDEBAR_WIDTH));
	}

    sideBarButton->setBounds (area.withWidth (HEADER_HEIGHT/2)
                                  .withSizeKeepingCentre (HEADER_HEIGHT/2, HEADER_HEIGHT*3/2));

	header->setBounds (area.removeFromTop (HEADER_HEIGHT));
    newGesturePanel->setBounds (area);

    area.reduce (sideBarButton->getWidth(), sideBarButton->getWidth());
	gesturePanel->setBounds (area);

	resizableCorner->setBounds (getWidth() - 20, getHeight() - 20, 20, 20);

	repaint();
}

//==============================================================================
void PlumeEditor::componentPeerChanged()
{
  #if JUCE_WINDOWS
    if (getPeer() != nullptr) // Peer was just created!
    {
		if (!plumeHWNDIsSet)
        {
            registerEditorHWND();

            if (processor.getWrapper().isWrapping())
            {
                processor.getWrapper().createWrapperEditor (this);
            }
        }
    }

    else // Peer was deleted
    {
        PLUME::globalPointers.removePlumeHWND (instanceHWND);
    }
  #endif
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
        //sideBarButton->repaint();
		resized();
    }
}

void PlumeEditor::createSideBarButtonPath()
{
    /*
    using namespace PLUME::UI;

    Path p;
    
    if (sideBarButton->getToggleState())
    {
        p.startNewSubPath (0,0);
        p.lineTo (HEADER_HEIGHT - 2*MARGIN, HEADER_HEIGHT/2 - MARGIN);
        p.lineTo (0, HEADER_HEIGHT - 2*MARGIN);
    }
    else
    {
        p.startNewSubPath (HEADER_HEIGHT - 2*MARGIN, 0);
        p.lineTo (0, HEADER_HEIGHT/2 - MARGIN);
        p.lineTo (HEADER_HEIGHT - 2*MARGIN, HEADER_HEIGHT - 2*MARGIN);
    }
    
    sideBarButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText), 2.0f);
    sideBarButton->setShape (p, false, false, false);
    */
}

void PlumeEditor::mouseEnter (const MouseEvent &event)
{
    if (event.eventComponent == sideBarButton)
    {
        //sideBarButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerHighlightedText), 2.0f);
    }
}

void PlumeEditor::mouseExit (const MouseEvent &event)
{
    if (event.eventComponent == sideBarButton)
    {
        //sideBarButton->setOutline (PLUME::UI::currentTheme.getColour (PLUME::colour::headerStandartText), 2.0f);
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
    removeChildComponent (gesturePanel);
    auto gpbounds = gesturePanel->getBounds();

    gesturePanel.reset (new GesturePanel (processor.getGestureArray(), processor.getWrapper(),
                                          processor.getParameterTree(), *newGesturePanel,
                                          PLUME::UI::FRAMERATE));
    addAndMakeVisible (gesturePanel, 0);
	gesturePanel->setBounds(gpbounds);
    
	header->update();
	sideBar->update();
	
	PLUME::UI::ANIMATE_UI_FLAG = true;
	toFront (true);
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
  #if JUCE_WINDOWS
	if (instanceHWND != NULL)
		PLUME::globalPointers.setActiveHWND(instanceHWND);
  #endif

    if (processor.getWrapper().isWrapping())
    {
        if (auto* wrapperWin = processor.getWrapper().getWrapperEditorWindow())
        {
          #if JUCE_WINDOWS
            // Sets the editor window right behind
            SetWindowPos(static_cast <HWND> (wrapperWin->getPeer()->getNativeHandle()),//HWND hWnd
                         static_cast <HWND> (getPeer()->getNativeHandle()),            //HWND hWndInsertAfter
                         0, 0, 0, 0,                                                   //X, Y, cx, cy (all ignored)
                         SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);                    //UINT uFlags
          #elif JUCE_MAC
            // Sets the editor window right behind
            wrapperWin->getPeer()->toBehind (this->getPeer());
          #endif
        }
    }
}

void PlumeEditor::minimisationStateChanged (bool isNowMinimized)
{
    if (processor.getWrapper().isWrapping())
    {
        processor.getWrapper().minimiseWrapperEditor (isNowMinimized);
    }
}

#if JUCE_WINDOWS
void PlumeEditor::registerEditorHWND()
{
    if (plumeHWNDIsSet) return;
    
    instanceHWND = GetAncestor (static_cast <HWND> (getPeer()->getNativeHandle()), GA_ROOT);
    PLUME::globalPointers.addPlumeHWND (instanceHWND);

    plumeHWNDIsSet = true;
}
#endif