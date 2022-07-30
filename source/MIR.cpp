#include "MIR.h"

MIRProcessThread::MIRProcessThread( juce::XmlElement* trackListToUse
                                   , juce::Component* parentToUse
                                   , AlgorithmType    algorithm
                                   , bool             reanalyse )
: ThreadWithNonModalAlertWindow(parentToUse)
, trackList(trackListToUse)
, shouldReanalyse(reanalyse)
{
    analysisFinishedWindow = std::make_unique<NonModalAlertWindow>();
    parentToUse->addChildComponent(analysisFinishedWindow.get());
    
    switch (algorithm)
    {
        case AlgorithmType::BPM:
            Analysis = [=] (juce::XmlElement* el) { AnalyseBpm(el); };
            break;
        case AlgorithmType::KEY:
            Analysis = [=] (juce::XmlElement* el) { AnalyseKey(el); };
            break;
        default:
            Analysis = nullptr;
            break;
    }
}

void MIRProcessThread::run()
{
    int tracksToProcess = trackList->getNumChildElements();

    for (int i = 0; i < tracksToProcess; ++i)
    {
        if (auto* track = trackList->getChildElement(i))
        {
            if(Analysis)
                Analysis(track);
        }
        // must check this as often as possible, because this is
        // how we know if the user's pressed 'cancel'
        if (threadShouldExit())
            return;

        // this will update the progress bar on the dialog box
        SetProgress((i + 1) / (double) tracksToProcess);
        SetStatusMessage(juce::String(tracksToProcess - (i+1)) + " tracks left to analyse...");
    }
}

void MIRProcessThread::threadComplete (bool userPressedCancel)
{
    const juce::String messageString(userPressedCancel ? "Analysis Cancelled"
                                                       : "Analysis Complete");
    
    analysisFinishedWindow->WithButton();
    analysisFinishedWindow->SetWindowTitle(messageString);
    analysisFinishedWindow->setVisible(true);
    analysisFinishedWindow->resized();
}

void MIRProcessThread::AnalyseBpm(juce::XmlElement* track)
{
    if (track->getStringAttribute("BPM").isEmpty() || shouldReanalyse)
    {
        auto bpm = bpmDetector.CalculateBpm(track->getStringAttribute("FileLocation").toStdString());
        track->setAttribute("BPM", juce::roundToInt(bpm));
    }
}

void MIRProcessThread::AnalyseKey(juce::XmlElement* track)
{
    if (track->getStringAttribute("Key").isEmpty() || shouldReanalyse)
    {
        auto key = keyDetector.CalculateKey(track->getStringAttribute("FileLocation").toStdString());
        track->setAttribute("Key", key);
    }
}
