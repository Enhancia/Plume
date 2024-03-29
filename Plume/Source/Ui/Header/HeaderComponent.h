/*
  ==============================================================================

    HeaderComponent.h
    Created: 12 Dec 2018 4:53:18pm
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../../Common/PlumeCommon.h"
#include "../../Plugin/PluginProcessor.h"
#include "../Gesture/GesturePanel.h"
#include "../Common/DualTextToggle.h"

//==============================================================================
/*
*/
class HeaderComponent    : public Component,
                           public PlumeComponent,
                           public Button::Listener,
                           private AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    HeaderComponent (PlumeProcessor& proc, Component& newPrst);
    ~HeaderComponent();

    //==============================================================================
    // PlumeComponent
    const String getInfoString() override;
    void update() override;
    void setBatteryVisible (bool shouldBeVisible);
    
    //==============================================================================
    // Component
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    // Mouse Listener
    void mouseUp (const MouseEvent &event) override;
    void mouseEnter (const MouseEvent &event) override;
    void mouseExit (const MouseEvent &event) override;
    void buttonClicked (Button* bttn) override;
    void parameterChanged (const String &parameterID, float newValue) override;
    
    //==============================================================================
    void createPluginMenu (KnownPluginList::SortMethod sort);
    void createPluginWindow();
    void createPresetOptionsMenu(bool showNew = true, bool showSave = true, bool showEdit = true, bool showDelete = true, bool showInExplorer = true);

    //==============================================================================
    void setPreviousPreset();
    void setNextPreset();

private:

	class BatteryComponent : public Component, public MultiTimer
	{

	public:
        //==========================================================================
        enum timerIds
        {
            batteryCheckTimer = 0,
            blinkTimer = 1,
            delayedRepaintTimer = 2
		};

        //DataReader& getDataReader();
        //void setDataReader (DataReader* pDataReader);

        //==========================================================================
        BatteryComponent (const float& batteryValRef, DataReader& dataReaderRef);
		~BatteryComponent() override;

        //==========================================================================
        void paint (Graphics& g) override;
        void resized() override { repaint(); };

        //==========================================================================
        void timerCallback (int timerID) override;

        //==========================================================================
        void repaintIfNeeded (bool forceRepaint = false);
        void repaintBlinkingIndicators ();
        void update();

        //==========================================================================
        const float& batteryValueRef;

    private:
        //==========================================================================
        void launchDelayedRepaint (const int delayMs, bool forceRepaint = false);
		    void drawLightningPath (Graphics& g, juce::Rectangle<float> area);

        /**
         * @brief Draws colored dots which represents battery level
         * @param g Graphics context to use
         * @param area Rectangle reference in which to draw
        */
        void drawBatteryPath (Graphics& g, juce::Rectangle<float> area);

        /**
         * @brief Draws arrows and red cross that represent connection lost with ring, and a red cross if connection is lost.
         * @param g Graphics context to use
         * @param area Rectangle reference in which to draw
        */
        void drawConnectedPath (Graphics& g, juce::Rectangle<float> area);

        /**
         * @brief Draws ring path in specified area.
         * @param g Graphics context to use
         * @param area Rectangle reference in which to draw
        */
        void drawRingPath (Graphics& g, juce::Rectangle<float> area);

        bool waitForRepaint = false;
        DataReader& dataReader;

        //==========================================================================
        //HubConfiguration& hubConfig;
        bool lastChargeState = false;
        bool lastConnectionState = false;

        //==========================================================================
        float lastBattery = -1.0f;
        float lastRawBattery = 3.0f;
        int numIndicators = 0;
        int numBlinkingIndicators = 0;
        juce::Rectangle<int> indicatorsArea;
        bool blinkState = false;
        bool mForceRepaint = false;

        //==========================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BatteryComponent)
	};

    //==============================================================================
	static void pluginMenuCallback (int result, HeaderComponent* header);
    static void presetOptionsMenuCallback(int result, HeaderComponent* header);
    void handlePluginChoice (int chosenId);
    void createButtons();
    void setPresetWithOffset (const int offset);
    void prepareGesturePanelAndLoadPreset (const int presetId);

    //==============================================================================
    PlumeProcessor& processor;
    Component& userPresetPanel;
    
    PopupMenu pluginListMenu;
    PopupMenu presetOptionsMenu;
    std::unique_ptr<PlumeShapeButton> pluginListButton;
    std::unique_ptr<PlumeShapeButton> savePresetButton;
    std::unique_ptr<PlumeShapeButton> presetOptionsButton;
    std::unique_ptr<PlumeShapeButton> leftArrowButton;
    std::unique_ptr<PlumeShapeButton> rightArrowButton;
    std::unique_ptr<PlumeShapeButton> trackArmButton;
    std::unique_ptr<Label> pluginNameLabel;
    std::unique_ptr<Label> presetNameLabel;
    
    std::unique_ptr<BatteryComponent> batteryComponent;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderComponent)
};
