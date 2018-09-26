/*
  ==============================================================================

    WrappedPluginEditor.cpp
    Created: 12 Apr 2018 11:05:19am
    Author:  Alex LEVACHER

  ==============================================================================
*/

#include "Wrapper/PluginWrapper.h"

#define TRACE_IN  Logger::writeToLog ("[+FNC] Entering: " + String(__FUNCTION__))
//==============================================================================
WrapperEditorWindow::WrapperEditorWindow (WrapperProcessor& wrapProc)
       : DocumentWindow (wrapProc.getName(),
                         LookAndFeel::getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                         DocumentWindow::minimiseButton | DocumentWindow::closeButton),
         wrapperProcessor (wrapProc)
{
    TRACE_IN;
    setSize (400, 300);

    if (auto* ui = createProcessorEditor (wrapperProcessor.getWrappedInstance()))
    {
        setContentOwned (ui, true);
    }
            
    setTopLeftPosition (/*wrapperProcessor.getOwnerProcessor().getActiveEditor() == nullptr ?*/
                        Point<int> (0,0)/* :
                        wrapperProcessor.getOwnerProcessor().getActiveEditor()->getPosition()*/);
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

AudioProcessorEditor* WrapperEditorWindow::createProcessorEditor (AudioProcessor& processor)
{
    if (auto* ui = processor.createEditorIfNeeded())
    {
        return ui;
    }
        
    jassertfalse;
    return nullptr;
}
