/*
  ==============================================================================

    PlumeColours.cpp
    Created: 20 Dec 2018 11:35:43am
    Author:  Alex

  ==============================================================================
*/

#include "PlumeColours.h"

using namespace PLUME::colour;
        
PlumeTheme::~PlumeTheme()
{
}

Colour PlumeTheme::operator[] (const int index) const noexcept
{
    // bright red means there's an issue for that colour..
    // Either there wasn't enough colours in the constructor or index is out of range
    return (index < numColours && index > -1) ? palette[index] : Colour (0xffff0000);
}

Colour PlumeTheme::getColour (PlumeColourID colourId)
{
    return palette[colourId];
}

void PlumeTheme::setColour (PlumeColourID colourId, Colour newColour)
{
    palette[colourId] = newColour;
}
            
PlumeTheme PlumeTheme::createPlumeTheme (ThemeId themeToCreate)
{
    switch (themeToCreate)
    {
        case red:
            return createRedTheme();

        case test:
            return createTestTheme();
        
        /* To add a theme: 

        case newTheme: (You must add newTheme to PlumeColour.h's enum!)
            return createNewTheme(); (You must create "PlumeTheme PlumeTheme::createNewTheme()" just
                                      below, and "static PlumeTheme createNewTheme();" to the header file)
        */

		default:
            return createDefaultTheme();
    }
}

PlumeTheme PlumeTheme::createDefaultTheme()
{
    return (PlumeTheme({ 
                    // General
                         0xff000000, //plumeBackground
 
                    // SideBar
                         // General
                         0xff323232, //sideBarBackground
                         0x10ffffff, //sideBarObjectFillBackground
                         0x50ffffff, //sideBarObjectFill
                         0xffffffff, //sideBarMainText
                         0xffffffff, //sideBarSubText
                         0xffffffff, //sideBarButtonFill
 
                         // Presets
                         0x00000000, //presetsBoxBackground
                         0x00000000, //presetsBoxRowBackground
                         0x40ffffff, //presetsBoxRowBackgroundHighlighted
                         0xbbffffff, //presetsBoxRowText
                         0xff000000, //presetsBoxRowTextHighlighted
                         0xbbffffff, //presetsBoxScrollBar
                         0xbbffffff, //presetTypeToggleSelected
                         0xbbffffff, //presetTypeToggleUnselected
                         0xff393c4e, //presetsSearchBarFill
 
                         // Info Panel
                         0xff606060, //infoPanelBackground
                         0xff000000, //infoPanelIcon
                         0xffffffff, //infoPanelMainText
                         0xffffffff, //infoPanelSubText
 
                    // Header
                         // General
                         0xfff8f8f8, //headerBackground
                         0xff393939, //headerText
                         0xff393939, //headerButtonStroke
 
                         // Plugin Display
                         0xff424656, //pluginDisplayBackground

                         // Preset Display
                         0xff000000, //presetDisplayArrowsBackground
                         0xff000000, //presetDisplayBackground
                         0xff000000, //presetDisplayButtonStroke
 
                    // Gesture Panel
                      // Gesture Base Panel
                         //General
                         0xffd0d0d0, //basePanelBackground
                         0xff606060, //basePanelHeaderFill
                         0xff606060, //basePanelIcon
                         0xffffffff, //basePanelMainText
                         0xffffffff, //basePanelSubText
                         0xff606060, //basePanelButtonFill
 
                         //Empty Slot
                         0xff000000, //emptySlotOutline
                         0x07ffffff, //emptySlotBackground
                             
                      // Gesture Detail Panel
                         // General
                         0xfff0f0f0, //detailPanelBackground
                         0xff000000, //detailPanelHeaderFill
                         0xffffffff, //detailPanelMainText
                         0xffffffff, //detailPanelSubText
                         0xff606060, //detailPanelButtonFill
 
                         // Tuner Panel
                         0xfff0f0f0, //tunerBackground
                         0xff505050, //tunerSliderBackground
                         0xfff0f0f0, //tunerSliderThumb
                         0xfff0f0f0, //tunerButtonFill
 
                         // Midi&Map Panel
                         0xffa0a0a0, //midiMapBannerBackground
                         0xffa0a0a0, //midiMapBodyBackground
                         0xff505050, //midiMapSliderBackground
 
                    // Gesture Colours
                         0xff808080, //mutedHighlight
                         0xffec6565, //vibratoHighlight
                         0xff1fcaa8, //pitchBendHighlight
                         0xfff3a536, //tiltHighlight
                         0xffa255f3, //rollHighlight
                         0xff7c80de, //waveHighlight
 
                    // Top Panel
                         0xff494949, //topPanelBackground
                         0xc0000000, //topPanelTransparentArea
                         0xffffffff, //topPanelMainText
                         0x50ffffff //topPanelSubText
                       }));
}

PlumeTheme PlumeTheme::createRedTheme()
{
    return (PlumeTheme({ 
                    // General
                         0xff401010, //plumeBackground
 
                    // SideBar
                         // General
                         0xff461d1d, //sideBarBackground
                         0x10ffffff, //sideBarObjectFillBackground
                         0x50ffffff, //sideBarObjectFill
                         0xffffffff, //sideBarMainText
                         0xffffffff, //sideBarSubText
                         0xffff8080, //sideBarButtonFill
 
                         // Presets
                         0x00000000, //presetsBoxBackground
                         0x00000000, //presetsBoxRowBackground
                         0x40ffffff, //presetsBoxRowBackgroundHighlighted
                         0xbbffffff, //presetsBoxRowText
                         0xff441818, //presetsBoxRowTextHighlighted
                         0xbbffffff, //presetTypeToggleSelected
                         0xbbffffff, //presetTypeToggleUnselected
                         0xbbcb4747, //presetsBoxScrollBar

                         0xff441818, //presetTypeToggleBackgroundLine
                         0xbbcb4747, //presetTypeToggleUnselected
                         0xff441818, //presetTypeToggleSelected
                         0xff393c4e, //presetsSearchBarFill
 
                         // Info Panel
                         0xff606060, //infoPanelBackground
                         0xff000000, //infoPanelIcon
                         0xffffffff, //infoPanelMainText
                         0xffffffff, //infoPanelSubText
 
                    // Header
                         // General
                         0xfff8f8f8, //headerBackground
                         0xff532323, //headerText
                         0xff532323, //headerButtonStroke
 
                         // Plugin Display
                         0xff424656, //pluginDisplayBackground
                         
                         // Preset Display
                         0xff532323, //presetDisplayArrowsBackground
                         0xff927575, //presetDisplayBackground
                         0xff927575, //presetDisplayButtonStroke
 
                    // Gesture Panel
                      // Gesture Base Panel
                         //General
                         0xffd0d0d0, //basePanelBackground
                         0xff606060, //basePanelHeaderFill
                         0xff606060, //basePanelIcon
                         0xffffffff, //basePanelMainText
                         0xffffffff, //basePanelSubText
                         0xff606060, //basePanelButtonFill
 
                         //Empty Slot
                         0xff000000, //emptySlotOutline
                         0x07ffffff, //emptySlotBackground
                             
                      // Gesture Detail Panel
                         // General
                         0xfff0f0f0, //detailPanelBackground
                         0xff000000, //detailPanelHeaderFill
                         0xffffffff, //detailPanelMainText
                         0xffffffff, //detailPanelSubText
                         0xff606060, //detailPanelButtonFill
 
                         // Tuner Panel
                         0xfff0f0f0, //tunerBackground
                         0xff505050, //tunerSliderBackground
                         0xfff0f0f0, //tunerSliderThumb
                         0xfff0f0f0, //tunerButtonFill
 
                         // Midi&Map Panel
                         0xffa0a0a0, //midiMapBannerBackground
                         0xffa0a0a0, //midiMapBodyBackground
                         0xff505050, //midiMapSliderBackground
 
                    // Gesture Colours
                         0xff808080, //mutedHighlight
                         0xffe04040, //vibratoHighlight
                         0xffe04040, //pitchBendHighlight
                         0xffe04040, //tiltHighlight
                         0xffe04040, //rollHighlight
                         0xffe04040, //waveHighlight
 
                    // Top Panel
                         0xff604040, //topPanelBackground
                         0xc0000000, //topPanelTransparentArea
                         0xffffffff, //topPanelMainText
                         0x50ffffff //topPanelSubText
                       }));
}

PlumeTheme PlumeTheme::createTestTheme()
{
    return (PlumeTheme({ 
                    // General
                         0xff1f222b, //plumeBackground
 
                    // SideBar
                         // General
                         0xff363c4e, //sideBarBackground
                         0xff21252f, //sideBarObjectFillBackground
                         0xff2c303e, //sideBarObjectFill
                         0xffffffff, //sideBarMainText
                         0xffffffff, //sideBarSubText
                         0xffffffff, //sideBarButtonFill
 
                         // Presets
                         0xff323643, //presetsBoxBackground
                         0xff3b3f4b, //presetsBoxRowBackground
                         0xff4a5063, //presetsBoxRowBackgroundHighlighted
                         0xffffffff, //presetsBoxRowText
                         0xffffffff, //presetsBoxRowTextHighlighted
                         0xfff8f8f9, //presetsBoxScrollBar
                         0xfff3a536, //presetTypeToggleSelected
                         0xff3b3f4b, //presetTypeToggleUnselected
                         0xff393c4e, //presetsSearchBarFill
 
                         // Info Panel
                         0xff2d303d, //infoPanelBackground
                         0xff1f222b, //infoPanelIcon
                         0xffffffff, //infoPanelMainText
                         0x70ffffff, //infoPanelSubText
 
                    // Header
                         // General
                         0xff363c4e, //headerBackground
                         0xffffffff, //headerText
                         0xffffffff, //headerButtonStroke 
 
                         // Plugin Display
                         0xff424656, //pluginDisplayBackground
                         
                         // Preset Display
                         0xff495063, //presetDisplayArrowsBackground
                         0xff252835, //presetDisplayBackground
                         0xffffffff, //presetDisplayButtonStroke
 
                    // Gesture Panel
                      // Gesture Base Panel
                         //General
                         0xff282b35, //basePanelBackground
                         0xff363c4e, //basePanelHeaderFill
                         0xfff3f3f3, //basePanelIcon
                         0xffffffff, //basePanelMainText
                         0xff4a5063, //basePanelSubText
                         0xff22252e, //basePanelButtonFill
 
                         //Empty Slot
                         0xff464b5e, //emptySlotOutline
                         0x07ffffff, //emptySlotBackground
                             
                      // Gesture Detail Panel
                         // General
                         0xff282b35, //detailPanelBackground
                         0xff363c4e, //detailPanelHeaderFill
                         0xffffffff, //detailPanelMainText
                         0xff4a5063, //detailPanelSubText
                         0xff22252e, //detailPanelButtonFill
 
                         // Tuner Panel
                         0xff282b35, //tunerBackground
                         0xff4a5062, //tunerSliderBackground
                         0xffffffff, //tunerSliderThumb
                         0xff424656, //tunerButtonFill
 
                         // Midi&Map Panel
                         0xff2c2f3a, //midiMapBannerBackground
                         0xff21252f, //midiMapBodyBackground
                         0xff323643, //midiMapSliderBackground
 
                      // Gesture Colours
                         0xff808080, //mutedHighlight
                         0xffec6565, //vibratoHighlight
                         0xff1fcaa8, //pitchBendHighlight
                         0xfff3a536, //tiltHighlight
                         0xffa255f3, //rollHighlight
                         0xff7c80de, //waveHighlight
 
                    // Top Panel
                         0xff363c4e, //topPanelBackground
                         0xc0000005, //topPanelTransparentArea
                         0xffffffff, //topPanelMainText
                         0x50ffffff //topPanelSubText
                       }));
}