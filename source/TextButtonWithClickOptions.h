#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

/// A TextButton that can perform different functions onlong press and shift click
///
/// @see onSingleClick, onShiftClick, onLongPress
class TextButtonWithClickOptions
: public juce::TextButton
, private juce::Timer
{
public:
    /** You can assign a lambda to these callback objects to have it called when the button is clicked. */
    std::function<void()> onSingleClick;
    std::function<void()> onShiftClick;
    std::function<void()> onLongPress;
    
private:
    void mouseUp(const juce::MouseEvent& e) override
    {
        stopTimer();
        if(hasBeenLongPressed)
        {
            hasBeenLongPressed = false;
            return;
        }

        if(e.mods.isShiftDown())
        {
            if (onShiftClick)
                onShiftClick();
        }
        else
        {
           if (onSingleClick)
               onSingleClick();
        }
    }
    
    void mouseDown(const juce::MouseEvent&) override
    {
        startTimerHz(60);
        lastButtonDownTime = juce::Time::getCurrentTime();
    }
    
    void timerCallback() override
    {
        auto buttonDownTime = juce::Time::getCurrentTime() - lastButtonDownTime;
        if(buttonDownTime.inMilliseconds() > 1200)
        {
            stopTimer();
            if(onLongPress)
            {
                hasBeenLongPressed = true;
                onLongPress();
            }
        }
    }
    
    juce::Time lastButtonDownTime;
    bool hasBeenLongPressed = false;
};
