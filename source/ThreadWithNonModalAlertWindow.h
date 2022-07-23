#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "NonModalAlertWindow.h"

class ThreadWithNonModalAlertWindow
    : public juce::Thread
    , private juce::Timer
    , private juce::Button::Listener
{
public:
    ThreadWithNonModalAlertWindow(juce::Component* parent);
    ~ThreadWithNonModalAlertWindow();
    
    void SetStatusMessage(const juce::String& newStatusMessage);
    void SetProgress(const double newProgress);
    void LaunchThread (int priority = 5);
    
    void runThread();
    
    virtual void threadComplete (bool userPressedCancel);
    
private:
    void timerCallback() override;
    void buttonClicked(juce::Button* button) override;
    
    double progress;
    juce::String message;
    juce::CriticalSection messageLock;
    std::unique_ptr<NonModalAlertWindowWithProgressBar> alertWindow;
};
