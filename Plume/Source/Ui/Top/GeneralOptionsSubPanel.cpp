/*
  ==============================================================================

    GeneralOptionsSubPanel.cpp
    Created: 25 Feb 2019 11:30:56am
    Author:  Alex

  ==============================================================================
*/

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "GeneralOptionsSubPanel.h"

//==============================================================================
GeneralOptionsSubPanel::GeneralOptionsSubPanel()
{
	addRow ("Je", new Separator());
	addRow ("Suis", new Separator());
	addRow ("Le", new Separator());
	addRow ("Lord", new Separator());
	addRow ("Jambon", new Separator());
}

GeneralOptionsSubPanel::~GeneralOptionsSubPanel()
{
}
