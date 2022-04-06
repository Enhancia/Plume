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
    

    // transparent area
    auto gradTransp = ColourGradient::vertical (Colour (0x00000000),
                                                0.0f, 
                                                Colour (0x00000000),
                                                float(getHeight()));
    gradTransp.addColour (0.05, getPlumeColour (topPanelTransparentArea));
    gradTransp.addColour (0.95, getPlumeColour (topPanelTransparentArea));

    g.setGradientFill (gradTransp);
    g.fillRect (getLocalBounds());
    
    // panel area
    ColourGradient gradFill (getPlumeColour (topPanelBackground)
                                         .overlaidWith (Colour (0x10000000)),
                             float (panelArea.getCentreX()),
                             float (panelArea.getBottom()),
                             getPlumeColour (topPanelBackground),
                             float (panelArea.getCentreX()),
                             float (panelArea.getY()),
                             true);
    gradFill.addColour (0.7, getPlumeColour (topPanelBackground)
                                         .overlaidWith (Colour (0x10000000)));

    g.setGradientFill (gradFill);
    g.fillRoundedRectangle (panelArea.toFloat(), 10.0f);
    
    // panel outline
    auto gradOut = ColourGradient::horizontal (Colour (0x10ffffff),
                                               float(panelArea.getX()), 
                                               Colour (0x10ffffff),
                                               float(panelArea.getRight()));
    gradOut.addColour (0.5, Colour (0x50ffffff));

    g.setGradientFill (gradOut);
    g.drawRoundedRectangle (panelArea.toFloat(), 10.0f, 1.0f);
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
    const String bullet = " " + String::charToString (juce_wchar(0x2022));
    
	return "Gesture creation Panel:\n\n"
		   + bullet + " Select a gesture type and add a custom name and description.";
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
	if (bttn == closeButton.get())
	{
		hidePanel (true);
	}
}

bool NewGesturePanel::keyPressed (const KeyPress& key)
{
    if (key == PLUME::keyboard_shortcut::closeWindow)
    {
        hidePanel (true);
    }

    return true;
}

void NewGesturePanel::createNewGesture()
{
	processor.getGestureArray().addGesture (Gesture::getTypeString (selectedGestureType, true), selectedGestureType, selectedGestureSlot);
	updateGesturePanel();
}

void NewGesturePanel::labelTextChanged (Label*)
{
}
void NewGesturePanel::editorShown (Label*, TextEditor&)
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
    grabKeyboardFocus();
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
    addAndMakeVisible (*(closeButton = std::make_unique<ShapeButton> ("Close Settings Button", Colour(0x00000000),
                                                                               Colour(0x00000000),
                                                                               Colour(0x00000000))));

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
	addAndMakeVisible (*(descriptionTextEditor = std::make_unique<TextEditor> ("Gesture Description Text Editor")));
    descriptionTextEditor->setMultiLine (true, true);
    descriptionTextEditor->setReturnKeyStartsNewLine (true);
    descriptionTextEditor->setReadOnly (true);
    descriptionTextEditor->setScrollbarsShown (false);
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
	if (highlighted)
	{
		// Fill
		g.setColour (Gesture::getHighlightColour (gestureType).withAlpha (0.15f));
		g.fillRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 10.0f);

		// Outline
		g.setColour (Gesture::getHighlightColour (gestureType));
		g.drawRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 10.0f, 1.0f);
	}
	else
	{
        // Fill
		g.setColour (getPlumeColour (basePanelBackground));
		g.fillRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 10.0f);

        // Outline
        g.setColour (Gesture::getHighlightColour (gestureType).withAlpha (0.2f));
        g.drawRoundedRectangle (getLocalBounds().reduced (2).toFloat(), 10.0f, 1.0f);
	}

	drawGesturePath (g, getLocalBounds());
}
void NewGesturePanel::GestureTypeSelector::resized()
{
}

void NewGesturePanel::GestureTypeSelector::mouseEnter (const MouseEvent &)
{
	setHighlighted (true);
	repaint();
}
void NewGesturePanel::GestureTypeSelector::mouseExit (const MouseEvent &)
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

void NewGesturePanel::GestureTypeSelector::drawGesturePath (Graphics& g, juce::Rectangle<int> area)
{
    g.setColour (Colour (0xfff3f3f3));
    g.drawText (Gesture::getTypeString (gestureType, true), area,
               Justification::centred, true);

    Path gesturePath;
    switch (gestureType)
    {
        case (Gesture::tilt):
            gesturePath = PLUME::path::createPath (PLUME::path::tilt);
            break;

        case (Gesture::vibrato):
            gesturePath = PLUME::path::createPath (PLUME::path::vibrato);
            break;

        case (Gesture::pitchBend):
            gesturePath = PLUME::path::createPath (PLUME::path::pitchBend);
            break;

        case (Gesture::roll):
            gesturePath = PLUME::path::createPath (PLUME::path::roll);
            break;

        default:
            return;
    }


    gesturePath.scaleToFit (area.toFloat().getX(),
                            area.toFloat().getY(),
                            area.toFloat().getWidth(),
                            area.toFloat().getHeight(),
                        false);

    Colour pathColour (0xff808080);
    ColourGradient gesturePathGradient (pathColour.withAlpha (0.2f),
                                        {area.toFloat().getX(),
                                         area.toFloat().getY() + area.toFloat().getHeight()},
                                        pathColour.withAlpha (0.2f),
                                        {area.toFloat().getX() + area.toFloat().getWidth(),
                                         area.toFloat().getY()},
                                        false);

    gesturePathGradient.addColour (0.3, pathColour.withAlpha (0.0f));
    gesturePathGradient.addColour (0.7, pathColour.withAlpha (0.0f));

    Path pathClip;
    pathClip.addRoundedRectangle (area.toFloat(), 10.0f);
    g.setGradientFill (gesturePathGradient);

    g.saveState();
    g.reduceClipRegion (pathClip);
    g.strokePath (gesturePath, PathStrokeType (2.0f));
    g.restoreState();
}