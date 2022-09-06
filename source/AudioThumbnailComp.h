#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioPlayer.h"

/// A component to draw the waveform of audio
///
/// Can also be used to adjust the transport position of the current playing audiofile
/// @param formatManager - The format manager to use
/// @param transportSource - The audio with which to follow
/// @param thumbCache - The AudioThumbnailCache to use
/// @param existingFile - The audio file to draw. Can also be set with SetFile()
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
    //juce::Component
    void paint(juce::Graphics& g) override;

    //juce::ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster*) override;
//==============================================================================
    //juce::Component
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
    
    /// Convert the current time of the track to an X position on the transport bar
    float TimeToX(const double time) const;
    /// Convert the current X position on the transport bar to a time value of a track
    double XToTime(const float x) const;

    bool CanMoveTransport() const noexcept;
    
    void UpdateCursorPosition();

    //juce::Timer
    void timerCallback() override;
};
