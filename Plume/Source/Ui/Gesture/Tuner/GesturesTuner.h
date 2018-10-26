/*
  ==============================================================================

    GesturesTuner.h
    Created: 23 Jul 2018 4:51:09pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "Ui/Gesture/Tuner/SymmetricalTuner.h"
#include "Ui/Gesture/Tuner/OneRangeTuner.h"
#include "Ui/Gesture/Tuner/TwoRangeTuner.h"
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
                           pb.rangeLeft, pb.rangeRight, Range<float> (-90.0f, 90.0f),
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
        :   OneRangeTuner (tilt.getValueReference(), tilt.getRangeReference(), tilt.range, Range<float> (-90.0f, 90.0f),
                           String (CharPointer_UTF8 ("\xc2\xb0")))
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
        :   OneRangeTuner (wave.getValueReference(), wave.getRangeReference(), wave.range, Range<float> (-90.0f, 90.0f),
                           String (CharPointer_UTF8 ("\xc2\xb0")))
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
        :   OneRangeTuner (roll.getValueReference(), roll.getRangeReference(), roll.range, Range<float> (-90.0f, 90.0f),
                           String (CharPointer_UTF8 ("\xc2\xb0")))
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
        :   SymmetricalTuner (vib.getValueReference(), vib.getRangeReference(), vib.gain, 500.0f, "", false),
            threshold (vib.threshold)
    {
        addAndMakeVisible(threshLabel = new Label("Threshold Label"));
        threshLabel->setEditable (true, false, false);
        threshLabel->setText (String(int (threshold)), dontSendNotification);
        threshLabel->setFont (Font (13.0f, Font::plain));
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
        
        // Writes the "threshold" text
        int x = W*3/4,
            y = H*1/2,
            width = W/4,
            height = H/4;
                
        String text (TRANS("Threshold"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }
    
    void resized() override
    {
        SymmetricalTuner::resized();
        
        threshLabel->setBounds (W*3/4+W/16, H*3/4, W/8, H/5);
        repaint();
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
                lbl->setText (String (int (threshold)), dontSendNotification);
                return;
            }
            
            float val = lbl->getText().getFloatValue();
        
            if (val < 0.0f) val = 0.0f;
            else if (val > 300.0f) val = 300.0f;
        
            threshold = val;
            lbl->setText (String (int(val)), dontSendNotification);
        }
    }
    
private:
    //==============================================================================
    ScopedPointer<Label> threshLabel;
    
    float& threshold; /**< \brief Vibrato's threshold reference, used by the label to modify the right vibrato parameter */

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VibratoTuner)
};