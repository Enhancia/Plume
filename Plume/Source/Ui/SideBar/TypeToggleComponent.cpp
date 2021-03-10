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
        String text;
        
        switch (i)
        {

            case PlumePreset::defaultPreset:     text = "Factory";   break;
            case PlumePreset::userPreset:        text = "User";      break;
            case PlumePreset::communityPreset:   text = "Community"; break;
            default:                             text = "All";
        }
        
        toggles.add (new Toggle (i, text, getPlumeColour (presetTypeToggleSelected),
                                          getPlumeColour (presetsBoxRowTextHighlighted),
                                          getPlumeColour (presetTypeToggleUnselected),
                                          getPlumeColour (presetsBoxRowText)));

        addChildAndSetID (toggles.getLast(), String(i));
        toggles.getLast()->addMouseListener (this, false);
    }
    
    toggleButton (-1);
}

TypeToggleComponent::~TypeToggleComponent()
{
    ScopedLock tglock (togglesLock);
    
    for (int i=0; i<=PlumePreset::numTypes; i++)
    {
        toggles[i]->removeMouseListener (this);
    }
    
    toggles.clear();
}

void TypeToggleComponent::paint (Graphics& g)
{
}

void TypeToggleComponent::paintOverChildren (Graphics& g)
{
}

void TypeToggleComponent::resized()
{
    auto area = getLocalBounds();
    int buttonW = getWidth()/PlumePreset::numTypes;
    
    {
        ScopedLock tglock (togglesLock);
    
        for (int i=0; i<=PlumePreset::numTypes; i++)
        {
            toggles[i]->setBounds (area.removeFromLeft (buttonW).reduced (0, 1).withTrimmedLeft (1));
        }
    }
}

void TypeToggleComponent::buttonClicked (Button* bttn)
{
    toggleButton (bttn->getComponentID().getIntValue());
}

void TypeToggleComponent::mouseUp (const MouseEvent &event)
{
    if (auto* tggle = dynamic_cast<Toggle*> (event.eventComponent))
    {
        toggleButton (tggle->id);
    }
}

void TypeToggleComponent::toggleButton (int buttonId)
{
    {
        ScopedLock tglock (togglesLock);
    
        for (int i=0; i<=PlumePreset::numTypes; i++)
        {
            if      (i == buttonId + 1)            toggles[i]->setToggleState (true);
            else if (toggles[i]->getToggleState()) toggles[i]->setToggleState (false);
        }

        repaint();
    }
    
    processor.getPresetHandler().setTypeSearchSetting (buttonId);
    
	if (getParentComponent() != nullptr)
	{
		if (auto* presetBox = dynamic_cast<ListBox*> (getParentComponent() // presetComp
			                                              ->findChildWithID("presetBox")))
		{
			presetBox->updateContent();
            presetBox->selectRow (0);
            presetBox->scrollToEnsureRowIsOnscreen (0);
		}
	}
}

TypeToggleComponent::Toggle::Toggle (const int num, String txt,
                                     Colour onBG, Colour onTxt,
                                     Colour offBG, Colour offTxt)
    : id (num), text (txt),
      backgroundOnColour (onBG), textOnColour (onTxt),
      backgroundOffColour (offBG), textOffColour (offTxt)
{
}

TypeToggleComponent::Toggle::~Toggle()
{
}
        
void TypeToggleComponent::Toggle::paint (Graphics& g)
{
    g.setColour (state ? backgroundOnColour : backgroundOffColour);
    g.fillRect (getLocalBounds().withTop (getLocalBounds().getBottom() - 3));

    g.setColour (state ? textOnColour : textOffColour);
    g.setFont (PLUME::font::plumeFontBold.withHeight (PLUME::font::SIDEBAR_LABEL_FONT_H + 2.0f));
    g.drawText (text, getLocalBounds().reduced (0, PLUME::UI::MARGIN), Justification::centredBottom);

    // Highlight for mouse over
    if (!state && highlighted)
    {    
        g.fillAll (Colours::white.withAlpha (0.05f));
    }
}

void TypeToggleComponent::Toggle::resized()
{
}

void TypeToggleComponent::Toggle::mouseEnter (const MouseEvent &event)
{
    if (!state && !highlighted)
    {
        highlighted = true;
        repaint();
    }
}

void TypeToggleComponent::Toggle::mouseExit (const MouseEvent &event)
{
    if (!state && highlighted)
    {
        highlighted = false;
        repaint();
    }
}

void TypeToggleComponent::Toggle::setToggleState (bool newState)
{
    state = newState;
    highlighted = false;
    repaint();
}

bool TypeToggleComponent::Toggle::getToggleState()
{
    return state;
}