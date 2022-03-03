/*
  ==============================================================================

    BugReportPanel.cpp
    Created: 15 Apr 2021 11:01:43am
    Author:  alexl

  ==============================================================================
*/

#include "BugReportPanel.h"

//==============================================================================
BugReportPanel::BugReportPanel()
    : credentials (Base64::toBase64 (key + String(":") + code))
{
    createLabels();
    createButtons();

    setComponentID ("BugReportPanel");
}

BugReportPanel::~BugReportPanel()
{
    nameLabel->removeListener (this);
    mailLabel->removeListener (this);
}

void BugReportPanel::paint (Graphics& g)
{
    // firmware panel area
    g.setColour (getPlumeColour (topPanelBackground));
    g.fillRoundedRectangle (panelArea.reduced (1).toFloat(), 10.0f);

    if (currentStep <= incorrectReport)
    {
        g.setColour (getPlumeColour (topPanelSubText));
        g.setFont (PLUME::font::plumeFont/*Norms*/.withHeight (16.0f));
        g.drawFittedText ("Help us improve your Plume experience !",
                          panelArea.withTrimmedTop (titleLabel->getHeight()).withBottom (nameLabel->getY()),
                          Justification::centred, 3);
    }
    else
    {
        g.setColour (getPlumeColour (topPanelMainText));
        g.setFont (PLUME::font::plumeFont/*Norms*/.withHeight (18.0f));

        if (currentStep == reportSentOk)
        {
            g.drawFittedText ("Thank you for your feedback !\n\nWe will take your input into account.",
                              panelArea.withTrimmedTop (titleLabel->getHeight()).withBottom (bottomButton->getY()),
                              Justification::centred, 3);
        }
        if (currentStep == reportSentError)
        {
            g.drawFittedText ("Error !\n\nFailed to send bug report... Please try again later.",
                              panelArea.withTrimmedTop (titleLabel->getHeight()).withBottom (bottomButton->getY()),
                              Justification::centred, 3);
        }
    }
}

void BugReportPanel::resized()
{
    using namespace PLUME::UI;

    panelArea = getBounds().reduced (getWidth()/5, getHeight()/10);
    
    // Close Button
    #if JUCE_WINDOWS
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withRightX (panelArea.getRight() - MARGIN_SMALL)
                                                         .withY (panelArea.getY() + MARGIN_SMALL));
    #elif JUCE_MAC
    closeButton->setBounds (juce::Rectangle<int> (25, 25).withPosition (panelArea.getTopLeft()
                                                                                 .translated (MARGIN_SMALL,
                                                                                              MARGIN_SMALL)));
    #endif

    auto area = panelArea.reduced (MARGIN);

    titleLabel->setBounds (area.removeFromTop (area.getHeight()/6));

    auto buttonArea = area.removeFromBottom (jmin (area.getHeight()/6, 30));
    area.removeFromTop (30);
    auto topLabelsArea = area.removeFromTop (jmin (area.getHeight()/5, 30));

    messageEditor->setBounds (area.reduced (MARGIN));

    nameLabel->setBounds (topLabelsArea.removeFromLeft (topLabelsArea.getWidth()/2).reduced (MARGIN, 0));
    mailLabel->setBounds (topLabelsArea.reduced (MARGIN, 0));

    bottomButton->setBounds (buttonArea.withSizeKeepingCentre (bottomButton->getBestWidthForHeight (buttonArea.getHeight()),
                                                               buttonArea.getHeight()));

    if (currentStep <= incorrectReport)
    {
        bottomButton->setBounds (bottomButton->getBounds().withRightX (messageEditor->getRight()));
    }
}

void BugReportPanel::buttonClicked (Button* bttn)
{
    if (bttn == closeButton.get())
    {
        closeAndResetPanel();
    }

    else if (bttn == bottomButton.get())
    {
        if (currentStep == newReport || currentStep == incorrectReport)
        {
            if (!formSeemsCorrect())
            {
                updateComponentsForSpecificStep (incorrectReport);
            }
            else
            {
                sendTicketAndUpdate();
            }
        }
        else
        {
            closeAndResetPanel();
        }
    }
}

bool BugReportPanel::keyPressed (const KeyPress& keyPress)
{
    if (keyPress == PLUME::keyboard_shortcut::closeWindow)
    {
        closeAndResetPanel ();
    }

    return false;
}

void BugReportPanel::labelTextChanged (Label*)
{
    checkFormEntry();

    nameLabel->setColour (Label::textColourId, nameIsCorrect ? getPlumeColour(topPanelMainText) : getPlumeColour(topPanelSubText));
    mailLabel->setColour (Label::textColourId, mailIsCorrect ? getPlumeColour(topPanelMainText) : getPlumeColour(topPanelSubText));
}

void BugReportPanel::resetAndOpenPanel()
{
    updateComponentsForSpecificStep (newReport);
    setVisible (true);
    grabKeyboardFocus();
}

void BugReportPanel::closeAndResetPanel()
{
    setVisible (false);
    updateComponentsForSpecificStep (newReport);
}

void BugReportPanel::createLabels()
{
    titleLabel.reset (new Label ("Title Label", ""));
    addAndMakeVisible (*titleLabel);
    titleLabel->setFont (PLUME::font::plumeFontBold.withHeight (25.0f));
    titleLabel->setJustificationType (Justification::centred);
    titleLabel->setText ("Bug Report", dontSendNotification);

    nameLabel.reset (new Label ("Name Label", ""));
    addAndMakeVisible (*nameLabel);
    nameLabel->setJustificationType (Justification::centredLeft);
    nameLabel->setEditable (true);
    nameLabel->setText ("Name*", dontSendNotification);
    nameLabel->setColour (Label::outlineColourId, getPlumeColour(topPanelSubText));
    nameLabel->setColour (Label::textColourId, getPlumeColour(topPanelSubText));
    nameLabel->setColour (Label::backgroundColourId, Colour (0x20000000));
    nameLabel->addListener (this);

    mailLabel.reset (new Label ("Mail Label", ""));
    addAndMakeVisible (*mailLabel);
    mailLabel->setJustificationType (Justification::centredLeft);
    mailLabel->setEditable (true);
    mailLabel->setText ("Email*", dontSendNotification);
    mailLabel->setColour (Label::outlineColourId, getPlumeColour(topPanelSubText));
    mailLabel->setColour (Label::textColourId, getPlumeColour(topPanelSubText));
    mailLabel->setColour (Label::backgroundColourId, Colour (0x20000000));
    mailLabel->addListener (this);

    messageEditor.reset (new TextEditor ("Message Label"));
    addAndMakeVisible (*messageEditor);
    messageEditor->setJustification (Justification::topLeft);
    messageEditor->setBorder (BorderSize<int> (PLUME::UI::MARGIN_SMALL));
    messageEditor->setMultiLine (true);
    messageEditor->setReturnKeyStartsNewLine (true);
    messageEditor->setInputRestrictions (1000);
    messageEditor->setFont (PLUME::font::plumeFont.withHeight (14.0f));
    messageEditor->setTextToShowWhenEmpty ("Message", getPlumeColour(topPanelSubText));
    messageEditor->setColour (TextEditor::outlineColourId, getPlumeColour(topPanelSubText));
    messageEditor->setColour (TextEditor::focusedOutlineColourId, getPlumeColour(topPanelSubText));
    messageEditor->setColour (TextEditor::textColourId, getPlumeColour(topPanelMainText));
    messageEditor->setColour (TextEditor::backgroundColourId, Colour (0x20000000));
}

void BugReportPanel::createButtons()
{
    // Bottom buttons
    bottomButton.reset (new TextButton ("Bottom Button"));
    addAndMakeVisible (*bottomButton);
    bottomButton->setButtonText ("Send Report");
    bottomButton->addListener (this);

    // Close button
    closeButton = std::make_unique <ShapeButton> ("Close Send Report Button",
                                                       Colour(0),
                                                       Colour(0),
                                                       Colour(0));
    addAndMakeVisible (*closeButton);
    Path p;
    p.startNewSubPath (0, 0);
    p.lineTo (3*PLUME::UI::MARGIN, 3*PLUME::UI::MARGIN);
    p.startNewSubPath (0, 3*PLUME::UI::MARGIN);
    p.lineTo (3*PLUME::UI::MARGIN, 0);
    closeButton->setShape (p, false, true, false);

    closeButton->setOutline (PLUME::UI::currentTheme.getColour(PLUME::colour::topPanelMainText), 1.0f);
    closeButton->setBorderSize (BorderSize<int> (6));
    closeButton->addListener (this);
}

void BugReportPanel::updateComponentsForSpecificStep (ReportStep stepToUpgradeTo)
{
    currentStep = stepToUpgradeTo;

    switch (stepToUpgradeTo)
    {
        case newReport:
            bottomButton->setButtonText ("Send Report");
            nameLabel->setVisible (true);
            nameLabel->setText ("Name*", sendNotification);
            nameLabel->setColour (Label::outlineColourId, getPlumeColour(topPanelSubText));
            mailLabel->setVisible (true);
            mailLabel->setText ("Email*", sendNotification);
            mailLabel->setColour (Label::outlineColourId, getPlumeColour(topPanelSubText));
            messageEditor->setVisible (true);
            break;
        case incorrectReport:
            bottomButton->setButtonText ("Send Report");
            nameLabel->setVisible (true);
            nameLabel->setColour (Label::outlineColourId, nameIsCorrect ? getPlumeColour(topPanelSubText) : Colours::red);
            mailLabel->setVisible (true);
            mailLabel->setColour (Label::outlineColourId, mailIsCorrect ? getPlumeColour(topPanelSubText) : Colours::red);
            messageEditor->setVisible (true);
            break;
        case reportSentOk:
            PLUME::log::writeToLog ("Bug report sent succesfully.", PLUME::log::LogCategory::options);
            bottomButton->setButtonText ("Ok");
            nameLabel->setVisible (false);
            mailLabel->setVisible (false);
            messageEditor->setVisible (false);
            break;
        case reportSentError:
            PLUME::log::writeToLog ("Failed to send bug report...", PLUME::log::LogCategory::options, PLUME::log::LogLevel::error);
            bottomButton->setButtonText ("Ok");
            nameLabel->setVisible (false);
            mailLabel->setVisible (false);
            messageEditor->setVisible (false);
            break;
        default:
            break;
    }

    resized();
    repaint();
}

//==============================================================================
bool BugReportPanel::formSeemsCorrect()
{
    return nameIsCorrect && mailIsCorrect;
}

void BugReportPanel::checkFormEntry()
{
    nameIsCorrect = nameLabel->getText().isNotEmpty() && nameLabel->getText() != "Name*"
                                                      && nameLabel->getText() != "Name";

    mailIsCorrect = (mailLabel->getText().isNotEmpty() && mailLabel->getText() != "Email*"
                                                       && mailLabel->getText() != "Email"
                                                       && mailLabel->getText().containsChar ('@')
                                                       && mailLabel->getText().upToLastOccurrenceOf ("@", false, false)
                                                                              .isNotEmpty()
                                                       && mailLabel->getText().fromLastOccurrenceOf ("@", false, false)
                                                                              .containsChar('.')
                                                       && mailLabel->getText().fromLastOccurrenceOf ("@", false, false)
                                                                              .removeCharacters(".")
                                                                              .isNotEmpty());
}

//==============================================================================
void BugReportPanel::sendTicketAndUpdate()
{
    URL ticketURL = createURLForTicket();

    // Gets response headers and display them
    const String headers ("\r\nAuthorization: Basic " + credentials + "\r\n"
                          "Content-Type: multipart/form-data;boundary=" + mimeBoundary + "\r\n");
    int statusCode;
    StringPairArray responseHeaders;

    std::unique_ptr<InputStream> webStream (ticketURL.createInputStream (true,
                                                                           nullptr,
                                                                           nullptr,
                                                                           headers,
                                                                           10000,
                                                                           &responseHeaders,
                                                                           &statusCode,
                                                                           5,
                                                                           "POST"));


    if (webStream == nullptr)
    {
        PLUME::log::writeToLog ("Failed to create an Input Stream to send the ticket..", PLUME::log::LogCategory::bugReport, PLUME::log::LogLevel::error);
        updateComponentsForSpecificStep (reportSentError);
        return;
    }
    
    if (dynamic_cast<WebInputStream*> (webStream.get())->isError() || statusCode >= 400)
    {
        const String response = webStream->readEntireStreamAsString();
        const String errorMessage =  ("Failed to send ticket .. \n Stream error    ? "
                                       + String (dynamic_cast<WebInputStream*> (webStream.get())->isError() ? "Yes" : "No")
                                       + "\n Code            : " + String (statusCode)
                                       + "\n Request  Headers: " + dynamic_cast<WebInputStream*> (webStream.get())->getRequestHeaders()
                                                                                                                    .getDescription()
                                       + "\n Response Headers: " + responseHeaders.getDescription()
                                       + "\n Response        : " + response);

        PLUME::log::writeToLog (errorMessage, PLUME::log::LogCategory::bugReport, PLUME::log::LogLevel::error);
        updateComponentsForSpecificStep (reportSentError);
        return;
    }

    updateComponentsForSpecificStep (reportSentOk);
}

URL BugReportPanel::createURLForTicket()
{
    URL happyFoxURL;
    happyFoxURL = happyFoxURL.withNewDomainAndPath ("https://enhancia.happyfox.com/api/1.1/json/tickets/");

    // Fills file array
    Array<File> filesToAttach;
    getFilestoAttach (filesToAttach);

    // Creates POST data and attaches it to URL
    String data = createMultipartData (mimeBoundary, filesToAttach);
    happyFoxURL = happyFoxURL.withPOSTData (data);

    return happyFoxURL;
}

String BugReportPanel::createMultipartData (const String& boundary, const Array<File>& txtFilesToAttach)
{
    String multipartData;

    // Individual params
    auto addFormDataParameter = [] (String& dataToAppendTo, const String paramName, const String paramValue, const String boundary)
    {
        dataToAppendTo += "--" + boundary + "\r\n";
        dataToAppendTo += "Content-Disposition: form-data; name=\"" + paramName + "\"\r\n\r\n"
                                                                    + paramValue
                                                                    + "\r\n";
    };

    addFormDataParameter (multipartData, "subject", "\"Plume Bug Report\"", boundary);
    addFormDataParameter (multipartData, "text", "\"" + messageEditor->getText() + "\"", boundary);
    addFormDataParameter (multipartData, "category", "9", boundary);
    addFormDataParameter (multipartData, "priority", "1", boundary);
    addFormDataParameter (multipartData, "email", mailLabel->getText(), boundary);
    addFormDataParameter (multipartData, "name", nameLabel->getText(), boundary);

    // Log files
    for (auto file : txtFilesToAttach)
    {
        multipartData += "--" + boundary + "\r\n";
        multipartData += "Content-Disposition: form-data; name=\"attachments\"; filename=\"" + file.getFileName() + "\"\r\n"
                         "Content-Type: text/plain\r\n\r\n";
        // MAYBE TODO automotic Content-Type, if we link other files eventually

        multipartData += file.loadFileAsString() + "\r\n";
    }

    // End
    multipartData += "--" + boundary + "--";

    return multipartData;
}

void BugReportPanel::getFilestoAttach (Array<File>& fileArrayToFill)
{
    fileArrayToFill.add (PLUME::file::plumeLog);
    fileArrayToFill.add (PLUME::file::scanLog);
    fileArrayToFill.add (PLUME::file::pluginList);
    
    #if JUCE_WINDOWS
    fileArrayToFill.add (File::getSpecialLocation (File::globalApplicationsDirectoryX86).getChildFile ("Enhancia\\Enhancia_Service_Reader\\Enhancia_Service_Reader_Log.txt"));
    #elif JUCE_MAC
    fileArrayToFill.add (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Caches/DaemonSeriaPort_MacOS/EnhanciaDaemonlog.txt"));
    fileArrayToFill.add (File::getSpecialLocation (File::userApplicationDataDirectory).getChildFile ("Caches/DaemonSeriaPort_MacOS/EnhanciaDaemonlog_previous.txt"));
    #endif
}
