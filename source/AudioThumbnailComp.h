#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioPlayer.h"

class AudioThumbnailComp
    : public  juce::Component
    , public  juce::ChangeListener
    , public  juce::ChangeBroadcaster
    , private juce::Timer
{
public:
    AudioThumbnailComp( juce::AudioFormatManager&  formatManager
                      , AudioPlayer&               transportSource
                      , juce::AudioThumbnailCache& thumbCache
                      , const juce::File& existingFile = juce::File());

    ~AudioThumbnailComp() override;
//==============================================================================
    void SetFile(const juce::File& file);

    juce::File GetLastDroppedFile() const noexcept;

    void SetZoomFactor(double amount);

    void SetRange(juce::Range<double> newRange);

    void SetFollowsTransport(bool shouldFollow);

//==============================================================================
    void paint(juce::Graphics& g) override;

    void changeListenerCallback(juce::ChangeBroadcaster*) override;
//==============================================================================
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent&) override;
    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel) override;
    void mouseEnter(const juce::MouseEvent&) override;
    void mouseExit(const juce::MouseEvent&) override;
//==============================================================================

private:
    AudioPlayer&            transportSource;

    juce::AudioThumbnail    thumbnail;
    juce::Range<double>     visibleRange;
    
    juce::DrawableRectangle currentPositionMarker;
    
    bool isFollowingTransport;
    bool shouldPaintAsHighlighted = false;

    float TimeToX(const double time) const;
    double XToTime(const float x) const;

    bool CanMoveTransport() const noexcept;
    
    void UpdateCursorPosition();

    void timerCallback() override;
};
