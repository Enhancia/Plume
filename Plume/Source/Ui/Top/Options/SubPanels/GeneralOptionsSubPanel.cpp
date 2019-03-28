/*
  ==============================================================================

    GeneralOptionsSubPanel.cpp
    Created: 25 Feb 2019 11:30:56am
    Author:  Alex

  ==============================================================================
*/

#include "../../../../../JuceLibraryCode/JuceHeader.h"
#include "GeneralOptionsSubPanel.h"

//==============================================================================
GeneralOptionsSubPanel::GeneralOptionsSubPanel (PlumeProcessor& proc) : processor (proc)
{
	addSeparatorRow ("Bug Report");
	addButtonRow ("Send report", "mailB", "Report");
}

GeneralOptionsSubPanel::~GeneralOptionsSubPanel()
{
}

void GeneralOptionsSubPanel::buttonClicked (Button* bttn)
{
	TRACE_IN;
    
    if (bttn->getComponentID() == "mailB")
    {
        if (auto* plumeLogger = dynamic_cast<FileLogger*> (Logger::getCurrentLogger()))
        {
            String fullLog = plumeLogger->getLogFile().loadFileAsString().removeCharacters ("\n");
            
            /* Only keeps the last 3 entries of the log: the one that had an issue, 
               the one used to send the report, and one more to cover cases where the plugin
               has to be checked (For instance the plugin check when launching Ableton..)
               If too long, keeps the 6000 last characters.. */
            int startIndex = jmax (fullLog.upToLastOccurrenceOf("Plume Log", false, false)
                                          .upToLastOccurrenceOf("Plume Log", false, false)
								          .lastIndexOf("Plume Log"),
								          fullLog.length() - 6000);
			
		      #if JUCE_WINDOWS					          
			      String mail_str ("mailto:damien.leboulaire@enhancia.co"
                             "?Subject=[Plume Report]"
			                       "&cc=alex.levacher@enhancia.co"
		                         "&body=" + fullLog.substring (startIndex));
		        LPCSTR mail_lpc = mail_str.toUTF8();

            ShellExecute (NULL, "open", mail_lpc,
		                      "", "", SW_SHOWNORMAL);

		      #elif JUCE_MAC					          
			      String mail_str ("open mailto:damien.leboulaire@enhancia.co"
                             "?Subject=\"[Plume Report]\""
			                       "\\&cc=alex.levacher@enhancia.co"
		                         "\\&body=\"" + fullLog.substring (startIndex) + "\"");
		    
		        system (mail_str.toUTF8());
		      #endif
        }
    }
}