/*
  ==============================================================================

    HideSideBarButton.cpp
    Created: 29 May 2019 12:03:32pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#include "HideSideBarButton.h"

//==============================================================================
HideSideBarButton::HideSideBarButton()	: Button ("Side Bar Button")
{
}

HideSideBarButton::~HideSideBarButton()
{
}

//==============================================================================
void HideSideBarButton::paintButton (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	drawButtonBackgroundPath (g);
	drawArrowPath (g, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

//==============================================================================
void HideSideBarButton::drawArrowPath (Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
	using namespace PLUME::UI;

    Path p;
    
    if (getToggleState())
    {
        p.startNewSubPath (0,0);
        p.lineTo (4, 3);
        p.lineTo (0, 6);
    }
    else
    {
        p.startNewSubPath (4, 0);
        p.lineTo (0, 3);
        p.lineTo (4, 6);
    }
    
    auto arrowArea = getLocalBounds().withSizeKeepingCentre (3, 6);

    p.scaleToFit (arrowArea.getX(),
                  arrowArea.getY(),
                  arrowArea.getWidth(),
                  arrowArea.getHeight(),
                  true);

    if (shouldDrawButtonAsDown)
    {
    	g.setColour (Colour (0xff9095ff));
    }
    else
    {
    	if (shouldDrawButtonAsHighlighted)
    	{
    		g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::sideBarButtonFill)
    											.withAlpha (0.6f));
    	}
    	else
    	{
    		g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::sideBarButtonFill));
    	}
    }

    g.strokePath (p, PathStrokeType (1.1f, PathStrokeType::mitered, PathStrokeType::rounded));
}

void HideSideBarButton::drawButtonBackgroundPath (Graphics& g)
{
	Path p;
	p.startNewSubPath (0.0f, 0.0f);
	p.cubicTo ({0.0f, getHeight()/4.0f},
			   {float(getWidth()), getHeight()/4.0f},
			   {float(getWidth()), getHeight()/2.0f});

	p.cubicTo ({float(getWidth()), getHeight()*3/4.0f},
			   {0.0f, getHeight()*3/4.0f},
			   {0.0f, float (getHeight())});
	p.closeSubPath();

	g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::sideBarBackground));
	g.fillPath (p);
}