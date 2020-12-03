/*
  ==============================================================================

    TypeToggleComponent.h
    Created: 8 Feb 2019 11:25:26am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class TypeToggleComponent    : public Component,
                               private Button::Listener
{
public:
    //==============================================================================
    explicit TypeToggleComponent (PlumeProcessor& p);
    ~TypeToggleComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void paintOverChildren (Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    void buttonClicked (Button* bttn) override;
    void mouseUp (const MouseEvent &event) override;

private:
    //==============================================================================
    void toggleButton (int buttonId);

	class Toggle : public Component
	{
	public:
		Toggle(const int num, String text, Colour onBG, Colour onTxt, Colour offBG, Colour offTxt);
		~Toggle();

		void paint(Graphics& g) override;
		void resized() override;
		void mouseEnter(const MouseEvent &event) override;
		void mouseExit(const MouseEvent &event) override;
		void setToggleState (bool newState);
		bool getToggleState();

        const int id;
	private:
		const String text;
		const Colour backgroundOnColour, textOnColour, backgroundOffColour, textOffColour;

		bool state = false;
		bool highlighted = false;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Toggle)
	};
    
    //==============================================================================
    PlumeProcessor& processor;
    OwnedArray<Toggle> toggles;
    CriticalSection togglesLock;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TypeToggleComponent)
};
