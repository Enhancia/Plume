/*
  ==============================================================================

    WrappedPluginEditor.cpp
    Created: 12 Apr 2018 11:05:19am
    Author:  Alex LEVACHER

  ==============================================================================
*/

#include "../../JuceLibraryCode/JuceHeader.h"

#if (JUCE_WINDOWS || defined(__OBJC__))

#if JUCE_MAC
#define Component juce::Component
#endif
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
        
        addToDesktop (ComponentPeer::windowHasTitleBar + ComponentPeer::windowIsTemporary
                                                       + ComponentPeer::windowHasCloseButton,
                      findHostHandle());
        
      #if JUCE_WINDOWS
        // Prevents the window to get lost on the side of the screen
        // (since you can't access it with the task bar to close it....)
        getPeer()->setConstrainer(wrappedUi->getConstrainer());
        getPeer()->getConstrainer()->setMinimumOnscreenAmounts (getHeight()*5, // top
                                                                20,            // left
                                                                getHeight(),   // bottom
                                                                20             // right
                                                                );
      #elif JUCE_MAC
        if (auto plumeView = (NSView*) (componentWhichWindowToAttachTo->getPeer()->getNativeHandle()))
        {
            auto plumeWin = [plumeView window];
            auto wrappedWin = [(NSView*) (this->getPeer()->getNativeHandle()) window];
            
            // Causes the wrapped plugin to hide whenever plume hides
            [wrappedWin setHidesOnDeactivate:YES];
            // Allows the wrapped plugin to both get in front and behind plume
            [wrappedWin setLevel: [plumeWin level]];
        }
      #endif
        
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

void WrapperEditorWindow::paint (Graphics&) {}

void WrapperEditorWindow::resized() {}

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

void* WrapperEditorWindow::findHostHandle()
{
    if (editorHandle == nullptr) return nullptr;

  #if JUCE_WINDOWS
    if (auto editorHwnd = static_cast<HWND> (editorHandle))
    {
        // If DAW has an owned window with children windows (ending with PLUME)
        if (HWND rootOwnerHostWindow = GetWindow (GetAncestor (editorHwnd, GA_ROOT), GW_OWNER))
        {
            return static_cast<void*> (rootOwnerHostWindow);
        }

        // If DAW only has an owned window with PLUME
        else if (HWND ownerHostWindow = GetWindow (editorHwnd, GW_OWNER))
        {
            return static_cast<void*> (ownerHostWindow);
        }

        // If DAW has an owned WS_POPUP with children windows (ending with PLUME)
        // Or if DAW only has children windows that end with PLUME
        else if (HWND rootParentHostWindow = GetAncestor (editorHwnd, GA_ROOTOWNER))
        {
            return static_cast<void*> (rootParentHostWindow);
        }
    }
  #endif
  
	// If Plume's window is not owned by / related to the DAW. This is the case on mac (and is really not likely on win).
    return nullptr;
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
#endif // WIN || OBJC
