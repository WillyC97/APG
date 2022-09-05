#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "BpmDetector.h"
#include "ThreadWithNonModalAlertWindow.h"

/// Runs the Essentia MIR algorithms
///
/// Makes use of the ThreadWithNonModalAlertWindow to run the algorithms on a new thread separate to that
/// of the audio thread and UI thread. Also handles showing and hiding of progress window to alert user of analysis progression
/// @param trackListToUse - An XmlElement of the current tracks in the playlist
/// @param parentToUse -  The parent component to show the AlertWindow ontop of
/// @param reanalyse - Whether or not to run the MIR algorithm again
class MIRProcessThread
    : public ThreadWithNonModalAlertWindow
{
public:
    MIRProcessThread( juce::XmlElement* trackListToUse
                    , juce::Component* parentToUse
                    , bool reanalyse = false)
    : ThreadWithNonModalAlertWindow(parentToUse)
    , trackList(trackListToUse)
    , shouldReanalyse(reanalyse)
    {
        analysisFinishedWindow = std::make_unique<NonModalAlertWindow>();
        parentToUse->addChildComponent(analysisFinishedWindow.get());
    }

    /// Runs the analysis on all tracks on a new thread
    ///
    /// Also updates the progress of the AlertWindow and status message
    void run() override
    {
        int tracksToProcess = trackList->getNumChildElements();

        for (int i = 0; i < tracksToProcess; ++i)
        {
            if (auto* track = trackList->getChildElement(i))
            {
                if (track->getStringAttribute("BPM").isEmpty() || shouldReanalyse)
                {
                    auto bpm = AnalyseBpm(track->getStringAttribute("FileLocation").toStdString());
                    track->setAttribute("BPM", juce::roundToInt(bpm));
                }
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

    /// This method gets called on the message thread once our thread has finished..
    ///
    /// @param userPressedCancel - Bool to determine if the user cancelled the analysis
    void threadComplete (bool userPressedCancel) override
    {
        const juce::String messageString(userPressedCancel ? "Analysis Cancelled"
                                                           : "Analysis Complete");
        
        analysisFinishedWindow->WithButton();
        analysisFinishedWindow->SetWindowTitle(messageString);
        analysisFinishedWindow->setVisible(true);
        analysisFinishedWindow->resized();
    }
    
private:
    juce::XmlElement* trackList;
    bool shouldReanalyse;
    BpmDetector bpmDetector;
    
    std::unique_ptr<NonModalAlertWindow> analysisFinishedWindow;
    
    float AnalyseBpm(const std::string& track)
    {
        return bpmDetector.CalculateBpm(track);
    }
};
