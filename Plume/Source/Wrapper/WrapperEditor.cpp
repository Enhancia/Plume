/*
  ==============================================================================

    WrappedPluginEditor.cpp
    Created: 12 Apr 2018 11:05:19am
    Author:  Alex LEVACHER

  ==============================================================================
*/

#include "Wrapper/PluginWrapper.h"

//==============================================================================
WrapperEditorWindow::WrapperEditorWindow (WrapperProcessor& wrapProc, int x, int y)
       : DocumentWindow (wrapProc.getName(),
                         LookAndFeel::getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                         /*DocumentWindow::minimiseButton | */DocumentWindow::closeButton),
         wrapperProcessor (wrapProc)
{
    TRACE_IN;
    setSize (400, 300);

    if (auto* ui = createProcessorEditor (wrapperProcessor.getWrappedInstance()))
    {
        setContentOwned (ui, true);
    }
    
    setVisible (true);
}

WrapperEditorWindow::~WrapperEditorWindow()
{
    TRACE_IN;
}

void WrapperEditorWindow::closeButtonPressed()
{
    clearContentComponent();
    wrapperProcessor.getOwnerWrapper().clearWrapperEditor();
}

void WrapperEditorWindow::focusGained (FocusChangeType cause)
{
    if (cause != Component::focusChangedDirectly)
    {
        toFront (false);
        //wrapperProcessor.getOwnerWrapper().getOwner().sendActionMessage (PLUME::commands::toFront);
    }
}

AudioProcessorEditor* WrapperEditorWindow::createProcessorEditor (AudioProcessor& processor)
{
    if (auto* ui = processor.createEditorIfNeeded())
    {
        toFront (false);
        return ui;
    }
        
    jassertfalse;
    return nullptr;
}
