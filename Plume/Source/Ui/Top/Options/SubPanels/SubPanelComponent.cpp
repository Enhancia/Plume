/*
  ==============================================================================

    SubPanelComponent.cpp
    Created: 26 Feb 2019 11:24:31am
    Author:  Alex

  ==============================================================================
*/

#include "SubPanelComponent.h"

//==============================================================================
SubPanelComponent::SubPanelComponent ()
{
}

SubPanelComponent::~SubPanelComponent()
{
    removeAllChildren();
    rows.clear();
}

void SubPanelComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;
    auto area = getLocalBounds();
    auto textArea = getLocalBounds().removeFromLeft (getWidth()/3).reduced (MARGIN);
    auto compArea = area.reduced (MARGIN);

    if (!rows.isEmpty())
    {
        for (auto* row : rows)
        {
            g.setColour (row->isSeparator() ? currentTheme.getColour (PLUME::colour::topPanelSubText)
                                            : currentTheme.getColour (PLUME::colour::topPanelMainText));

            g.setFont (row->isSeparator() ? PLUME::font::plumeFontBook.withHeight (15.0f)
                                          : PLUME::font::plumeFont.withHeight (11.0f));

            if (row->isSeparator() && row != rows.getFirst())
            {
                // Additional space between sections
                textArea.removeFromTop (MARGIN);
                compArea.removeFromTop(MARGIN);
            }

            g.drawText (row->name,
                        textArea.removeFromTop (row->height),
                        //row->isSeparator() ? Justification::centredRight : Justification::centredLeft,
                        Justification::centredLeft,
                        true);

            if (row->hasFeedback && row->feedbackCount != 0)
            {
                auto feedbackArea = compArea.withHeight (row->height)
                                            .withLeft (row->comp->getRight() + MARGIN);

                row->drawUploadFeedback (g, feedbackArea);
            }

            textArea.removeFromTop(MARGIN);
            compArea.removeFromTop(row->height + MARGIN);
        }
    }
}

void SubPanelComponent::resized()
{
    using namespace PLUME::UI;

    auto compArea = getLocalBounds().removeFromRight (getWidth()*2/3).reduced (MARGIN);

    if (!rows.isEmpty())
    {
        for (int i =0; i < rows.size(); i++)
        {
            Row& row = *rows[i];

            switch (row.type)
            {
                case Row::separator:
                    if (i != 0) compArea.removeFromTop (MARGIN); // Additional space between sections
                    /*
                    getChildComponent (i)->setBounds (jmin (row.name.length()*7, compArea.getX(),
                                                      compArea.getY(),
                                                      getWidth() - jmin (row.name.length()*7, compArea.getX()),
                                                      row.height);*/
                    getChildComponent (i)->setBounds (compArea.removeFromTop (row.height));
                    //compArea.removeFromTop (row.height);
                    break;

                case Row::toggle:
                    getChildComponent (i)->setBounds (compArea.removeFromTop (row.height).withWidth(row.height*3/2));
                    break;

                case Row::button:
                case Row::buttonWithFeedback:
                    getChildComponent (i)->setBounds (compArea.removeFromTop (row.height).withWidth(4*row.height));
                    break;

                default:
                    getChildComponent (i)->setBounds (compArea.removeFromTop (row.height));
            }
            
            compArea.removeFromTop (MARGIN);
        }
    }
}


void SubPanelComponent::addRow (String rowText, Component* rowCompToAdd, int height)
{
    rows.add (new Row (rowCompToAdd, rowText, Row::custom, height));
    addAndMakeVisible (rowCompToAdd);
}

void SubPanelComponent::addSeparatorRow (String rowText)
{
    Separator* sep = new Separator();

    rows.add (new Row (sep, rowText, Row::separator));
    addAndMakeVisible (sep);
}

void SubPanelComponent::addToggleRow (String rowText, String buttonID, bool initialState)
{
    ToggleButton* tggle = new ToggleButton();
    tggle->setComponentID (buttonID);
    tggle->setToggleState (initialState, dontSendNotification);
    tggle->setColour (ToggleButton::tickColourId,
                      getPlumeColour(topPanelMainText));
    tggle->setColour (ToggleButton::tickDisabledColourId,
                      getPlumeColour(topPanelSubText));

    rows.add (new Row (tggle, rowText, Row::toggle));
    addAndMakeVisible (tggle);
    tggle->addListener(this);
}

void SubPanelComponent::addButtonRow (String rowText, String buttonID, String buttonText, String buttonFeedbackString)
{
    TextButton* bttn = new TextButton (buttonID);
    bttn->setComponentID (buttonID);
    bttn->setColour (TextButton::textColourOffId,
                     getPlumeColour(topPanelMainText));
    bttn->setButtonText (buttonText);

    rows.add (new Row (bttn, rowText, buttonFeedbackString.isNotEmpty() ? Row::buttonWithFeedback : Row::button));
    rows.getLast()->feedbackString = buttonFeedbackString;
    
    if (rows.getLast()->hasFeedback)
    {
        const int rowNum = rows.size() - 1;

        bttn->onClick = [this, rowNum] ()
        { 
            Row& rowToStartFeedbackOn = *rows[rowNum];

            rowToStartFeedbackOn.feedbackCount = 0;
            rowToStartFeedbackOn.startTimerHz (30);
        };
    }

    addAndMakeVisible (bttn);
    bttn->addListener(this);
}

void SubPanelComponent::addLabelRow (String rowText, String labelID, String labelText)
{
    Label* lbl = new Label (labelID, labelText);
    lbl->setComponentID (labelID);
    lbl->setColour (Label::backgroundColourId, Colour (0x30000000));
    lbl->setFont (PLUME::font::plumeFont.withHeight (11.0f));
    lbl->setEditable (true, false, false);
    lbl->setMouseCursor (MouseCursor (MouseCursor::IBeamCursor));

    rows.add (new Row (lbl, rowText, Row::label));
    addAndMakeVisible (lbl);
    lbl->addListener(this);
}

void SubPanelComponent::addScannerRow (String rowText, String scannerID,
                                       const String& dialogBoxTitle,
                                       const File& initialFileOrDirectory,
                                       const String& filePatternsAllowed,
                                       File initialStoredFile, bool searchDirs)
{
    ScannerRowComponent* src = new ScannerRowComponent (scannerID, dialogBoxTitle,
                                                        initialFileOrDirectory, filePatternsAllowed,
                                                        initialStoredFile, searchDirs);


    rows.add (new Row (src, rowText, Row::scanner));
    addAndMakeVisible (src);
    src->addChangeListener(this);
}

void SubPanelComponent::changeListenerCallback (ChangeBroadcaster* cb)
{
    if (auto* scannerComp = dynamic_cast<ScannerRowComponent*> (cb))
    {
        fileScanned (scannerComp->getComponentID(), scannerComp->getFile());
    }
}

//==============================================================================
SubPanelComponent::ScannerRowComponent::ScannerRowComponent  (const String& scannerID,
                                                              const String &dialogBoxTitle, 
                                                              const File &initialFileOrDirectory,
                                                              const String &filePatternsAllowed,
                                                              const File& initialStoredFile,
                                                              bool searchDirs)
                                                              : searchDirectories (searchDirs)
{
    setComponentID (scannerID);
    lastFile = initialStoredFile;

    chooser.reset (new FileChooser (dialogBoxTitle, initialFileOrDirectory, filePatternsAllowed));
    addAndMakeVisible (*(scanButton = std::make_unique<TextButton> ("scanButton")));
    scanButton->setButtonText ("Browse");
    scanButton->setColour (TextButton::textColourOffId,
                           getPlumeColour(topPanelMainText));
    scanButton->addListener (this);
}

SubPanelComponent::ScannerRowComponent::~ScannerRowComponent()
{
    scanButton->removeListener (this);
    scanButton = nullptr;
    chooser = nullptr;
}

void SubPanelComponent::ScannerRowComponent::paint (Graphics& g)
{
    using namespace PLUME::UI;

    g.setColour (currentTheme.getColour (PLUME::colour::topPanelSubText));
    g.setFont (PLUME::font::plumeFont.withHeight (11.0f));

    g.drawFittedText (reducePathName (lastFile.getFullPathName(), 3, 2),
                      getLocalBounds().removeFromRight (getWidth() - 4*getHeight())
                                      .reduced (MARGIN, 0),
                      Justification::centredLeft,
                      1);
}

void SubPanelComponent::ScannerRowComponent::resized()
{
    scanButton->setBounds (getLocalBounds().removeFromLeft (jmin (4*getHeight(), getWidth()/2)));
}

void SubPanelComponent::ScannerRowComponent::buttonClicked (Button* bttn)
{
    if (bttn == scanButton.get())
    {
        if (searchDirectories)
        {
            auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories;
         
            chooser->launchAsync(folderChooserFlags, [this](const FileChooser& fchsr)
            {
                lastFile = fchsr.getResult();
                PLUME::log::writeToLog("Changed " + getComponentID() + " : " + lastFile.getFullPathName(), PLUME::log::options);
                sendChangeMessage();
                repaint();
            });
        }
        else
        {
            //if (chooser->browseForFileToOpen())
            {
                lastFile = chooser->getResult();
                PLUME::log::writeToLog ("Changed " + getComponentID() + " : " + lastFile.getFullPathName(), PLUME::log::options);
                sendChangeMessage();
                repaint();
            }
        }
    }
}

const File SubPanelComponent::ScannerRowComponent::getFile()
{
    return lastFile;
}

String SubPanelComponent::ScannerRowComponent::reducePathName (String pathToReduce,
                                                               int numFoldersLeft,
                                                               int numFoldersRight)
{
    String tempString = pathToReduce;
    int indexLeft = 0;
    int indexRight = pathToReduce.length();

    // Gets left index for trimming
    for (int i=0; i<numFoldersLeft; i++)
    {
        int indexLeftSubstring = tempString.indexOfChar ('/');
        if (indexLeftSubstring == -1) indexLeftSubstring = tempString.indexOfChar ('\\');

        if (indexLeftSubstring == -1)
        {
            if (i == 0) return pathToReduce;

            break;
        }
        else
        {
            indexLeft += indexLeftSubstring + 1;
            tempString = pathToReduce.substring (indexLeft);
        }
    }

    tempString = pathToReduce;

    // Gets right index for trimming
    for (int i=0; i<numFoldersRight; i++)
    {
        int indexRightSubstring = tempString.lastIndexOfChar ('/');
        if (indexRightSubstring == -1) indexRightSubstring = tempString.lastIndexOfChar ('\\');

        if (indexRightSubstring == -1)
        {
            break;
        }
        else
        {
            indexRight -= tempString.length() - indexRightSubstring;
            tempString = pathToReduce.substring (0, indexRight);
        }
    }

    if (indexLeft >= indexRight) return pathToReduce;

    return (pathToReduce.replaceSection (indexLeft, indexRight - indexLeft, "..."));
}
