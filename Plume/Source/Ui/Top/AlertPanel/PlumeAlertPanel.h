/*
  ==============================================================================

    PlumeAlertPanel.h
    Created: 25 Sep 2019 4:35:26pm
    Author:  Enhancia Dev

  ==============================================================================
*/

#pragma once

#include "../../../../JuceLibraryCode/JuceHeader.h"
#include "../../../Common/PlumeCommon.h"
#include "../../Common/PlumeShapeButton.h"

//==============================================================================
/*
*/
class PlumeAlertPanel    : public Component,
                           private Button::Listener
{
public:
    //==============================================================================
    enum SpecificReturnValue
    {
        missingPlugin = 1,
        missingScript,
        scanRequired,
        mappingOverwrite,
        scanCrashed,
        plumeCrashed,
        unknown
    };

    //==============================================================================
    PlumeAlertPanel (const String& title, const String& message,
                                         int returnValue = 0,
                                         const bool hasCloseButton = true,
                                         const String& buttonText = String());
    ~PlumeAlertPanel();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    //==============================================================================
    void buttonClicked (Button* bttn) override;

    bool keyPressed (const KeyPress& key) override;

    //==============================================================================
    static PlumeAlertPanel* createSpecificAlertPanel (SpecificReturnValue panelType, const String& specificText = "");

private:
    //==============================================================================
    void createAndAddTextEditor (const String& textToSet);
    void createAndAddLabel (const String& textToSet);
    void createAndAddButtons (const String& buttonText, const bool addCloseButton = true);

    //==============================================================================
    juce::Rectangle<int> panelArea;
    bool showButton = false;

    //==============================================================================
    std::unique_ptr<Label> bodyText;
    std::unique_ptr<Label> titleLabel;
    std::unique_ptr<TextButton> okButton;
    std::unique_ptr<PlumeShapeButton> closeButton;

    int modalReturnValue = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlumeAlertPanel)
};


