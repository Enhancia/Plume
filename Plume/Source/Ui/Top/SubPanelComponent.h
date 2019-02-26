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
class SubPanelComponent    : public Component
{
public:
    //==============================================================================
    SubPanelComponent();
    ~SubPanelComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //==============================================================================
    void addRow (String name, Component* rowCompToAdd);
    
protected:
    //==============================================================================
	class Separator : public Component
    {
    public:
        Separator() {}
        ~Separator() {}

        void paint(Graphics& g) override
        {
            g.setColour(Colour(0xff000000));
            //g.fillAll();
            g.drawHorizontalLine(this->getHeight() / 2, 0.0f, float (this->getWidth()));
        }
		void resized() override { repaint();  }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Separator)
    };

private:
    //==============================================================================
    class Row
    {
	public:
        Row (Component* compToUse, String rowName) : name (rowName)
        {
            comp = compToUse;
        }

        ~Row () { comp = nullptr; }

        ScopedPointer<Component> comp;
        const String name;
    };

    //==============================================================================
    OwnedArray<Row> rows;

    const int rowHeight = 16;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SubPanelComponent)
};
