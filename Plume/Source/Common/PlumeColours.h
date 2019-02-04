/*
  ==============================================================================

    PlumeColours.h
    Created: 20 Dec 2018 11:35:43am
    Author:  Alex

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

namespace PLUME
{
namespace colour
{
    
enum PlumeColourID
{
    // SideBar
        // General
        sideBarBackground=0,
        sideBarMainText,
        sideBarSubText,
        sideBarSeparatorOut,
        sideBarSeparatorIn,
        sideBarButtonFill,
		sideBarButtonFillHighlighted,
		sideBarButtonFillClicked,
            
        // Presets
        presetsBoxBackground,
        presetsBoxStandartText,
        presetsBoxHighlightedBackground,
        presetsBoxHighlightedText,
        presetsBoxOutlineTop,
        presetsBoxOutlineBottom,
        presetsBoxScrollBar,
            
        // InfoPanel
        infoPanelBackground,
        infoPanelMainText,
        infoPanelSubText,
            
    // Header
        // General
        headerBackground,
        headerSeparatorTop,
        headerSeparatorBottom,
                
        // Components
        headerStandartText,
        headerHighlightedText,
            
    // Gesture Panel
        // Gesture Base Panel
            // General
            basePanelBackground,
            basePanelGestureBackground,
            basePanelGestureOutline,
            basePanelGestureHighlightedBackground,
            basePanelGestureHighlightedOutline,
            basePanelMainText,
            basePanelSubText,
                
        // gesture detail window
            detailPanelBackground,
            detailPanelHighlightedBackground,
            detailPanelMainText,
            detailPanelSubText,
            detailPanelHighlightedMainText,
            detailPanelHighlightedSubText,
            detailPanelSubComponentsOutline,
            detailPanelActiveMapping,
            
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
		red
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
    
    // Any new theme can be created here using a new private static method...
    // make sure you add the theme name to the "ThemeId" enum, and create a switch case in "createPlumeTheme()"!
};

} // namespace colour
} // namespace PLUME