#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "NonModalAlertWindow.h"

/// Launches a new thread for processing
///
/// Also manages a NonModalAlertWindowWithProgressBar
/// @see NonModalAlertWindowWithProgressBar
/// For useage:
/// @see MIR
class ThreadWithNonModalAlertWindow
    : public juce::Thread
    , private juce::Timer
    , private juce::Button::Listener
{
public:
    ThreadWithNonModalAlertWindow(juce::Component* parent);
    ~ThreadWithNonModalAlertWindow() override;
    
    ///Used to set the status message shown in the alertwindow
    void SetStatusMessage(const juce::String& newStatusMessage);
    ///Updates the current progress shown on the progress bar
    void SetProgress(const double newProgress);
    void LaunchThread (int priority = 5);
    
    void runThread();
    
    /// This method is called (on the message thread) when the operation has finished.
    virtual void threadComplete (bool userPressedCancel);
    
private:
    void timerCallback() override;
    void buttonClicked(juce::Button* button) override;
    
    double progress;
    juce::String message;
    juce::CriticalSection messageLock;
    std::unique_ptr<NonModalAlertWindowWithProgressBar> alertWindow;
};
