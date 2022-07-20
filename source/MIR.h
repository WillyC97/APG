#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "BpmDetector.h"

class MIRProcessThread  : public juce::ThreadWithProgressWindow
{
public:
    MIRProcessThread(juce::XmlElement* trackListToUse)
        : juce::ThreadWithProgressWindow ("Analysing Track Features", true, true)
        , trackList(trackListToUse)
    {
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
            setProgress((i + 1) / (double) tracksToProcess);
            setStatusMessage(juce::String(tracksToProcess - (i+1)) + " tracks left to analyse...");
        }
    }

    // This method gets called on the message thread once our thread has finished..
    void threadComplete (bool userPressedCancel) override
    {
        const juce::String messageString(userPressedCancel ? "Analysis Cancelled" : "Analysis Complete");

        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
                                    .withIconType(juce::MessageBoxIconType::InfoIcon)
                                    .withTitle("Progress window")
                                    .withMessage(messageString)
                                    .withButton("OK"),
                                nullptr);
    }
    
private:
    juce::XmlElement* trackList;
    BpmDetector bpmDetector;
    
    float AnalyseBpm(const std::string& track)
    {
        return bpmDetector.CalculateBpm(track);
    }
};
