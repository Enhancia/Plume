/*
  ==============================================================================

    TabbedPanelComponent.h
    Created: 22 Feb 2019 5:02:08pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"

//==============================================================================
/*
*/
class TabbedPanelComponent    : public Component,
                                private Button::Listener
{
public:
	enum TabbedPanelColourId
	{
		tabUnselectedText = 0,
		tabSelectedText,
		tabUnselectedBackground,
		tabSelectedBackground,
		tabSelectedHighlight,

        numColours
	};

    enum TabbedPanelStyle
    {
        tabsVertical =0,
        tabsHorizontal
    };

    //==============================================================================
    TabbedPanelComponent();
    ~TabbedPanelComponent();

    //==============================================================================
    void addTab (Component* panel, String tabName);
    void switchToTab (const int tabNumber);
    Component* getComponentFromTab (const int tabNumber);
    Component* getComponentFromTab (const String tabName);
    Component* getComponentFromSelectedTab();

    //==============================================================================
    void setColour (TabbedPanelColourId id, Colour colourToSet);
    void setStyle (TabbedPanelStyle newStyle);

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    struct Tab
    {
        Tab (Component* panelToUse, String tabName)	: name (tabName), panel (panelToUse)
        {
            button = new TextButton (name);
            button->setButtonText ("");
            //button->setColour (TextButton::textColourOffId, PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText));
            //button->setColour (TextButton::textColourOnId, PLUME::UI::currentTheme.getColour(PLUME::colour::topPanelSubText));
            button->setColour (TextButton::buttonColourId, Colour (0x00000000));
            button->setColour (TextButton::buttonOnColourId, Colour (0x00000000));
        }

        ~Tab ()
        {
            button = nullptr;
        }

        ScopedPointer<TextButton> button;
        ScopedPointer<Component> panel;
        const String name;
    };

    //==============================================================================
    OwnedArray<Tab> tabs;
    OwnedArray<Colour> colours;
    TabbedPanelStyle style = tabsVertical;
    int selectedTab = 0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TabbedPanelComponent)
};
