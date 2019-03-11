/*
  ==============================================================================

    SubPanelComponent.cpp
    Created: 26 Feb 2019 11:24:31am
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
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

    auto textArea = getLocalBounds().removeFromLeft (getWidth()/4).reduced (MARGIN);

    if (!rows.isEmpty())
    {
		for (auto* row : rows)
		{
			g.setColour (row->isSeparator() ? currentTheme.getColour (PLUME::colour::topPanelSubText)
											: currentTheme.getColour (PLUME::colour::topPanelMainText));

    		g.setFont (row->isSeparator() ? PLUME::font::plumeFontBook.withHeight (14.0f)
    			                          : PLUME::font::plumeFont.withHeight (13.0f));

    		if (row->isSeparator() && row != rows.getFirst())
    		{
    			textArea.removeFromTop (MARGIN);  // Additional space between sections
    		}

    		g.drawText (row->name, 
                		textArea.removeFromTop (row->height),
                		//row->isSeparator() ? Justification::centredRight : Justification::centredLeft,
                		Justification::centredLeft,
                		false);

    		textArea.removeFromTop (MARGIN);
		}
    }
}

void SubPanelComponent::resized()
{
    using namespace PLUME::UI;

    auto area = getLocalBounds();
    auto compArea = getLocalBounds().removeFromRight (getWidth()*3/4).reduced (MARGIN);

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
        			getChildComponent (i)->setBounds (compArea.removeFromTop (row.height).withWidth(row.height));
        			break;

        		case Row::button:
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

void SubPanelComponent::addToggleRow (String rowText, String buttonID)
{
	ToggleButton* tggle = new ToggleButton();
	tggle->setComponentID (buttonID);

	rows.add (new Row (tggle, rowText, Row::toggle));
	addAndMakeVisible (tggle);
	tggle->addListener(this);
}

void SubPanelComponent::addButtonRow (String rowText, String buttonID, String buttonText)
{
	TextButton* bttn = new TextButton (buttonID);
	bttn->setComponentID (buttonID);
	bttn->setColour (TextButton::textColourOffId,
		             PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText));
	bttn->setButtonText (buttonText);

	rows.add (new Row (bttn, rowText, Row::button));
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

	chooser = new FileChooser (dialogBoxTitle, initialFileOrDirectory, filePatternsAllowed);
	addAndMakeVisible (scanButton = new TextButton ("scanButton"));
	scanButton->setButtonText ("Search");
	scanButton->setColour (TextButton::textColourOffId,
		                   PLUME::UI::currentTheme.getColour (PLUME::colour::topPanelMainText));
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

	g.drawFittedText (lastFile.getFullPathName(),
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
	if (bttn == scanButton)
	{
		if (searchDirectories)
		{
			if (chooser->browseForDirectory())
			{
				lastFile = chooser->getResult();
				sendChangeMessage();
				repaint();
			}
		}
		else
		{
			if (chooser->browseForFileToOpen())
			{
				lastFile = chooser->getResult();
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
