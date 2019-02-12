/*
  ==============================================================================

    TypeToggleComponent.cpp
    Created: 8 Feb 2019 11:25:26am
    Author:  Alex

  ==============================================================================
*/

#include "TypeToggleComponent.h"

//==============================================================================
TypeToggleComponent::TypeToggleComponent (PlumeProcessor& p) : processor (p)
{
    ScopedLock tglock (togglesLock);
    
    for (int i=-1; i<PlumePreset::numTypes; i++)
    {
        toggles.add (new TextButton ("toggle"+String(i)));
        
        switch (i)
        {
            case PlumePreset::defaultPreset:     toggles.getLast()->setButtonText ("Factory");   break;
            case PlumePreset::userPreset:        toggles.getLast()->setButtonText ("User");      break;
            case PlumePreset::communityPreset:   toggles.getLast()->setButtonText ("Community"); break;
            default:                             toggles.getLast()->setButtonText ("All");
        }
        
        addChildAndSetID (toggles.getLast(), String(i));
        toggles.getLast()->addListener (this);
        toggles.getLast()->setColour (TextButton::buttonColourId, Colour (0x00000000));
        toggles.getLast()->setColour (TextButton::buttonOnColourId, Colour (0x00000000));
        toggles.getLast()->setColour (TextButton::textColourOffId,
                                      PLUME::UI::currentTheme.getColour (colour::presetsBoxStandartText));
        toggles.getLast()->setColour (TextButton::textColourOnId,
                                      PLUME::UI::currentTheme.getColour (colour::presetsBoxHighlightedText));
    }
    
    toggleButton (-1);
}

TypeToggleComponent::~TypeToggleComponent()
{
    ScopedLock tglock (togglesLock);
    
    for (int i=0; i<=PlumePreset::numTypes; i++)
    {
        toggles[i]->removeListener (this);
    }
    
    toggles.clear();
}

void TypeToggleComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    
    auto area = getLocalBounds();
    int buttonW = getWidth()/PlumePreset::numTypes;
    
    // Fills the component's inside
    auto gradIn = ColourGradient::vertical (Colour (0x30000000),
                                            0, 
                                            Colour (0x20000000),
                                            getHeight());
                                          
    gradIn.addColour (0.2, Colour (0x15000000));
    g.setGradientFill (gradIn);
    g.fillRect (1, 1, getWidth()-2, getHeight()-2);
    
	//Gradient for the box's outline
	auto gradOut = ColourGradient::horizontal(currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
		                                      MARGIN,
		                                      currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
		                                      getWidth() - PLUME::UI::MARGIN);
	gradOut.addColour(0.5, currentTheme.getColour(PLUME::colour::sideBarSeparatorIn));
	
	{
        ScopedLock tglock (togglesLock);
    
        for (int i=0; i<=PlumePreset::numTypes; i++)
        {
            // Toggled Button button fill
            if (toggles[i]->getToggleState())
            {
				g.setColour (Colour (currentTheme.getColour (colour::presetsBoxHighlightedBackground)));
                g.fillRect (area.removeFromLeft (buttonW).withTrimmedLeft (1).reduced (0, 1));
            }
            else
            {
                area.removeFromLeft (buttonW);
            }
            
            // Separators
            if (i != PlumePreset::numTypes)
            {
				g.setGradientFill (gradOut);
                g.drawVerticalLine (area.getX(), 1, getHeight()-1);
            }
        }
    }
}

void TypeToggleComponent::paintOverChildren (Graphics& g)
{
	using namespace PLUME::UI;

    //Gradient for the box's outline
    auto gradOut = ColourGradient::horizontal (currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               MARGIN, 
                                               currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               getWidth() - MARGIN);
    gradOut.addColour (0.5, currentTheme.getColour(PLUME::colour::sideBarSeparatorIn));
    g.setGradientFill (gradOut);
    
	g.drawRect (getLocalBounds());
}

void TypeToggleComponent::resized()
{
    auto area = getLocalBounds();
    int buttonW = getWidth()/PlumePreset::numTypes;
    
    {
        ScopedLock tglock (togglesLock);
    
        for (int i=0; i<=PlumePreset::numTypes; i++)
        {
            toggles[i]->setBounds (area.removeFromLeft (buttonW));
        }
    }
}

void TypeToggleComponent::buttonClicked (Button* bttn)
{
    toggleButton (bttn->getComponentID().getIntValue());
}

void TypeToggleComponent::toggleButton (int buttonId)
{
    {
        ScopedLock tglock (togglesLock);
    
        for (int i=0; i<=PlumePreset::numTypes; i++)
        {
            if      (i == buttonId + 1)            toggles[i]->setToggleState (true, dontSendNotification);
            else if (toggles[i]->getToggleState()) toggles[i]->setToggleState (false, dontSendNotification);
        }
    }
    
    processor.getPresetHandler().setTypeSearchSetting (buttonId);
    
	if (getParentComponent() != nullptr)
	{
		if (auto* presetBox = dynamic_cast<ListBox*> (getParentComponent() // presetComp
			                                              ->findChildWithID("presetBox")))
		{
			presetBox->updateContent();
		}
	}
}