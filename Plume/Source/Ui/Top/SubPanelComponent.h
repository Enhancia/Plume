/*
  ==============================================================================

    SubPanelComponent.h
    Created: 26 Feb 2019 11:24:31am
    Author:  Alex

  ==============================================================================
*/

#pragma once

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "Common/PlumeCommon.h"
#include "Plugin/PluginProcessor.h"

//==============================================================================
/*
*/
class SubPanelComponent    : public Component,
                             public Label::Listener,
                             public Button::Listener
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

    //==============================================================================
    void addRow (String rowText, Component* rowCompToAdd, int height = 16);
    void addSeparatorRow (String rowText);
    void addToggleRow (String rowText, String buttonID);
    void addButtonRow (String rowText, String buttonID, String buttonText);
    void addLabelRow (String rowText, String labelID, String labelText);

private:
    //==============================================================================
    class Separator : public Component
    {
    public:
        Separator() {}
        ~Separator() {}

        void paint(Graphics& g) override
        {
            g.setColour(Colour (0x50ffffff));
            //g.fillAll();
            g.drawHorizontalLine(this->getHeight() / 2, 0.0f, float (this->getWidth()));
        }
        void resized() override { repaint();  }

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
            value,

            custom
        };

        Row (Component* compToUse, String rowName, rowType t, int rowH = 16) : name (rowName), height (rowH)
        {
            comp = compToUse;
            type = t;
        }

        ~Row () { comp = nullptr; }

        bool isSeparator() { return type == separator; }

        ScopedPointer<Component> comp;
        const String name;
        rowType type;
        const int height;
    };

    //==============================================================================
    OwnedArray<Row> rows;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubPanelComponent)
};
