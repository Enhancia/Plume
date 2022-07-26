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
    void setTabAlert (const int tabNumber, const bool alert);
    void setTabAlert (const String tabName, const bool alert);

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

private:
    //==============================================================================
    struct Tab
    {
        class TabButton : public Button
        {
        public:
            TabButton(String tabName) : Button(tabName + String("Button")) {}
            ~TabButton() {}

            void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
            {
                if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
                {
                    g.setColour(PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelSubText).withAlpha(shouldDrawButtonAsDown ? 0.2f : 0.05f));
                    g.fillRect(getLocalBounds());
                }
            }

        private:
            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabButton)
        };

        Tab (Component* panelToUse, String tabName) : name (tabName), panel (panelToUse)
        {
            button.reset (new TabButton (name));
        }

        ~Tab ()
        {
            button = nullptr;
        }

        std::unique_ptr<TabButton> button;
        const String name;
        std::unique_ptr<Component> panel;
        bool alert = false;
    };

    //==============================================================================
    juce::Rectangle<int> panelArea, tabsArea;
    OwnedArray<Tab> tabs;
    OwnedArray<Colour> colours;
    TabbedPanelStyle style = tabsHorizontal;
    int selectedTab = 0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TabbedPanelComponent)
};
