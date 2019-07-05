/*
  ==============================================================================

    GesturesTuner.h
    Created: 23 Jul 2018 4:51:09pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"

#include "Ui/Gesture/Tuner/Tuner.h"
#include "Ui/Gesture/Tuner/SymmetricalTuner.h"
#include "Ui/Gesture/Tuner/TwoRangeTuner.h"
#include "Ui/Gesture/Tuner/OneRangeTuner.h"
#include "Ui/Gesture/Tuner/VibratoTestTuner.h"

#include "Gesture/Vibrato.h"
#include "Gesture/PitchBend.h"
#include "Gesture/Tilt.h"
#include "Gesture/Wave.h"
#include "Gesture/Roll.h"

//==============================================================================
class PitchBendTuner: public TwoRangeTuner
{
public:
    PitchBendTuner(PitchBend& pb)
        :   TwoRangeTuner (pb.getValueReference(), pb.getRangeReference(),
                           pb.rangeLeftLow, pb.rangeLeftHigh,
                           pb.rangeRightLow, pb.rangeRightHigh,
			               Range<float> (PLUME::UI::PITCHBEND_DISPLAY_MIN, PLUME::UI::PITCHBEND_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")))
    {}
    
    ~PitchBendTuner()
    {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchBendTuner)
};


//==============================================================================
class TiltTuner: public OneRangeTuner
{
public:
    TiltTuner(Tilt& tilt)
        :   OneRangeTuner (tilt.getValueReference(), tilt.getRangeReference(),
			               tilt.rangeLow,
						   tilt.rangeHigh,
			               Range<float> (PLUME::UI::TILT_DISPLAY_MIN, PLUME::UI::TILT_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")), OneRangeTuner::tilt)
    {}
    
    ~TiltTuner()
    {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TiltTuner)
};

/*
//==============================================================================
class WaveTuner: public OneRangeTuner
{
public:
    WaveTuner(Wave& wave)
        :   OneRangeTuner (wave.getValueReference(), wave.getRangeReference(), wave.range,
                           Range<float> (PLUME::UI::WAVE_DISPLAY_MIN, PLUME::UI::WAVE_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")), OneRangeTuner::wave)
    {}
    
    ~WaveTuner()
    {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveTuner)
};
*/

//==============================================================================
class RollTuner: public OneRangeTuner
{
public:
    RollTuner(Roll& roll)
        :   OneRangeTuner (roll.getValueReference(), roll.getRangeReference(),
			               roll.rangeLow, roll.rangeHigh,
				           Range<float> (PLUME::UI::ROLL_DISPLAY_MIN, PLUME::UI::ROLL_DISPLAY_MAX),
                           String (CharPointer_UTF8 ("\xc2\xb0")), OneRangeTuner::roll)
    {}
    
    ~RollTuner()
    {}

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RollTuner)
};

//==============================================================================
class VibratoTuner: public SymmetricalTuner
{
public:
    VibratoTuner(Vibrato& vib)
        :   SymmetricalTuner (vib.getValueReference(), vib.getRangeReference(), vib.gain, PLUME::UI::VIBRATO_DISPLAY_MAX, "", false),
            threshold (vib.threshold)
    {
        addAndMakeVisible(threshLabel = new Label("Threshold Label"));
        threshLabel->setEditable (true, false, false);
        threshLabel->setText (String (int (threshold.convertFrom0to1 (threshold.getValue()))), dontSendNotification);
        threshLabel->setFont (Font (PLUME::UI::font, 13.0f, Font::plain));
        threshLabel->setJustificationType (Justification::centred);
        threshLabel->addListener (this);
    }
    
    ~VibratoTuner()
    {
        threshLabel->removeListener (this);
        threshLabel = nullptr;
    }
    
    //==============================================================================
    void paint (Graphics& g) override
    {
        SymmetricalTuner::paint(g); // SuperClass method call
        g.setColour (Colour (0xff303030));
        g.setFont (PLUME::font::plumeFont);
        
        // Writes the "threshold" text
        int x = getWidth()*3/4,
            y = getHeight()*1/2,
            width = getWidth()/4,
            height = getHeight()/4;
                
        String text (TRANS("Threshold"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }
    
    void resized() override
    {
        SymmetricalTuner::resized();
        
        threshLabel->setBounds (getWidth()*3/4+getWidth()/16, getHeight()*3/4, getWidth()/8, getHeight()/5);
        repaint();
    }
    
    void updateComponents() override
    {
        SymmetricalTuner::updateComponents(); //base class display update
        
        // Sets label text
        if (!(threshLabel->isBeingEdited()))
		{
            threshLabel->setText (String (int (threshold.convertFrom0to1 (threshold.getValue()))), dontSendNotification);
		}
    }
    //==============================================================================
    void labelTextChanged (Label* lbl) override
    {
        SymmetricalTuner::labelTextChanged(lbl); // SuperClass method call
        
        if (lbl == threshLabel)
        {
            // checks that the string is numbers only
            if (lbl->getText().containsOnly ("0123456789") == false)
            {
                lbl->setText (String (int (threshold.convertFrom0to1 (threshold.getValue()))), dontSendNotification);
                return;
            }
            
            float val = lbl->getText().getFloatValue();
        
            if (val < 0.0f) val = 0.0f;
            else if (val > 300.0f) val = 300.0f;
            
            threshold.beginChangeGesture();
            threshold.setValueNotifyingHost (threshold.convertTo0to1 (val));
            threshold.endChangeGesture();
            
            lbl->setText (String (int(val)), dontSendNotification);
        }
    }
    
private:
    //==============================================================================
    ScopedPointer<Label> threshLabel;
    
    RangedAudioParameter& threshold; /**< \brief Vibrato's threshold reference, used by the label to modify the right vibrato parameter */

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoTuner)
};