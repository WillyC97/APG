#include "ThreadWithNonModalAlertWindow.h"

ThreadWithNonModalAlertWindow::ThreadWithNonModalAlertWindow(juce::Component* parent)
: Thread ("ThreadWithProgressWindow"),
  progress (0.0)
{
    alertWindow = std::make_unique<NonModalAlertWindowWithProgressBar>("Analysing Tracks", progress);
    parent->addChildComponent(alertWindow.get());
    alertWindow->AddButtonListener(this);
}
//------------------------------------------------------------------------------
ThreadWithNonModalAlertWindow::~ThreadWithNonModalAlertWindow()
{
    stopTimer();
    stopThread(1000);
}
//------------------------------------------------------------------------------

void ThreadWithNonModalAlertWindow::LaunchThread(int priority)
{
    startThread(priority);
    startTimer(100);
    
    {
        const juce::ScopedLock sl(messageLock);
        alertWindow->SetMessageText(message);
    }
    
    alertWindow->setVisible(true);
    alertWindow->resized();
}
//------------------------------------------------------------------------------

void ThreadWithNonModalAlertWindow::SetStatusMessage(const juce::String& newStatusMessage)
{
    const juce::ScopedLock sl(messageLock);
    message = newStatusMessage;
}
//------------------------------------------------------------------------------

void ThreadWithNonModalAlertWindow::SetProgress(const double newProgress)
{
    progress = newProgress;
}
//------------------------------------------------------------------------------

void ThreadWithNonModalAlertWindow::buttonClicked(juce::Button* button)
{
    if (button->getName() == "Cancel")
    {
        threadComplete(true);
        alertWindow->setVisible(false);
    }
}
//------------------------------------------------------------------------------

void ThreadWithNonModalAlertWindow::timerCallback()
{
    bool threadStillRunning = isThreadRunning();

    if (! (threadStillRunning && alertWindow->isVisible()))
    {
        stopTimer();
        stopThread (10000);
        alertWindow->setVisible (false);

        threadComplete(isThreadRunning());
        return; // (this may be deleted now)
    }
    
    const juce::ScopedLock sl (messageLock);
    alertWindow->SetMessageText(message);
}
//------------------------------------------------------------------------------
void ThreadWithNonModalAlertWindow::threadComplete(bool) {}
//------------------------------------------------------------------------------

void ThreadWithNonModalAlertWindow::runThread()
{
    LaunchThread();
    
    while(isTimerRunning())
        juce::MessageManager::getInstance()->runDispatchLoopUntil (5);
}
