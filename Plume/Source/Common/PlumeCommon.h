/*
  ==============================================================================

    PlumeCommon.h
    Created: 7 Nov 2018 5:01:34pm
    Author:  Alex

  ==============================================================================
*/


#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

#if JUCE_WINDOWS
#include <windows.h>
#elif JUCE_MAC
#include <stdlib.h>
#endif

#include "PlumeColours.h"
#include "../Ui/Common/PlumeComponent.h"
#include "../Ui/Common/PlumeProgressBar.h"
#include "../Ui/Common/DualTextToggle.h"
#include "../Ui/Common/PlumeShapeButton.h"

// Preprocessor expressions
#define TRACE_IN  Logger::writeToLog ("[+] Entering: " + String(__FUNCTION__) + "\n")
#define TRACE_OUT Logger::writeToLog ("[-]  Leaving: " + String(__FUNCTION__) + "\n")

namespace PLUME
{
    const int NUM_GEST = 8;
    const int MAX_PARAMETER = 6;
    
    namespace UI
    {
        // Default settings
        const int DEFAULT_WINDOW_WIDTH = 890;
        const int DEFAULT_WINDOW_HEIGHT = 500;
    
        // Display panels values
        const int MARGIN = 8;
        const int MARGIN_SMALL = 4;
        const int SIDEBAR_WIDTH = 220;
        const int INFO_HEIGHT = 60;
        const int PRESET_BUTTONS_HEIGHT = 30;
        const int PRESET_SEARCHBAR_HEIGHT = 22;
        const int HEADER_HEIGHT = 40;
        const int GESTURE_DETAIL_WIDTH = 300;
        const int OPTIONS_HEIGHT = 30;
        const int SUBPANEL_ROW_HEIGHT = 16;
        
        // Display settings
        const int FRAMERATE = 60;
        
        extern bool ANIMATE_UI_FLAG;
        
        const float VIBRATO_DISPLAY_MAX  = 500.0f;
        const float VIBRATO_THRESH_DISPLAY_MAX = 500.0f;
    
        const float PITCHBEND_DISPLAY_MIN = -90.0f;
        const float PITCHBEND_DISPLAY_MAX =  90.0f;
    
        const float TILT_DISPLAY_MIN = -90.0f;
        const float TILT_DISPLAY_MAX =  90.0f;
    
        const float ROLL_DISPLAY_MIN = -90.0f;
        const float ROLL_DISPLAY_MAX =  90.0f;
    
        const float WAVE_DISPLAY_MIN = -90.0f;
        const float WAVE_DISPLAY_MAX =  90.0f;
        
        // Theme
        using namespace PLUME::colour;
        extern PlumeTheme currentTheme;

        extern void paintTiledPath (Graphics&, Path&, juce::Rectangle<float>,
                                    const float, const float,
                                    const Colour, const Colour, const float);
    }
    
    namespace font
    {
        enum PlumeFontId
        {
            regular =0,
            bold,
            book,
            light
        };
        
        extern const Font plumeFont;
        extern const Font plumeFontBold;
        extern const Font plumeFontBook;
        extern const Font plumeFontLight;

        // Font heights
        const float SIDEBAR_LABEL_FONT_H = 12.0f;
        const float HEADER_LABEL_FONT_H = 14.0f;
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

    // data
    namespace data
    {
        enum PlumeData
        {
            variance =0,
            acceleration,
            tilt,
            roll,
            battery,

            numDatas
        };

        static float convertRawBatteryToPercentage (float rawBatteryValue)
        {
            if (rawBatteryValue <= 3.0f)      return 0.0f;
            else if (rawBatteryValue >= 4.2f) return 1.0f;

            else return (rawBatteryValue - 3.0f)/(4.2f - 3.0f);
        }
    }
    
    // commands strings
    namespace commands
    {
        static const String toFront ("toFront");
        static const String updateInterface ("updateInterface");
        static const String unlockInterface ("unlockInterface");
        static const String lockInterface ("lockInterface");
    }
    
    namespace path
    {
		enum PathId
		{
			gear = 0,
			piano,
			burger,
			burgerShort,
			options,
			optionsShort,
			folder,
			magnifyingGlass,
			floppyDisk,
			flatSave,
			onOff,
            info,
            handFingerDown,
            handTilt,
            handRoll,
            vibratoRipple,
            tiltArrow,
            pitchBendArrow,
            rollArrow,
            waveIcon
		};

        extern const Path createPath (PathId pathToCreate);
    }

  #if JUCE_WINDOWS
	extern LRESULT CALLBACK messageHook (int nCode, WPARAM wParam, LPARAM lParam);
  #endif

    struct GlobalPointers
    {
    public:
        //ComponentPeer* const getWrappedEditorPeer (int index);
        const int findComponentPeerIndex (ComponentPeer* componentPeerToCheck);
        void addWrappedEditorPeer (ComponentPeer* newPeerPtr);
        void resetWrappedEditorPeer (ComponentPeer* peerToReset);
        //void removeWrappedEditorPeer (int index);

      #if JUCE_WINDOWS
        bool isPlumeHWND (HWND HWNDToCheck);
        const int findPlumeHWNDindex (HWND HWNDToCheck);
        void addPlumeHWND (HWND newHWND);
        void removePlumeHWND (HWND HWNDToRemove);

        void clearPlumeAndWrappedWindows();
        void setActiveHWND (HWND HWNDToSetActive);
        HWND getActiveHWND();

        ComponentPeer* const getWrappedEditorPeer (HWND correspondingHWND);
        void setWrappedEditorPeer (HWND correspondingHWND, ComponentPeer* newPeerPtr);
        void resetWrappedEditorPeer (HWND correspondingHWND);
      #endif

    private:
        /** \brief Global pointer (*Gasp...*) that keeps the adress of the WrappedEditor's ComponentPeer.
            
                   Use sparingly, mainly used to force the window to behave correctly on specific DAW/use case.
                   For instance, to force the WrapperEditor to minimize when switching tracks in Ableton Live.
        */
        Array<ComponentPeer*> wrappedEditorPeerArray;

      #if JUCE_WINDOWS
        Array<HWND> plumeWindowArray;
        CriticalSection hwndArrayLock;

        HWND activePlumeWindow = NULL;
      #endif

        JUCE_LEAK_DETECTOR (GlobalPointers);
    };
    
    extern GlobalPointers globalPointers;
}