/*
  ==============================================================================

    PlumeComponent.h
    Created: 17 Jan 2019 11:16:05am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../../../JuceLibraryCode/JuceHeader.h"

/**
 *  \class PlumeComponent PlumeComponent.h
 *
 *  \brief Plume's derived Component class.
 *
 *  This is a class that derives from JUCE's Component object. It's purpose is having
 *  a generic method to update their content (update(), which can hence be called
 *  by any other object as they all know this class) and methods to easily implement
 *  the info panel (getInfoString() to write general info, and sendAlertMessage()
 *  to prompt errors or warnings). Note that these latter use an ActionBroadcaster
 *  method, therefore the InfoPanel needs to add any PlumeComponent as a Listener
 *  for them to work properly.
 *
 *  This class should only be used for classes that would directly derive from the
 *  Component class. Also, it should mostly be used to implement the bigger components
 *  (ie the main panels and sections), the smaller can use the simple Component class just fine.
 *
 */
class PlumeComponent : public Component,
                       private ActionBroadcaster
{
public:
    /**
    *  \brief Constructor.
    */
	PlumeComponent() {}
    
    /**
    *  \brief Destructor.
    */
    ~PlumeComponent() {}
    
    /**
    *  \brief Getter to an informative text on the component.
    *
    *  This method must be overriden to return a text. This text is the
    *  one that is displayed in the InfoPanel object at the bottom of the
    *  SideBarComponent object.
    *  This method is mainly called by the InfoPanel upon a mouseEnter()
    *  callback on a PlumeComponent.
    *
    *  \returns A string that describes the component to the user
    *
    */
    virtual const String getInfoString() =0;
    
    /**
    *  \brief Generic update method.
    *
    *  This method must be overriden to update the children components
    *  of the object. Being a virtual method means it can be called by any
    *  other component or PlumeComponent.
    *  This method shouldn't be used as a repaint() or a Destruction/Reconstruction
    *  of the children elements, it is meant to set the interface elements to their
    *  up to date value. (For instance: setText() on a Label to match the internal
    *  values of an object from PlumeProcessor, after receiving a changeMessage)
    *
    */
    virtual void update() =0;
    
protected:
    /**
    *  \brief Alert message sender.
    *
    *  This method will send an action message describing an error or
    *  a warning directed to the user.
    *  In order for the method to work, the PlumeComponent must be Listened
    *  to by the InfoPanel object from the Editor.
    *
    */
    void sendAlertMessage (const String& message) { sendActionMessage (message); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeComponent)
};