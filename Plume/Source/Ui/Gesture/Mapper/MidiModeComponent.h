/*
  ==============================================================================

    MidiModeComponent.h
    Created: 17 Oct 2018 2:08:20pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"

#define W getWidth()
#define H getHeight()

//==============================================================================
/*
*/
class MidiModeComponent    : public Component,
                             private Label::Listener,
                             private ComboBox::Listener
{
public:
    MidiModeComponent(Gesture& gest)    : gesture (gest)
    {
        createComboBox();
        createLabels();
    }

    ~MidiModeComponent()
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
    void paint (Graphics& g) override
    {
    }

    void resized() override
    {
        midiTypeBox->setBounds (4 , H/4 + 1, W - 8, H/4 - 2);
        ccLabel->setBounds (W/4 , H/2 + 1, W/2, H/4 - 2);
	    rangeLabelMin->setBounds (2, H*3/4 + 2, W/2 - 4, H/4 - 4);
	    rangeLabelMax->setBounds (W/2 + 2, H*3/4 + 2, W/2 - 4, H/4 - 4);
    }

    //==============================================================================
    void labelTextChanged (Label* lbl)
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
    
    void comboBoxChanged (ComboBox* box)
    {
        if (box == midiTypeBox)
        {
            bool isCC = (midiTypeBox->getSelectedId() == Gesture::controlChange);
            
            // cc Label is visible & editable only if "CC" is selected
            ccLabel->setEditable (isCC, false, false);
            ccLabel->setVisible (isCC);
            
            // Affects the gesture's midiType variable
            gesture.midiType = midiTypeBox->getSelectedId();
        }
    }
    
    void updateComponents()
    {
        ccLabel->setText (String (gesture.getCc()), dontSendNotification);
        rangeLabelMin->setText (String (gesture.midiLow.getValue(), 2), dontSendNotification);
		rangeLabelMax->setText (String (gesture.midiHigh.getValue(), 2), dontSendNotification);
        
    }

private:
    //==============================================================================
    void createComboBox()
    {
        addAndMakeVisible (midiTypeBox = new ComboBox ("midiTypeBox"));
        midiTypeBox->addItem ("CC", Gesture::controlChange);
        midiTypeBox->addItem ("Pitch", Gesture::pitch);
        //midiTypeBox->addItem ("AfterTouch", Gesture::afterTouch);
        midiTypeBox->setSelectedId (gesture.midiType, dontSendNotification);
        
        // ComboBox look
        midiTypeBox->setJustificationType (Justification::centred);
        midiTypeBox->setColour (ComboBox::outlineColourId, Colour (0xff000000));
        
        midiTypeBox->addListener (this);
    }
    
    void createLabels()
    {
        //=== Midi Type label ===
        
        // CC label
        addAndMakeVisible (ccLabel = new Label ("CC Label", TRANS (String(gesture.getCc()))));
        ccLabel->setEditable ((midiTypeBox->getSelectedId() == Gesture::controlChange), false, false);
        ccLabel->setFont (Font (13.0f, Font::plain));
        ccLabel->setJustificationType (Justification::centred);
        
        // cc Label is visible & editable only if "CC" is selected
        ccLabel->setEditable (midiTypeBox->getSelectedId() == Gesture::controlChange, false, false);
        ccLabel->setVisible (midiTypeBox->getSelectedId() == Gesture::controlChange);
        
        ccLabel->addListener (this);
        
        //=== range Control labels ===
        
        addAndMakeVisible (rangeLabelMin = new Label ("Min Label", TRANS (String (gesture.midiLow.getValue(), 2))));
        addAndMakeVisible (rangeLabelMax = new Label ("Max Label", TRANS (String (gesture.midiHigh.getValue(), 2))));
        
        // LabelMin style
        rangeLabelMin->setEditable (true, false, false);
        rangeLabelMin->setFont (Font (11.0f, Font::plain));
        rangeLabelMin->setJustificationType (Justification::centred);
        
        // LabelMax style
        rangeLabelMax->setEditable (true, false, false);
        rangeLabelMax->setFont (Font (11.0f, Font::plain));
        rangeLabelMax->setJustificationType (Justification::centred);
        
        
        rangeLabelMin->addListener (this);
        rangeLabelMax->addListener (this);
    }
    
    //==============================================================================
    ScopedPointer<ComboBox> midiTypeBox;
    ScopedPointer<Label> ccLabel;
    ScopedPointer<Label> rangeLabelMin;
    ScopedPointer<Label> rangeLabelMax;

    //==============================================================================
    Gesture& gesture;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiModeComponent)
};
