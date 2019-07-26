/*
  ==============================================================================

    MidiModeComponent.cpp
    Created: 14 Jun 2019 2:33:50pm
    Author:  Alex

  ==============================================================================
*/

#include "MidiModeComponent.h"

#ifndef W 
#define W Component::getWidth()
#endif

#ifndef H 
#define H Component::getHeight()
#endif

MidiModeComponent::MidiModeComponent (Gesture& gest, GestureArray& gestArray)
    : gesture (gest), gestureArray (gestArray)
{
    createComboBox();
    createLabels();
}

MidiModeComponent::~MidiModeComponent()
{
    midiTypeBox->removeListener (this);
    ccLabel->removeListener (this);
    rangeLabelMin->removeListener (this);
    rangeLabelMax->removeListener (this);
    
    midiTypeBox = nullptr;
    ccLabel = nullptr;
    rangeLabelMin = nullptr;
    rangeLabelMax = nullptr;
}

//==============================================================================
void MidiModeComponent::paint (Graphics& g)
{
    auto area = getLocalBounds().reduced (PLUME::UI::MARGIN)
                                .withTrimmedLeft (PLUME::UI::MARGIN);
    area.removeFromRight (getWidth()*2/3);

    g.setColour (getPlumeColour (detailPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight (13.0f));

    g.drawText ("MIDI Type:",
                area.removeFromTop (area.getHeight()/2),
                Justification::centredLeft, false);

    g.drawText ("MIDI Range:",
                area,
                Justification::centredLeft, false);
}

void MidiModeComponent::resized()
{
    using namespace PLUME::UI;

	auto area = getLocalBounds().reduced (MARGIN);
	area.removeFromLeft (area.getWidth()/3);

    auto typeArea = area.removeFromTop (getHeight()/2);
    midiTypeBox->setBounds (typeArea.removeFromLeft (typeArea.getWidth()/2)
    								.withSizeKeepingCentre (area.getWidth()*4/10, 20));
    ccLabel->setBounds (typeArea.withSizeKeepingCentre (area.getWidth()/4, 20));

    auto rangeArea = area;
    rangeLabelMin->setBounds (rangeArea.removeFromLeft (rangeArea.getWidth()/2)
    								.withSizeKeepingCentre (area.getWidth()/4, 20));
    rangeLabelMax->setBounds (rangeArea.withSizeKeepingCentre (area.getWidth()/4, 20));
}

//==============================================================================
void MidiModeComponent::labelTextChanged (Label* lbl)
{
    if (lbl == ccLabel)
    {
        // checks that the string is numbers only
        if (lbl->getText().containsOnly ("0123456789") == false)
        {
            lbl->setText (String (gesture.getCc()), dontSendNotification);
            return;
        }

        int val = lbl->getText().getIntValue();
    
        if (val < 0) val = 0;
        else if (val > 127) val = 127;
    
        gesture.setCc(val);

        lbl->setText (String(val), dontSendNotification);

        if (auto* parentComp = getParentComponent())
        	parentComp->repaint();
    }
    
    else if (lbl == rangeLabelMin || lbl == rangeLabelMax)
    {
        // checks that the string is numbers only (and dot)
        if (lbl->getText().containsOnly ("-.0123456789") == false)
        {
            if (lbl == rangeLabelMin)       lbl->setText (String (gesture.midiLow.getValue(), 2), dontSendNotification);
            else if (lbl == rangeLabelMax)  lbl->setText (String (gesture.midiHigh.getValue(), 2), dontSendNotification);

            return;
        }
    
        // Gets the float value of the text 
        float val = lbl->getText().getFloatValue();
    
        if (val < 0.0f)      val = 0.0f;
        else if (val > 1.0f) val = 1.0f;
    
        // Sets slider and labels accordingly
        if (lbl == rangeLabelMin)
        {
            // Min > Max
            if ( val > gesture.midiHigh.getValue()) val = gesture.midiHigh.getValue();
        
            // Normal case
            gesture.setMidiLow (val);
            lbl->setText (String (gesture.midiLow.getValue(), 2), dontSendNotification);
        }
        else if (lbl == rangeLabelMax)
        {
            // Max < Min
            if ( val < gesture.midiLow.getValue()) val = gesture.midiLow.getValue();
        
            // Normal case
            gesture.setMidiHigh  (val);
            lbl->setText (String (gesture.midiHigh.getValue(), 2), dontSendNotification);
        }
    }
}

void MidiModeComponent::comboBoxChanged (ComboBox* box)
{
    if (box == midiTypeBox)
    {
        bool isCC = (midiTypeBox->getSelectedId() == Gesture::controlChange);
        
        // cc Label is visible & editable only if "CC" is selected
        ccLabel->setEditable (isCC, false, false);
        ccLabel->setVisible (isCC);
        
        // Affects the gesture's midiType variable
        gesture.midiType = midiTypeBox->getSelectedId();
        gestureArray.checkPitchMerging();

        if (auto* parentComp = getParentComponent())
        	parentComp->repaint();
    }
}

void MidiModeComponent::updateComponents()
{
    ccLabel->setText (String (gesture.getCc()), dontSendNotification);
    rangeLabelMin->setText (String (gesture.midiLow.getValue(), 2), dontSendNotification);
	rangeLabelMax->setText (String (gesture.midiHigh.getValue(), 2), dontSendNotification);
}

//==============================================================================
void MidiModeComponent::createComboBox()
{
    addAndMakeVisible (midiTypeBox = new ComboBox ("midiTypeBox"));
    midiTypeBox->addItem ("CC", Gesture::controlChange);
    midiTypeBox->addItem ("Pitch", Gesture::pitch);
    //midiTypeBox->addItem ("AfterTouch", Gesture::afterTouch);
    midiTypeBox->setSelectedId (gesture.midiType, dontSendNotification);
    
    // ComboBox look
    midiTypeBox->setJustificationType (Justification::centred);
    midiTypeBox->setColour (ComboBox::outlineColourId, getPlumeColour (detailPanelSubText));
    midiTypeBox->setColour (ComboBox::textColourId, getPlumeColour (detailPanelMainText));
    midiTypeBox->setColour (ComboBox::arrowColourId, getPlumeColour (detailPanelSubText));

    if (gesture.type == Gesture::vibrato || gesture.type == Gesture::pitchBend)
    {
    	midiTypeBox->setAlpha (0.3f);
    	midiTypeBox->setInterceptsMouseClicks (false, false);
    }
    
    midiTypeBox->addListener (this);
}

void MidiModeComponent::createLabels()
{
    //=== Midi Type label ===
    
    // CC label
    addAndMakeVisible (ccLabel = new Label ("CC Label", TRANS (String(gesture.getCc()))));
    ccLabel->setEditable ((midiTypeBox->getSelectedId() == Gesture::controlChange), false, false);
    ccLabel->setFont (PLUME::font::plumeFont.withHeight (13.0f));
    ccLabel->setJustificationType (Justification::centred);

    ccLabel->setColour (Label::backgroundColourId , Colour (0));
    ccLabel->setColour (Label::textColourId, getPlumeColour (detailPanelMainText));
    ccLabel->setColour (Label::outlineColourId, getPlumeColour (detailPanelSubText));
    
    // cc Label is visible & editable only if "CC" is selected
    ccLabel->setEditable (midiTypeBox->getSelectedId() == Gesture::controlChange, false, false);
    ccLabel->setVisible (midiTypeBox->getSelectedId() == Gesture::controlChange);
    
    ccLabel->addListener (this);
    
    //=== range Control labels ===

    addAndMakeVisible (rangeLabelMin = new Label ("Min Label", TRANS (String (gesture.midiLow.getValue(), 2))));
    addAndMakeVisible (rangeLabelMax = new Label ("Max Label", TRANS (String (gesture.midiHigh.getValue(), 2))));
    
    // LabelMin style
    rangeLabelMin->setEditable (true, false, false);
    rangeLabelMin->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    rangeLabelMin->setJustificationType (Justification::centred);
    rangeLabelMin->setColour (Label::backgroundColourId , Colour (0));
    rangeLabelMin->setColour (Label::textColourId, getPlumeColour (detailPanelMainText));
    rangeLabelMin->setColour (Label::outlineColourId, getPlumeColour (detailPanelSubText));
    
    // LabelMax style
    rangeLabelMax->setEditable (true, false, false);
    rangeLabelMax->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    rangeLabelMax->setJustificationType (Justification::centred);
    rangeLabelMax->setColour (Label::backgroundColourId , Colour (0));
    rangeLabelMax->setColour (Label::textColourId, getPlumeColour (detailPanelMainText));
    rangeLabelMax->setColour (Label::outlineColourId, getPlumeColour (detailPanelSubText));
    
    
    rangeLabelMin->addListener (this);
    rangeLabelMax->addListener (this);
}

//==============================================================================
// Midi Banner

MidiBanner::MidiBanner (Gesture& gest)	: gesture (gest)
{
}
MidiBanner::~MidiBanner()
{
}
void MidiBanner::paint (Graphics& g)
{
    String midiString = (gesture.midiType == Gesture::controlChange ?
    				    	"CC " + String (gesture.getCc()) :
    						(gesture.midiType == Gesture::pitch ? "Pitch" : "Unknown" ));

    g.setColour (getPlumeColour (detailPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight(15));
	g.drawText ("MIDI", getLocalBounds().withWidth (getWidth()/4),
                            Justification::centred);

    g.setColour (getPlumeColour (detailPanelMainText));
    g.setFont (PLUME::font::plumeFontBold.withHeight(13));
    g.drawText (midiString, getLocalBounds().withSizeKeepingCentre (getWidth()/2, getHeight()),
                            Justification::centred);
}
void MidiBanner::resized()
{
}