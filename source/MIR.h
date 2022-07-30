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

class MIRProcessThread
    : public ThreadWithNonModalAlertWindow
{
public:
    MIRProcessThread( juce::XmlElement* trackListToUse
                    , juce::Component*  parentToUse
                    , AlgorithmType     algorithm
                    , bool              reanalyse = false);

    void run() override;

    // This method gets called on the message thread once our thread has finished..
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
