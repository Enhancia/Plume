/*
  ==============================================================================

    PlumeColours.h
    Created: 20 Dec 2018 11:35:43am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#define getPlumeColour(x) (PLUME::UI::currentTheme.getColour (PLUME::colour::x))

namespace PLUME
{
namespace colour
{
    
enum PlumeColourID
{
    // General
    plumeBackground =0,

    // SideBar
        // General
        sideBarBackground,
        sideBarObjectFillBackground,
        sideBarObjectFill,
        sideBarMainText,
        sideBarSubText,
        sideBarButtonFill,
            
        // Presets
        presetsBoxBackground,
        presetsBoxRowBackground,
        presetsBoxRowBackgroundHighlighted,
        presetsBoxRowText,
        presetsBoxRowTextHighlighted,
        presetsBoxScrollBar,
            
        // Info Panel
        infoPanelBackground,
        infoPanelIcon,
        infoPanelMainText,
        infoPanelSubText,
            
    // Header
        // General
        headerBackground,
        headerText,
        headerButtonStroke,

       // Preset Display
        presetDisplayArrowsBackground,
        presetDisplayBackground,
        presetDisplayButtonStroke,
            
    // Gesture Panel
        // Gesture Base Panel
            //General
            basePanelBackground,
            basePanelHeaderFill,
            basePanelIcon,
            basePanelMainText,
            basePanelSubText,
            basePanelButtonFill,

            //Empty Slot
            emptySlotOutline,
            emptySlotBackground,
                
        // Gesture Detail Panel
            // General
            detailPanelBackground,
            detailPanelHeaderFill,
            detailPanelMainText,
            detailPanelSubText,
            detailPanelButtonFill,

            // Tuner Panel
            tunerBackground,
            tunerSliderBackground,
            tunerSliderThumb,
            tunerButtonFill,

            // Midi&Map Panel
            midiMapBannerBackground,
            midiMapBodyBackground,
            midiMapSliderBackground,

        // Gesture Colours
        mutedHighlight,
        vibratoHighlight,
        pitchBendHighlight,
        tiltHighlight,
        rollHighlight,
        waveHighlight,
            
    // Top Panel
    topPanelBackground,
    topPanelTransparentArea,
    topPanelMainText,
    topPanelSubText,
    
    numColours
};
        
class PlumeTheme
{
public:
    enum ThemeId
    {
	   plumeDefault = 0,
	   red,
       test
    };
    
    template <typename... Items>
    PlumeTheme (Items... colourNumbers)
    {
        jassert (sizeof... (colourNumbers) == numColours);
    
        const Colour c[] = { Colour (colourNumbers)... };
        for (int i=0; i<numColours; i++)
        {
            palette[i] = c[i];
        }
    }
    
    ~PlumeTheme();
    
    Colour operator[] (const int index) const noexcept;
    
    Colour getColour (PlumeColourID colourId);
    void setColour (PlumeColourID colourId, Colour newColour);

    static PlumeTheme createPlumeTheme (ThemeId themeToCreate);
            
private:
    Colour palette[PlumeColourID::numColours];
    
    static PlumeTheme createDefaultTheme();
    static PlumeTheme createRedTheme();
    static PlumeTheme createTestTheme();
    
    // Any new theme can be created here using a new private static method...
    // make sure you add the theme name to the "ThemeId" enum, and create a switch case in "createPlumeTheme()"!
};

} // namespace colour
} // namespace PLUME