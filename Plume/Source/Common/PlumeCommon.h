/*
  ==============================================================================

    PlumeCommon.h
    Created: 7 Nov 2018 5:01:34pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

namespace plumeCommon
{
    // Default settings
    const int DEFAULT_WINDOW_WIDTH = 800;
    const int DEFAULT_WINDOW_HEIGHT = 450;
    
    // Display settings
    const int MARGIN = 8;
    const int NUM_GEST = 3;
    const int FRAMERATE = 60;
    
    // Value tree identifiers
    const Identifier groupIdentifier ("group");
    const Identifier typeIdentifier ("type");
    const Identifier gestureIdentifier ("gesture");
    const Identifier parameterIdentifier ("parameter");
    const Identifier windowWIdentifier ("window_w");
    const Identifier windowHIdentifier ("window_h");
    
    // parameter suffixs
    namespace param
    {
		enum
		{
			on = 0,
			midi_on,
			midi_cc,
			midi_low,
			midi_high,
			vibrato_range,
			vibrato_thresh,
			bend_leftLow,
			bend_leftHigh,
			bend_rightLow,
			bend_rightHigh,
			tilt_low,
			tilt_high,
			roll_low,
			roll_high,

			numParams
		};
        
        const String paramIds[] = {
        
            "_on",
            "_midi_on",
            "_midi_cc",
            "_midi_low",
            "_midi_high",
            "_vibrato_range",
            "_vibrato_thresh",
            "_bend_leftLow",
            "_bend_leftHigh",
            "_bend_rightLow",
            "_bend_rightHigh",
            "_tilt_low",
            "_tilt_high",
            "_roll_low",
            "_roll_high"
        };
    }
    
    // gesture max range values
    const float VIBRATO_RANGE_MAX  = 500.0f;
    const float VIBRATO_THRESH_MAX = 300.0f;
    
    const float PITCHBEND_MIN = -90.0f;
    const float PITCHBEND_MAX =  90.0f;
    
    const float TILT_MIN = -90.0f;
    const float TILT_MAX =  90.0f;
    
    const float ROLL_MIN = -90.0f;
    const float ROLL_MAX =  90.0f;
    
    const float WAVE_MIN = -90.0f;
    const float WAVE_MAX =  90.0f;
}