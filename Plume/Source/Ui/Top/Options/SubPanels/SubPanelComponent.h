/*
  ==============================================================================

    SubPanelComponent.h
    Created: 26 Feb 2019 11:24:31am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../../Common/PlumeCommon.h"
#include "../../../../Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class SubPanelComponent    : public Component,
                             public Label::Listener,
                             public Button::Listener,
                             private ChangeListener
{
public:
    //==============================================================================
    SubPanelComponent();
    ~SubPanelComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    virtual void buttonClicked (Button*) override {}
    virtual void labelTextChanged (Label*) override {}
    virtual void fileScanned (const String&, const File&) {}

    //==============================================================================
    void addRow (String rowText, Component* rowCompToAdd, int height =PLUME::UI::SUBPANEL_ROW_HEIGHT);
    void addSeparatorRow (String rowText);
    void addToggleRow (String rowText, String buttonID, bool initialState =false);
    void addButtonRow (String rowText, String buttonID, String buttonText, String feedbackString = String());
    void addLabelRow (String rowText, String labelID, String labelText);
    void addScannerRow (String rowText, String scannerID,
                        const String& dialogBoxTitle,
                        const File& initialFileOrDirectory =File(),
                        const String& filePatternsAllowed =String(),
                        File initialStoredFile =File(), bool searchDirs = false);
 




private:
    //==============================================================================
    class Separator : public Component
    {
    public:
        Separator() {}
        ~Separator() {}

        void paint(Graphics& g) override
        {
            g.setColour(PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelSubText));
            g.drawHorizontalLine(this->getHeight() / 2, 0.0f, float (this->getWidth()));
        }
        void resized() override { repaint(); }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Separator)
    };

    //==============================================================================
    class Row : public Timer
    {
	public:
        enum rowType
        {
            separator =0,
            toggle,
            button,
            buttonWithFeedback,
            label,
            scanner,
            value,

            custom
        };

        Row (Component* compToUse, String rowName, rowType t, int rowH =PLUME::UI::SUBPANEL_ROW_HEIGHT)
            : name (rowName), height (rowH), type (t == buttonWithFeedback ? button : t), comp (compToUse), hasFeedback (t == buttonWithFeedback)
        {
        }

        ~Row () { comp = nullptr; }

        bool isSeparator() { return type == separator; }
        void timerCallback() override
        {
            if (hasFeedback && feedbackCount < feedbackMax)
            {
                comp->getParentComponent()->repaint();
                feedbackCount++;
            }
            else if (feedbackCount == feedbackMax)
            {
                feedbackCount = 0;
                stopTimer();
            }
        }

        void drawUploadFeedback (Graphics& g, juce::Rectangle<int> area)
        {
            if (!hasFeedback) return;

            const float animationProgress = feedbackCount/float (feedbackMax);
            const float alpha = animationProgress < 0.5f ? 1.0f
                                                         : 1.0f - std::pow (animationProgress*2 - 1.0f, 2.0f);

            g.setColour (PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText).withAlpha (alpha));

            g.setFont (PLUME::font::plumeFont.withHeight (11.0f));
            g.drawText (feedbackString, area, Justification::centredLeft);
        }

        const String name;
        const int height;
        const rowType type;
        const bool hasFeedback;
        int feedbackCount = 0;
        const int feedbackMax = 25;
        String feedbackString;
        std::unique_ptr<Component> comp;
    };

    //==============================================================================
    class ScannerRowComponent : public Component,
                                public ChangeBroadcaster,
                                private Button::Listener
    {
    public:
        ScannerRowComponent  (const String& scannerID,
                              const String &dialogBoxTitle, 
                              const File &initialFileOrDirectory=File(),
                              const String &filePatternsAllowed=String(),
                              const File& initialStoredFile =File(),
                              bool searchDirs =false);
        ~ScannerRowComponent();

        void paint (Graphics&) override;
        void resized() override;
        void buttonClicked (Button*) override;

        const File getFile();

    private:
        String reducePathName (String pathToReduce, int numFoldersLeft, int numFoldersRight);

        std::unique_ptr<FileChooser> chooser;
        std::unique_ptr<TextButton> scanButton;
        File lastFile;
        const bool searchDirectories;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ScannerRowComponent)
    };

    //==============================================================================
    void changeListenerCallback (ChangeBroadcaster*) override;

    //==============================================================================
    OwnedArray<Row> rows;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubPanelComponent)
};
