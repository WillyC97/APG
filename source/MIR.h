#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "BpmDetector.h"
#include "ThreadWithNonModalAlertWindow.h"

class MIRProcessThread
    : public ThreadWithNonModalAlertWindow
{
public:
    MIRProcessThread(juce::XmlElement* trackListToUse, juce::Component* parentToUse)
        : ThreadWithNonModalAlertWindow(parentToUse)
        , trackList(trackListToUse)
    {
        analysisFinishedWindow = std::make_unique<NonModalAlertWindow>();
        parentToUse->addChildComponent(analysisFinishedWindow.get());
    }

    void run() override
    {
        int tracksToProcess = trackList->getNumChildElements();

        for (int i = 0; i < tracksToProcess; ++i)
        {
            
            if (auto* track = trackList->getChildElement(i))
            {
                auto bpm = AnalyseBpm(track->getStringAttribute("FileLocation").toStdString());
                track->setAttribute("BPM", juce::roundToInt(bpm));
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

    // This method gets called on the message thread once our thread has finished..
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
    BpmDetector bpmDetector;
    
    std::unique_ptr<NonModalAlertWindow> analysisFinishedWindow;
    
    float AnalyseBpm(const std::string& track)
    {
        return bpmDetector.CalculateBpm(track);
    }
};
