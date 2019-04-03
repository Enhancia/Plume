/*
  ==============================================================================

    NewGesturePanel.cpp
    Created: 28 Mar 2019 11:10:16am
    Author:  Alex

  ==============================================================================
*/

#include "NewGesturePanel.h"

NewGesturePanel::NewGesturePanel (PlumeProcessor& proc) : processor (proc)
{
	addAndMakeVisible (gestureNameLabel = new Label ("gestureNameLabel", "Gesture Name..."));
    gestureNameLabel->setColour (Label::backgroundColourId, Colour (0x30000000));
    gestureNameLabel->setColour (Label::textColourId, PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText)
                                                                      .withAlpha (0.6f));
    gestureNameLabel->setFont (PLUME::font::plumeFont.withHeight (14.0f));
    gestureNameLabel->setEditable (true, false, false);
    gestureNameLabel->setMouseCursor (MouseCursor (MouseCursor::IBeamCursor));
    gestureNameLabel->addListener (this);

	createAndAddButtons();
	createGestureSelectorButtons();
}

NewGesturePanel::~NewGesturePanel()
{
	gestureNameLabel = nullptr;
	closeButton = nullptr;
	addGestureButton = nullptr;
}

//==============================================================================
// Component Methods
void NewGesturePanel::paint (Graphics& g)
{
	using namespace PLUME::UI;
    
    ColourGradient gradFill (currentTheme.getColour (PLUME::colour::topPanelBackground)
                                         .overlaidWith (Colour (0x10000000)),
                             float (panelArea.getCentreX()),
                             float (panelArea.getBottom()),
                             currentTheme.getColour (PLUME::colour::topPanelBackground),
                             float (panelArea.getCentreX()),
                             float (panelArea.getY()),
                             true);
    gradFill.addColour (0.7, currentTheme.getColour (PLUME::colour::topPanelBackground)
                                         .overlaidWith (Colour (0x10000000)));

    // transparent area
    g.setColour (currentTheme.getColour (PLUME::colour::topPanelTransparentArea));
    g.fillRect (getLocalBounds());
    
    // panel area
    g.setGradientFill (gradFill);
    g.fillRect (panelArea);
    
    // panel outline
    auto gradOut = ColourGradient::horizontal (currentTheme.getColour(colour::sideBarSeparatorOut),
                                               float(panelArea.getX()), 
                                               currentTheme.getColour(colour::sideBarSeparatorOut),
                                               float(panelArea.getRight()));
    gradOut.addColour (0.5, currentTheme.getColour(colour::sideBarSeparatorIn));

    g.setGradientFill (gradOut);
    g.drawRect (panelArea);

    auto area = panelArea.reduced (2*MARGIN);

    area.removeFromTop (panelArea.getHeight()/3); // GestureSelectors area
    area.removeFromTop (panelArea.getHeight()*2/9); // Gesture Description area

    g.setColour (currentTheme.getColour (PLUME::colour::topPanelMainText));
    g.setFont (PLUME::font::plumeFont.withHeight (14.0f));
    g.drawText ("Name:", area.removeFromTop (panelArea.getHeight()*2/9)
    						 .reduced (4*MARGIN, 0)
    						 .withWidth (panelArea.getWidth()/4),
    				    Justification::centredLeft,
    				    false);
}

void NewGesturePanel::resized()
{
	using namespace PLUME::UI;

    // Panel Area
    panelArea = getLocalBounds().reduced (getWidth()/6, getHeight()/6);

    closeButton->setBounds (juce::Rectangle<int> (15, 15).withX (panelArea.getRight() - MARGIN - 15)
                                                         .withY (panelArea.getY() + MARGIN)
                                                         .reduced (3));

	auto area = panelArea.reduced (2*MARGIN);

    resizeGestureSelectorButtons (area.removeFromTop (panelArea.getHeight()/3)
    								  .reduced (MARGIN));

    area.removeFromTop (panelArea.getHeight()*2/9); // Gesture Description area

 	gestureNameLabel->setBounds (area.removeFromTop (panelArea.getHeight()*2/9)
 									 .withSizeKeepingCentre (area.getWidth(), OPTIONS_HEIGHT)
 									 .reduced (4*MARGIN, 0)
 									 .withTrimmedLeft (panelArea.getWidth()/4));

    addGestureButton->setBounds (area.removeFromTop (panelArea.getHeight()*2/9)
 									 .withSizeKeepingCentre (panelArea.getWidth()/3, OPTIONS_HEIGHT));
}

//==============================================================================
// PlumeComponent Methods
const String NewGesturePanel::getInfoString()
{
	return "Gesture creation Panel:\n\n"
		   "- Select a gesture type and add a custom name and description.";
}
void NewGesturePanel::update()
{
	gestureNameLabel->setText ("Gesture Name...", sendNotification);
}

//==============================================================================
// Callbacks
void NewGesturePanel::visibilityChanged()
{
}
void NewGesturePanel::mouseUp (const MouseEvent &event)
{
	if (auto* gestureSelector = dynamic_cast<GestureTypeSelector*> (event.eventComponent))
	{
		selectGestureTypeExclusive (gestureSelector);
	}
}
void NewGesturePanel::buttonClicked (Button* bttn)
{
	if (bttn == closeButton)
	{
		setVisible (false);
	}

	else if (bttn == addGestureButton)
	{
		if (selectedGestureType != -1 && gestureNameLabel->getText() != "New Gesture...")
		{
			// creates gesture and closes panel
			processor.getGestureArray().addGesture (gestureNameLabel->getText(),
													selectedGestureType,
													selectedGestureSlot);
			updateGesturePanel();
			hidePanel ();
		}
	}
}

void NewGesturePanel::labelTextChanged (Label* lbl)
{
	if (lbl == gestureNameLabel)
    {
        if (lbl->getText().isEmpty())
        {
            lbl->setText ("Gesture Name...", dontSendNotification);
            lbl->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText)
                                                                 .withAlpha (0.6f));
        }
        else
        {
            lbl->setColour (Label::textColourId, UI::currentTheme.getColour (colour::topPanelMainText));
        }
    }
}
void NewGesturePanel::editorShown (Label* lbl, TextEditor& ed)
{
	if (lbl == gestureNameLabel && lbl->getText() == "Gesture Name...")
    {
        lbl->setColour (Label::outlineColourId, Colour (0x000000));
        
        // The user doesn't have to manually remove "Gesture Name :"
        ed.setText ("", false);
    }
}

//==============================================================================
void NewGesturePanel::showPanelForGestureID (const int gestureID)
{
	if (gestureID < 0 || gestureID >= NUM_GEST)
	{
		// Plume tries to create a gesture for an id that can't exist
		jassertfalse;
		return;
	}
	else if (processor.getGestureArray().getGestureById (gestureID) != nullptr)
	{
		// Plume tries to create a gesture for an id that already has a gesture!!
		jassertfalse;
		return;
	}

    selectedGestureSlot = gestureID;
    update();

    setVisible (true);
}

void NewGesturePanel::hidePanel()
{
    selectedGestureType = -1;
    selectedGestureSlot = -1;
    setVisible (false);
}

void NewGesturePanel::updateGesturePanel()
{
	if (auto* gesturePanel = dynamic_cast<PlumeComponent*> (getParentComponent()->findChildWithID ("gesturePanel")))
	{
		gesturePanel->update();
	}
}

void NewGesturePanel::setVisible (bool shouldBeVisible)
{
	Component::setVisible (shouldBeVisible);
}

void NewGesturePanel::selectGestureTypeExclusive (GestureTypeSelector* gestureTypeToSelect)
{
	if (gestureTypeToSelect->isSelected()) return;

	for (auto* typeSelector : gestureSelectors)
	{
		if (typeSelector->isSelected())
			typeSelector->setSelected (false);
	}

	gestureTypeToSelect->setSelected (true);
	selectedGestureType = gestureTypeToSelect->gestureType;
}
void NewGesturePanel::unselectGestureType()
{
	gestureSelectors[selectedGestureType]->setSelected (false);
	selectedGestureType = -1;
}

//==============================================================================
void NewGesturePanel::createAndAddButtons()
{
    addAndMakeVisible (closeButton = new ShapeButton ("Close Settings Button", Colour(0x00000000),
                                                                               Colour(0x00000000),
                                                                               Colour(0x00000000)));

    Path p;
    p.startNewSubPath (0, 0);
    p.lineTo (3*PLUME::UI::MARGIN, 3*PLUME::UI::MARGIN);
    p.startNewSubPath (0, 3*PLUME::UI::MARGIN);
    p.lineTo (3*PLUME::UI::MARGIN, 0);

    closeButton->setShape (p, false, true, false);
    closeButton->setOutline (Colour (0xffffffff), 1.0f);
    closeButton->addMouseListener (this, false);
    closeButton->addListener (this);

    addAndMakeVisible (addGestureButton = new TextButton ("addGestureButton"));
    addGestureButton->setButtonText ("Add Gesture");
    addGestureButton->addListener (this);
}

void NewGesturePanel::createGestureSelectorButtons()
{
	jassert (gestureSelectors.isEmpty());

	for (int i=0; i < Gesture::numGestureTypes; i++)
	{
		gestureSelectors.add (new GestureTypeSelector (i));
		addAndMakeVisible (gestureSelectors.getLast());
		gestureSelectors.getLast()->addMouseListener (this, false);
	}
}
void NewGesturePanel::resizeGestureSelectorButtons (juce::Rectangle<int> buttonsArea)
{
	using namespace PLUME::UI;
	int N = Gesture::numGestureTypes; 

	int selectorWidth = (buttonsArea.getWidth() - (N - 1 - 1)*MARGIN) / (N - 1); // TODO WAVE remove -1 when wave is implemented

	for (int i=0; i < N; i++)
	{
		if (i == (int) Gesture::wave) continue; // TODO WAVE remove when wave is implemented

		gestureSelectors[i]->setBounds (buttonsArea.removeFromLeft (selectorWidth));
		buttonsArea.removeFromLeft (MARGIN);
	}
}

//==============================================================================
// GestureTypeSelector

NewGesturePanel::GestureTypeSelector::GestureTypeSelector (int gestType) : gestureType (gestType)
{
}
NewGesturePanel::GestureTypeSelector::~GestureTypeSelector()
{
}

const String NewGesturePanel::GestureTypeSelector::getInfoString()
{
	return "Add a " + Gesture::getTypeString (gestureType, true);
}
void NewGesturePanel::GestureTypeSelector::update()
{
}

void NewGesturePanel::GestureTypeSelector::paint (Graphics& g)
{
	g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText));
	g.drawRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 2.0f, 1.0f);

	if (highlighted || selected)
	{
		g.setColour (selected ? PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText)
											           .withAlpha (0.3f)
							  : PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText)
											           .withAlpha (0.1f));

		g.fillRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 2.0f);
	}

	g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText));
	g.setFont (PLUME::font::plumeFontLight.withHeight (15.0f));
	g.drawText (Gesture::getTypeString (gestureType, true), getLocalBounds(), Justification::centred, false);
}
void NewGesturePanel::GestureTypeSelector::resized()
{
}

void NewGesturePanel::GestureTypeSelector::mouseEnter (const MouseEvent &event)
{
	highlighted = true;
	repaint();
}
void NewGesturePanel::GestureTypeSelector::mouseExit (const MouseEvent &event)
{
	highlighted = false;
	repaint();
}

bool NewGesturePanel::GestureTypeSelector::isSelected()
{
	return selected;
}
void NewGesturePanel::GestureTypeSelector::setSelected (bool shouldBeSelected)
{
	if (shouldBeSelected != selected)
	{
		selected = shouldBeSelected;
		repaint();
	}
}