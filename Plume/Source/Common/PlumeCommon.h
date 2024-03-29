/*==============================================================================

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

#if JUCE_DEBUG
#define JUCE_DISABLE_AUDIOPROCESSOR_BEGIN_END_GESTURE_CHECKING 1
#endif

namespace PLUME
{
    const int NUM_GEST = 8;
    const int MAX_PARAMETER = 6;

    extern int nbInstance; /**< @Brief shared variable between same instance .dll, tracks number of instance opened  */
    
    namespace UI
    {
        // Default settings
        const int DEFAULT_WINDOW_WIDTH = 980;
        const int DEFAULT_WINDOW_HEIGHT = 550;
    
        // Display panels values
        const int MARGIN = 6;
        const int MARGIN_SMALL = 4;
        const int SIDEBAR_WIDTH = 220;
        const int INFO_HEIGHT = 60;
        const int PRESET_BUTTONS_HEIGHT = 30;
        const int PRESET_SEARCHBAR_HEIGHT = 28;
        const int HEADER_HEIGHT = 40;
        const int GESTURE_DETAIL_WIDTH = 300;
        const int OPTIONS_HEIGHT = 30;
        const int SUBPANEL_ROW_HEIGHT = 16;
        
        // Display settings
        const int FRAMERATE = 60;
        
        extern bool ANIMATE_UI_FLAG;
        
        const float VIBRATO_DISPLAY_MAX  = 100.0f;
        const float VIBRATO_THRESH_DISPLAY_MAX = 100.0f;
    
        const float PITCHBEND_DISPLAY_MIN = -80.0f;
        const float PITCHBEND_DISPLAY_MAX =  80.0f;
    
        const float TILT_DISPLAY_MIN = -80.0f;
        const float TILT_DISPLAY_MAX =  80.0f;
    
        const float ROLL_DISPLAY_MIN = -80.0f;
        const float ROLL_DISPLAY_MAX =  80.0f;
    
        const float WAVE_DISPLAY_MIN = -80.0f;
        const float WAVE_DISPLAY_MAX =  80.0f;
        
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
            medium,
            book,
            light
        };
        
        extern const Font plumeFont;
        extern const Font plumeFontBold;
        extern const Font plumeFontMedium;
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
        static Identifier currentPreset ("CURRENTPRESET");
        static Identifier currentPresetName ("c_preset_name");
        static Identifier currentPresetType ("c_preset_type");
        static Identifier currentPresetFile ("c_preset_file");
	    static Identifier general ("GENERAL");
	    static Identifier instance ("instance");
	    static Identifier presetDir ("PRESETDIR");
	    static Identifier pluginDirs ("PLUGINDIRS");
	    static Identifier directory ("DIR");
	    static Identifier winX ("window_x");
	    static Identifier winY ("window_y");
	    static Identifier winW ("window_w");
	    static Identifier winH ("window_h");
        static Identifier wrapped_winX ("w_window_x");
        static Identifier wrapped_winY ("w_window_y");
        static Identifier wrapped_winW ("w_window_w");
        static Identifier wrapped_winH ("w_window_h");
        static Identifier wrapped_visible ("w_visible");
    }

    namespace gesture
    {
        // gesture max range values
        const float VIBRATO_RANGE_MAX  = 30.0f;
        const float VIBRATO_THRESH_MAX = 500.0f;
        const float VIBRATO_INTENSITY_MAX = 700.0f;
        const float PITCHBEND_MIN = -180.0f;
        const float PITCHBEND_MAX =  180.0f;
        const float TILT_MIN = -180.0f;
        const float TILT_MAX =  180.0f;
        const float ROLL_MIN = -180.0f;
        const float ROLL_MAX =  180.0f;
        
        const float WAVE_MIN = -180.0f;
        const float WAVE_MAX =  180.0f;

        // Gesture default values
        const float VIBRATO_RANGE_DEFAULT  = 50.0f;
        const float VIBRATO_THRESH_DEFAULT = 40.0f;
        const float PITCHBEND_DEFAULT_LEFTMIN = -40.0f;
        const float PITCHBEND_DEFAULT_LEFTMAX = -20.0f;
        const float PITCHBEND_DEFAULT_RIGHTMIN = 20.0f;
        const float PITCHBEND_DEFAULT_RIGHTMAX = 40.0f;
        const float TILT_DEFAULT_MIN =  0.0f;
        const float TILT_DEFAULT_MAX =  60.0f;
        const float ROLL_DEFAULT_MIN =  0.0f;
        const float ROLL_DEFAULT_MAX =  60.0f;
        const float WAVE_DEFAULT_MIN = -50.0f;
        const float WAVE_DEFAULT_MAX =  50.0f;
    }
    
    // parameter suffixs
    namespace param
    {
        static String defaultParameterName = "Unmapped Parameter";
        
		enum
		{
            gesture_param_0 = 0,
            gesture_param_1,
            gesture_param_2,
            gesture_param_3,

			numParams
		};

        enum
        {
            vibrato_value = 0,
            vibrato_intensity,
            tilt_value,
            roll_value,

            numValues
        };
        
        const String paramIds[] = {
            "_gesture_param_0",
            "_gesture_param_1",
            "_gesture_param_2",
            "_gesture_param_3"
        };
        
        const String valuesIds[] = {
            "vibrato_value",
            "vibrato_intensity",
            "tilt_value",
            "roll_value"
        };

        enum armValue
        {
            notArmed = 0,
            armed = 1,
            unknownArm = 2
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

        /**
         * @brief convert raw battery to value between 0 and 1.0
         * 
         * @param batteryValue 
         * @param isCharging 
         * @return float 
         */
		extern float convertRawBatteryToPercentage (float batteryValue, bool isCharging);
    }

    // file
    namespace file
    {
      #if JUCE_WINDOWS
        static const File pluginList       (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/Plume/plumepl.cfg"));
        static const File presetDir        (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/Plume/plumepr.cfg"));
        static const File deadMansPedal    (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/Plume/plumedmp.cfg"));
        static const File plumeLog         (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/Plume/Logs/plumeLog.txt"));
        static const File scanLog          (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Enhancia/Plume/Logs/scanLog.txt"));
        static const File defaultPresetDir (File::getSpecialLocation (File::globalApplicationsDirectoryX86).getChildFile ("Enhancia/Plume/Presets/"));
        static const File scannerExe       (File::getSpecialLocation (File::globalApplicationsDirectory).getChildFile ("Enhancia/utilities/PluginScanner.exe"));
      #elif JUCE_MAC
        static const File pluginList       (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Application Support/Enhancia/Plume/plumepl.cfg"));
        static const File presetDir        (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Application Support/Enhancia/Plume/plumepr.cfg"));
        static const File deadMansPedal    (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Application Support/Enhancia/Plume/plumedmp.cfg"));
        static const File plumeLog         (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Logs/Enhancia/Plume/plumeLog.txt"));
        static const File scanLog          (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Logs/Enhancia/Plume/scanLog.txt"));
        static const File defaultPresetDir (File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Audio/Presets/Plume/"));
        static const File scannerExe       (File::getSpecialLocation (File::commonApplicationDataDirectory).getChildFile ("Application Support/Enhancia/PlumePluginScanner"));
      #endif
    } 
    
    // commands strings
    namespace commands
    {
        static const String toFront ("toFront");
        static const String updateInterface ("updateInterface");
        static const String updateBatteryDisplay ("updateBatteryDisplay");
        static const String unlockInterface ("unlockInterface");
        static const String lockInterface ("lockInterface");
        static const String scanRequired ("scanRequired");
        static const String missingScript ("missingScript");
        static const String missingPlugin ("missingPlugin");
        static const String mappingOverwrite ("mappingOverwrite");
        static const String scanCrashed ("scanCrashed");
        static const String plumeCrashed ("plumeCrashed");
    }

    namespace compatibility
    {
        extern bool isTestVersion();
    }

    namespace auth
    {
        const String MACHINE_TOKEN ("50327c582d22471d2427faed42c9928dcd8b0e98 "); //std::getenv ("MACHINE_ENHANCIA_OAUTH");
    }

    namespace standalone
    {
        static const int minBpm = 10;
        static const int maxBpm = 400;
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
            vibrato,
            tilt,
            pitchBend,
            roll,
            wave,
            plumeLogo,
            ring
		};

        extern const Path createPath (PathId pathToCreate);
    }

    namespace log
    {
        enum class LogLevel
        {
            trace =0,
            debug,
            info,
            warning,
            error,
            fatal
        };

        enum class LogCategory
        {
            general =0,
            gesture,
            presets,
            ui,
            options,
            update,
            pluginScan,
            pluginWrapping,
            wrappedInterface,
            security,
            bugReport
        };
        
        const String levelStrings[] = {
            "TRACE:  ",
            "DEBUG:  ",
            "INFO:   ",
            "WARNING:",
            "ERROR:  ",
            "FATAL:  "
        };

        const String categoryStrings[] = {
            "general: ",
            "gesture: ",
            "presets: ",
            "interface: ",
            "options: ",
            "update: ",
            "pluginScan: ",
            "pluginWrapping: ",
            "wrappedInterface: ",
            "security: ",
            "bugReport: "
        };

        extern void writeToLog (const String& message, const LogCategory category=LogCategory::general, const LogLevel level=LogLevel::info);
    }

    namespace keyboard_shortcut
    {
        const KeyPress closeWindow          (KeyPress::escapeKey);
        const KeyPress openPreset           (KeyPress::spaceKey);
        const KeyPress openPresetType       (KeyPress::spaceKey);
        const KeyPress openPresetTypeBis    (KeyPress::returnKey);
        const KeyPress duplicateGesture     ('d', ModifierKeys (ModifierKeys::commandModifier), 'd');
        const KeyPress saveGesture          ('s', ModifierKeys (ModifierKeys::commandModifier), 's');
        const KeyPress help                 (KeyPress::F1Key);
        const KeyPress rename               (KeyPress::F2Key);
    }

  #if JUCE_WINDOWS
	extern LRESULT CALLBACK messageHook (int nCode, WPARAM wParam, LPARAM lParam);
  #endif

    /** \brief Global pointer (*Gasp...*) that keeps the adress of the WrappedEditor's ComponentPeer.
        
               Use sparingly, mainly used to force the window to behave correctly on specific DAW/use case.
               For instance, to force the WrapperEditor to minimize when switching tracks in Ableton Live.
    */
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
