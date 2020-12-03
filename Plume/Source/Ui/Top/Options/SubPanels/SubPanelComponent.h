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
    void addButtonRow (String rowText, String buttonID, String buttonText);
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
    class Row
    {
	public:
        enum rowType
        {
            separator =0,
            toggle,
            button,
            label,
            scanner,
            value,

            custom
        };

        Row (Component* compToUse, String rowName, rowType t, int rowH =PLUME::UI::SUBPANEL_ROW_HEIGHT)
            : name (rowName), height (rowH), type (t), comp (compToUse)
        {
        }

        ~Row () { comp = nullptr; }

        bool isSeparator() { return type == separator; }

        ScopedPointer<Component> comp;
        const String name;
        const rowType type;
        const int height;
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

        ScopedPointer<FileChooser> chooser;
        ScopedPointer<TextButton> scanButton;
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
