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
        
        toggles.add (new Toggle (i, text, getPlumeColour (presetsBoxRowBackgroundHighlighted),
                                          getPlumeColour (presetsBoxRowTextHighlighted),
                                          getPlumeColour (presetsBoxRowBackground),
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
    using namespace PLUME::UI;
    
    // Fills the component's inside
    auto gradIn = ColourGradient::vertical (Colour (0x30000000),
                                            0.0f, 
                                            Colour (0x20000000),
                                            float(getHeight()));
                                          
    gradIn.addColour (0.2, Colour (0x15000000));
    g.setGradientFill (gradIn);
    g.fillRect (1, 1, getWidth()-2, getHeight()-2);
}

void TypeToggleComponent::paintOverChildren (Graphics& g)
{
	using namespace PLUME::UI;

    //Gradient for the box's outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               float(MARGIN), 
                                               Colour (0x10ffffff),
                                               float(getWidth() - MARGIN));
    gradOut.addColour (0.5, Colour (0x50ffffff));
    g.setGradientFill (gradOut);

    // Outline
    g.drawRect (getLocalBounds());

    // Separators
    {
        ScopedLock tglock (togglesLock);
    
        auto area = getLocalBounds();
        int buttonW = getWidth()/PlumePreset::numTypes;

        for (int i=0; i<=PlumePreset::numTypes; i++)
        {
            if (i != PlumePreset::numTypes)
            {
                area.removeFromLeft (buttonW);
                g.drawVerticalLine (area.getX(), 1.0f, float(getHeight())-1.0f);
            }
        }
    }
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
            presetBox->repaint();
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
    g.fillAll (state ? backgroundOnColour : backgroundOffColour);

    g.setColour (state ? textOnColour : textOffColour);
    g.setFont (PLUME::font::plumeFont.withHeight (PLUME::font::SIDEBAR_LABEL_FONT_H));
    g.drawText (text, getLocalBounds(), Justification::centred);

    // Highlight for mouse over
    if (!state && highlighted)
    {    
        g.fillAll (backgroundOnColour.withAlpha (0.1f));
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