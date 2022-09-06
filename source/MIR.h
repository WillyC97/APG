#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "BpmDetector.h"
#include "KeyDetector.h"
#include "ThreadWithNonModalAlertWindow.h"

enum class AlgorithmType
{
    BPM,
    KEY
};

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
                    , juce::Component*  parentToUse
                    , AlgorithmType     algorithm
                    , bool              reanalyse = false);
    /// Runs the analysis on all tracks on a new thread
    ///
    /// Also updates the progress of the AlertWindow and status message
    void run() override;

    /// This method gets called on the message thread once our thread has finished..
    ///
    /// @param userPressedCancel - Bool to determine if the user cancelled the analysis
    void threadComplete (bool userPressedCancel) override;

private:
    void AnalyseBpm(juce::XmlElement* track);
    void AnalyseKey(juce::XmlElement* track);

    juce::XmlElement* trackList;
    bool shouldReanalyse;
    BpmDetector bpmDetector;
    KeyDetector keyDetector;

    std::function<void(juce::XmlElement*)> Analysis;

    std::unique_ptr<NonModalAlertWindow> analysisFinishedWindow;
};
