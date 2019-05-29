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
    \class  PlumeComponent PlumeComponent.h
 
    \brief  Plume's derived Component class.
 
            This is a class that derives from JUCE's Component object. It's purpose is having
            a generic method to update their content (update(), which can hence be called
            by any other object as they all know this class) and methods to easily implement
            the info panel (getInfoString() to write general info, and sendAlertMessage()
            to prompt errors or warnings). Note that these latter use an ActionBroadcaster
            method, therefore the InfoPanel needs to add any PlumeComponent as a Listener
            for them to work properly.
  
            This class should only be used for classes that would directly derive from the
            Component class. Also, it should mostly be used to implement the bigger components
            (ie the main panels and sections), the smaller can use the simple Component class just fine.
 
*/
class PlumeComponent : public Component,
                       public ActionBroadcaster
{
public:
  /**
    \brief Constructor.
  */
	PlumeComponent() {}
    
  /**
    \brief Destructor.
  */
	~PlumeComponent() { removeAllActionListeners(); }
    
    /**
        \brief    Getter to an informative text on the component.
    
                  This method must be overriden to return a text. This text is the
                  one that is displayed in the InfoPanel object at the bottom of the
                  SideBarComponent object.
                  This method is mainly called by the InfoPanel upon a mouseEnter()
                  callback on a PlumeComponent.
     
        \returns  A string that describes the component to the user. String() if the component
                  doesn't have a description.
    
    */
    virtual const String getInfoString() =0;
    
    /**
        \brief  Generic update method.
    
                This method must be overriden to update the children components
                of the object. Being a virtual method means it can be called by any
                other component or PlumeComponent.
                This method shouldn't be used as a repaint() or a Destruction/Reconstruction
                of the children elements, it is meant to set the interface elements to their
                up to date value. (For instance: setText() on a Label to match the internal
                values of an object from PlumeProcessor, after receiving a changeMessage)
    
    */
    virtual void update() =0;

    /**
        \brief  Method to check if this component returns a description.
    */
    const bool hasInfoString()
    {
        return (!getInfoString().isEmpty());
    }
    
    /**
        \brief  Static helper method for the InfoPanel.
    
                This method will recursively navigate through all the child components of the
                attribute Component, registering the listener for every Plume Component and their children.
                This won't, however, register the listener for the top component (Which is the editor, that
                is not a PlumeComponent.. ).
    
        \param  listener                         The ActionListener object that will recieve alert
                                                 messages from the PlumeComponent objects.
        \param  topComponent                     Component that will have its children listened to.
        \param  searchPlumeComponentChildrenOnly If true, will only search for new PlumeComponent
                                                 objects among the children of the PlumeComponents
                                                 that were found. Set this to false if you have
                                                 standart Components that possess PlumeComponents.
                                                 Otherwise, keeping this to true saves needless
                                                 computation.
    */
    static void listenToAllChildrenPlumeComponents (Component* topComponent,
                                                    ActionListener* listener,
                                                    bool searchPlumeComponentChildrenOnly = true)
    {
        // This method call is pointless if the listener doesn't exist...
        jassert (listener != nullptr);

        if (topComponent == nullptr || listener == nullptr) return;

        for (Component* child : topComponent->getChildren())
        {
            // Child is PlumeComponent
            if (auto* plumeComp = dynamic_cast<PlumeComponent*> (child))
            {
                // Listens and search this component's children
                plumeComp->addActionListener (listener);
                listenToAllChildrenPlumeComponents (plumeComp, listener, searchPlumeComponentChildrenOnly);
            }
            
            // child is Component and we are searching Components' children
            if (!searchPlumeComponentChildrenOnly) 
            {
                listenToAllChildrenPlumeComponents (child, listener, searchPlumeComponentChildrenOnly);
            }
        }
    }
    
protected:
    /**
        \brief  Alert message sender.
    
                This method will send an action message describing an error or
                a warning directed to the user.
                In order for the method to work, the PlumeComponent must be Listened
                to by the InfoPanel object from the Editor.
    
    */
    void sendAlertMessage (const String& message) { sendActionMessage (message); }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeComponent)
};