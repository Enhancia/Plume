/*
  ==============================================================================

    PlumeCommon.h
    Created: 7 Nov 2018 5:01:34pm
    Author:  Alex

  ==============================================================================
*/


#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PlumeColours.h"
#include "Ui/Common/PlumeComponent.h"
#include "Ui/Common/PlumeProgressBar.h"

// Preprocessor expressions
#define TRACE_IN  Logger::writeToLog ("[+] Entering: " + String(__FUNCTION__) + "\n")
#define TRACE_OUT Logger::writeToLog ("[-]  Leaving: " + String(__FUNCTION__) + "\n")

namespace PLUME
{
    const int NUM_GEST = 3;
    
    namespace UI
    {
        // Default settings
        const int DEFAULT_WINDOW_WIDTH = 890;
        const int DEFAULT_WINDOW_HEIGHT = 500;
    
        // Display panels values
        const int MARGIN = 8;
        const int SIDEBAR_WIDTH = 200;
        const int INFO_HEIGHT = 60;
        const int PRESET_BUTTONS_HEIGHT = 20;
        const int HEADER_HEIGHT = 40;
        const int GESTURE_DETAIL_WIDTH = 300;
        
        // Display settings
        const int FRAMERATE = 60;
        
        const String font = "Tw Cen MT";
        
        extern bool ANIMATE_UI_FLAG;
        
        const float VIBRATO_DISPLAY_MAX  = 500.0f;
        const float VIBRATO_THRESH_DISPLAY_MAX = 300.0f;
    
        const float PITCHBEND_DISPLAY_MIN = -90.0f;
        const float PITCHBEND_DISPLAY_MAX =  90.0f;
    
        const float TILT_DISPLAY_MIN = -90.0f;
        const float TILT_DISPLAY_MAX =  90.0f;
    
        const float ROLL_DISPLAY_MIN = -90.0f;
        const float ROLL_DISPLAY_MAX =  90.0f;
    
        const float WAVE_DISPLAY_MIN = -90.0f;
        const float WAVE_DISPLAY_MAX =  90.0f;
        
        // Theme
        extern PLUME::colour::PlumeTheme currentTheme;
    }
    
    namespace treeId
    {
		// Value tree identifiers
	    static Identifier parameter ("parameter");
	    static Identifier plume ("PLUME");
	    static Identifier group ("group");
	    static Identifier setting ("setting");
	    static Identifier value ("value");
	    static Identifier general ("GENERAL");
	    static Identifier instance ("instance");
	    static Identifier presetDir ("PRESETDIR");
	    static Identifier pluginDirs ("PLUGINDIRS");
	    static Identifier directory ("DIR");
	    static Identifier winX ("window_x");
	    static Identifier winY ("window_y");
	    static Identifier winW ("window_w");
	    static Identifier winH ("window_h");
    }

    // gesture max range values
    const float VIBRATO_RANGE_MAX  = 500.0f;
    const float VIBRATO_THRESH_MAX = 300.0f;
    
    const float PITCHBEND_MIN = -180.0f;
    const float PITCHBEND_MAX =  180.0f;
    
    const float TILT_MIN = -180.0f;
    const float TILT_MAX =  180.0f;
    
    const float ROLL_MIN = -180.0f;
    const float ROLL_MAX =  180.0f;
    
    const float WAVE_MIN = -180.0f;
    const float WAVE_MAX =  180.0f;
    
    
    // parameter suffixs
    namespace param
    {
		enum
		{
		    value =0,
			on,
			midi_on,
			midi_cc,
			midi_low,
			midi_high,
			vibrato_range,
			vibrato_intensity,
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
        
            "_value",
            "_on",
            "_midi_on",
            "_midi_cc",
            "_midi_low",
            "_midi_high",
            "_vibrato_range",
            "_vibrato_intensity",
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
    
    // commands strings
    namespace commands
    {
        const String toFront ("toFront");
        const String updateInterface ("updateInterface");
        const String unlockInterface ("unlockInterface");
        const String lockInterface ("lockInterface");
    }
    
    namespace path
    {
		extern const Path createGearPath();
		extern const Path createPianoPath();
		extern const Path createFolderPath();
		extern const Path createMagnifyingGlassPath();
        
        const Path gearPath = createGearPath();
        const Path pianoPath = createPianoPath();
        const Path folderPath = createFolderPath();
        const Path magnifyingGlassPath = createMagnifyingGlassPath();
    }
}