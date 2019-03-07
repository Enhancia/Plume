/*
  ==============================================================================

    WrappedPluginEditor.cpp
    Created: 12 Apr 2018 11:05:19am
    Author:  Alex LEVACHER

  ==============================================================================
*/

#include "Wrapper/PluginWrapper.h"

//==============================================================================
WrapperEditorWindow::WrapperEditorWindow (WrapperProcessor& wrapProc, const Component* componentWhichWindowToAttachTo)
       : wrapperProcessor (wrapProc), topLevelPlumeComponent (*componentWhichWindowToAttachTo->getTopLevelComponent())
{
    TRACE_IN;
    setSize (400, 300);

    if (auto* ui = createProcessorEditor (wrapperProcessor.getWrappedInstance()))
    {
        // Adds the interface and sets the component size in accordance
        addAndMakeVisible (wrappedUi = ui);
        childBoundsChanged (wrappedUi);

        // Creates the desktop window, attached to a component (Plume's editor)
        setOpaque (true);
        editorHandle = (componentWhichWindowToAttachTo == nullptr) ? nullptr :
                                                         componentWhichWindowToAttachTo->getPeer()->getNativeHandle();
        addToDesktop (ComponentPeer::windowHasTitleBar + ComponentPeer::windowHasCloseButton, nullptr); //handle);

        // Prevents the window to get lost on the side of the screen
        // (since you can't access it with the task bar to close it....)
        getPeer()->setConstrainer(wrappedUi->getConstrainer());
        getPeer()->getConstrainer()->setMinimumOnscreenAmounts (getHeight()*5, // top
                                                                20,            // left
                                                                getHeight(),   // bottom
                                                                20             // right
                                                                );
        getPeer()->setTitle ("Plume | " + wrapperProcessor.getName());

        if (componentWhichWindowToAttachTo == nullptr)
        {
            setTopLeftPosition (0, 0);
        }
        else
        {
            setTopLeftPosition (componentWhichWindowToAttachTo->getScreenBounds().getTopLeft());
        }

        setVisible (true);
    }

    else 
    {
        wrapperProcessor.getOwnerWrapper().clearWrapperEditor();
    }
}

WrapperEditorWindow::~WrapperEditorWindow()
{
    TRACE_IN;

    wrappedUi.deleteAndZero();
    if (isOnDesktop()) removeFromDesktop();
}

void WrapperEditorWindow::paint (Graphics& g)
{
    //g.fillAll (Colours::black);
}

void WrapperEditorWindow::resized()
{
    //wrappedUi->setBounds (getBounds());
}

void WrapperEditorWindow::childBoundsChanged (Component* child)
{
    if (child == wrappedUi && child != nullptr
                           && child->getWidth() > 0
                           && child->getHeight() > 0)
    {
        setSize (child->getWidth(), child->getHeight());
    }
}

void WrapperEditorWindow::userTriedToCloseWindow()
{
    wrappedUi.deleteAndZero();
    wrapperProcessor.getOwnerWrapper().clearWrapperEditor();
}

void WrapperEditorWindow::broughtToFront()
{
  #if JUCE_WINDOWS
    // Sets the editor window right behind
    SetWindowPos(static_cast <HWND> (editorHandle),                 //HWND hWnd
                 static_cast <HWND> (getPeer()->getNativeHandle()), //HWND hWndInsertAfter
                 0, 0, 0, 0,                                        //X, Y, cx, cy (all ignored because of uFlags)
                 SWP_NOACTIVATE + SWP_NOMOVE + SWP_NOSIZE);         //UINT uFlags
  #endif
}

AudioProcessorEditor* WrapperEditorWindow::createProcessorEditor (AudioProcessor& processor)
{
    if (auto* ui = processor.createEditorIfNeeded())
    {
        return ui;
    }
        
    jassertfalse;
    return nullptr;
}