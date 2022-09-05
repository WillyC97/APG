
#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioPlayer.h"

/// A slider that updates based on the current position of the audio
///
/// Can also be used to set the current audio position
class TransportSlider
    : public juce::Component
    , public juce::Timer
{
public:
    TransportSlider(AudioPlayer& _transportSource)
    : transportSource(_transportSource)
    {
        startTimerHz(30);
        
        slider.setColour(juce::Slider::backgroundColourId, juce::Colours::slategrey);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        SetSliderColours(false);
        trackTimeLabel.setJustificationType(juce::Justification::centred);
        timeRemainingLabel.setJustificationType(juce::Justification::centred);
        addChildComponent(slider);
        addChildComponent(trackTimeLabel);
        addChildComponent(timeRemainingLabel);
        addMouseListener(this, true);
    }
    
    ~TransportSlider() override = default;
    //==============================================================================
    
    void SetRange()
    {
        if(transportSource.GetLengthInSeconds() > 0.0)
        {
            slider.setVisible(true);
            trackTimeLabel.setVisible(true);
            timeRemainingLabel.setVisible(true);
            slider.setRange(0.0, transportSource.GetLengthInSeconds());
            trackTimeLabel.setText(SecondsToMinutes(transportSource.GetLengthInSeconds()), juce::NotificationType::dontSendNotification);
        }
    }
    
    void resized() override
    {
        auto totalBounds = getLocalBounds();
        auto labelSize = 50;
        auto timeRemainingBounds = totalBounds.removeFromLeft(labelSize);
        auto totalTimeBounds = totalBounds.removeFromRight(labelSize);
        
        timeRemainingLabel.setBounds(timeRemainingBounds);
        trackTimeLabel.setBounds(totalTimeBounds);
        slider.setBounds(totalBounds);
    }
            
private:
    //==============================================================================
    void mouseEnter(const juce::MouseEvent&) override
    {
        SetSliderColours(true);
    }
    void mouseExit(const juce::MouseEvent&) override
    {
        SetSliderColours(false);
    }
    
    void mouseUp(const juce::MouseEvent&) override
    {
        transportSource.start();
        isMouseDown = false;
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        mouseDrag(e);
        isMouseDown = true;
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        transportSource.stop();
        transportSource.SetTransportPosition(juce::jmax(0.0, XToTime((float)e.x)));
    }
    //==============================================================================
    void timerCallback() override
    {
        if(!isMouseDown)
            slider.setValue(transportSource.getTransportPosition());
        
        timeRemainingLabel.setText(SecondsToMinutes(transportSource.getTransportPosition()), juce::NotificationType::dontSendNotification);
    }
    //==============================================================================
    ///Converts the current slider position to a time in seconds relative to the track length
    double XToTime(const float x) const
    {
        return (x / slider.getWidth()) * transportSource.GetLengthInSeconds();
    }
    
    juce::String SecondsToMinutes(const double timeInSeconds)
    {
        int secs = int(timeInSeconds) % 60;
        int mins = int(timeInSeconds - secs) / 60;
        auto additionalZero = secs < 10 ? "0" : "";
        auto songLength = juce::String(mins) + ":" + additionalZero + juce::String(secs);
        return songLength;
    }
    
    void SetSliderColours(bool shouldPaintAsHighlighted)
    {
        auto trackColour = shouldPaintAsHighlighted ? juce::Colours::turquoise  : juce::Colours::whitesmoke;
        auto thumbColour = shouldPaintAsHighlighted ? juce::Colours::whitesmoke : juce::Colours::transparentBlack;
        slider.setColour(juce::Slider::trackColourId, trackColour);
        slider.setColour(juce::Slider::thumbColourId, thumbColour);
    }
    //==============================================================================
    
    AudioPlayer& transportSource;
    juce::Slider slider;
    bool isMouseDown = false;
    juce::Label trackTimeLabel;
    juce::Label timeRemainingLabel;
};
