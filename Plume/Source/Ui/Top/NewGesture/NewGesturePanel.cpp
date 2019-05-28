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
	createCloseButton();
	createGestureSelectorButtons();
	createAndAddTextEditor();
}

NewGesturePanel::~NewGesturePanel()
{
	closeButton = nullptr;
	descriptionTextEditor = nullptr;
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
    auto gradOut = ColourGradient::horizontal (currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               float(panelArea.getX()), 
                                               currentTheme.getColour(PLUME::colour::sideBarSeparatorOut),
                                               float(panelArea.getRight()));
    gradOut.addColour (0.5, currentTheme.getColour(PLUME::colour::sideBarSeparatorIn));

    g.setGradientFill (gradOut);
    g.drawRect (panelArea);
}

void NewGesturePanel::resized()
{
	using namespace PLUME::UI;

    // Panel Area
    panelArea = getLocalBounds().reduced (getWidth()/6, getHeight()/4);

    closeButton->setBounds (juce::Rectangle<int> (15, 15).withX (panelArea.getRight() - MARGIN - 15)
                                                         .withY (panelArea.getY() + MARGIN)
                                                         .reduced (3));

	auto area = panelArea.reduced (2*MARGIN);

    resizeGestureSelectorButtons (area.removeFromTop (area.getHeight()/2)
    								  .reduced (MARGIN, 2*MARGIN));

    descriptionTextEditor->setBounds (area.reduced (2*MARGIN, MARGIN));
    if (area.getHeight() > 80)
    	descriptionTextEditor->setBounds (area.withSizeKeepingCentre (area.reduced (2*MARGIN, MARGIN).getWidth(),
    																  80));
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
}

//==============================================================================
// Callbacks
void NewGesturePanel::visibilityChanged()
{
}

void NewGesturePanel::mouseEnter (const MouseEvent &event)
{
	if (auto* gestureSelector = dynamic_cast<GestureTypeSelector*> (event.eventComponent))
	{
		descriptionTextEditor->setText ("", false);

		// Gesture Type
		descriptionTextEditor->setFont (PLUME::font::plumeFontBold.withHeight (15.0f));
		descriptionTextEditor->insertTextAtCaret (Gesture::getTypeString (gestureSelector->gestureType, true)
												  	  + " :\n");
		// Gesture Description
		descriptionTextEditor->setFont (PLUME::font::plumeFontBook.withHeight (14.0f));
		descriptionTextEditor->insertTextAtCaret (Gesture::getGestureTypeDescription (gestureSelector->gestureType));
	
		descriptionTextEditor->setCaretPosition (0);
	}
}
void NewGesturePanel::mouseUp (const MouseEvent &event)
{
	if (auto* gestureSelector = dynamic_cast<GestureTypeSelector*> (event.eventComponent))
	{
		selectGestureType (gestureSelector);
		createNewGesture();
		gestureSelector->setHighlighted (false);
	}
}
void NewGesturePanel::buttonClicked (Button* bttn)
{
	if (bttn == closeButton)
	{
		hidePanel (true);
	}
}

void NewGesturePanel::createNewGesture()
{
	processor.getGestureArray().addGesture (Gesture::getTypeString (selectedGestureType, true), selectedGestureType, selectedGestureSlot);
	updateGesturePanel();
}

void NewGesturePanel::labelTextChanged (Label* lbl)
{
}
void NewGesturePanel::editorShown (Label* lbl, TextEditor& ed)
{
}

//==============================================================================
void NewGesturePanel::showPanelForGestureID (const int gestureID)
{
	if (gestureID < 0 || gestureID >= PLUME::NUM_GEST)
	{
		// Plume tries to create a gesture for an id that can't exist
		jassertfalse;
		return;
	}
	else if (processor.getGestureArray().getGesture (gestureID) != nullptr)
	{
		// Plume tries to create a gesture for an id that already has a gesture!!
		jassertfalse;
		return;
	}

    selectedGestureSlot = gestureID;
    update();

    setVisible (true);
}

void NewGesturePanel::hidePanel (const bool resetSelectedSlot)
{
    if (resetSelectedSlot) selectedGestureSlot = -1;

    unselectGestureType();
    setVisible (false);
}

void NewGesturePanel::updateGesturePanel()
{
	if (auto* gesturePanel = dynamic_cast<PlumeComponent*> (getParentComponent()->findChildWithID ("gesturePanel")))
	{
		gesturePanel->update();
	}
}

const int NewGesturePanel::getLastSelectedSlot()
{
	return selectedGestureSlot;
}

void NewGesturePanel::setVisible (bool shouldBeVisible)
{
	Component::setVisible (shouldBeVisible);
}

void NewGesturePanel::selectGestureType (const GestureTypeSelector* gestureTypeToSelect)
{
	selectedGestureType = gestureTypeToSelect->gestureType;
}
void NewGesturePanel::unselectGestureType()
{
	selectedGestureType = -1;
}

//==============================================================================
void NewGesturePanel::createCloseButton()
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

void NewGesturePanel::createAndAddTextEditor()
{
	addAndMakeVisible (descriptionTextEditor = new TextEditor ("Gesture Description Text Editor"));
    descriptionTextEditor->setMultiLine (true, true);
    descriptionTextEditor->setReturnKeyStartsNewLine (true);
    descriptionTextEditor->setReadOnly (true);
    descriptionTextEditor->setScrollbarsShown (true);
    descriptionTextEditor->setPopupMenuEnabled (false);
    descriptionTextEditor->applyColourToAllText (PLUME::UI::currentTheme.getColour (PLUME::colour::sideBarMainText), true);
    descriptionTextEditor->setJustification (Justification::centred);
    descriptionTextEditor->setFont (PLUME::font::plumeFontBook.withHeight (13.0f));
    descriptionTextEditor->setMouseCursor (MouseCursor (MouseCursor::NormalCursor));
    descriptionTextEditor->setInterceptsMouseClicks (false, false);
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
	return String();
}
void NewGesturePanel::GestureTypeSelector::update()
{
}

void NewGesturePanel::GestureTypeSelector::paint (Graphics& g)
{
	g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText));
	g.drawRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 2.0f, 1.0f);

	if (highlighted)
	{
		g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText)
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
	setHighlighted (true);
	repaint();
}
void NewGesturePanel::GestureTypeSelector::mouseExit (const MouseEvent &event)
{
	setHighlighted (false);
	repaint();
}

void NewGesturePanel::GestureTypeSelector::setHighlighted (bool shouldBeHighlighted)
{
	if (shouldBeHighlighted != highlighted)
	{
		highlighted = shouldBeHighlighted;
		repaint();
	}
}