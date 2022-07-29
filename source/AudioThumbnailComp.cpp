#include "AudioThumbnailComp.h"

AudioThumbnailComp::AudioThumbnailComp( juce::AudioFormatManager& formatManager
                                      , AudioPlayer& _transportSource
                                      , juce::AudioThumbnailCache& thumbCache
                                      , const juce::File& existingFile)
: transportSource(_transportSource)
, thumbnail(512, formatManager, thumbCache)
, isFollowingTransport(false)
{
    thumbnail.addChangeListener(this);

    currentPositionMarker.setFill(juce::Colours::white.withAlpha(0.85f));
    addAndMakeVisible(currentPositionMarker);

    SetFile(existingFile);
}

AudioThumbnailComp::~AudioThumbnailComp()
{
    thumbnail.removeChangeListener(this);
}
//==============================================================================
void AudioThumbnailComp::SetFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        thumbnail.setSource(new juce::FileInputSource(file));
        thumbnailLength = thumbnail.getTotalLength();
        const juce::Range<double> newRange(0.0, thumbnail.getTotalLength());
        SetRange(newRange);

        startTimerHz(30);
    }
}

void AudioThumbnailComp::SetZoomFactor(double amount)
{
    if (thumbnailLength > 0)
    {
        const double newScale = juce::jmax(0.001, thumbnail.getTotalLength() * (1.0 - juce::jlimit(0.0, 0.99, amount)));
        const double timeAtCentre = XToTime(getWidth() / 2.0f);
        SetRange(juce::Range<double>(timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5));
    }
}

void AudioThumbnailComp::SetRange(juce::Range<double> newRange)
{
    visibleRange = newRange;
    UpdateCursorPosition();
    repaint();
}

void AudioThumbnailComp::SetFollowsTransport(bool shouldFollow)
{
    isFollowingTransport = shouldFollow;
}
//==============================================================================
void AudioThumbnailComp::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::slategrey);

    if (thumbnailLength > 0.0)
    {
        juce::Rectangle<int> thumbArea(getLocalBounds());
        thumbnail.drawChannel( g
                             , thumbArea.reduced (3)
                             , visibleRange.getStart()
                             , visibleRange.getEnd()
                             , 0
                             , 1.0f);
        
        int width = int(TimeToX(transportSource.getTransportPosition()));
        
        auto clippedWaveformColour = shouldPaintAsHighlighted ? juce::Colours::turquoise : juce::Colours::whitesmoke;
        g.setColour(clippedWaveformColour);
        g.reduceClipRegion(0, 0, width, getHeight());
        thumbnail.drawChannel( g
                             , thumbArea.reduced (3)
                             , visibleRange.getStart()
                             , visibleRange.getEnd()
                             , 0
                             , 1.0f);
    }
    else
    {
        g.setFont(14.0f);
        g.setColour(juce::Colours::whitesmoke);
        g.drawFittedText("(No audio file selected)", getLocalBounds(), juce::Justification::centred, 2);
    }
}
//==============================================================================
void AudioThumbnailComp::changeListenerCallback(ChangeBroadcaster*)
{
    // this method is called by the thumbnail when it has changed, so we should repaint it..
    repaint();
}
//==============================================================================
void AudioThumbnailComp::mouseDown(const juce::MouseEvent& e)
{
    mouseDrag(e);
}

void AudioThumbnailComp::mouseEnter(const juce::MouseEvent&)
{
    shouldPaintAsHighlighted = true;
}

void AudioThumbnailComp::mouseExit(const juce::MouseEvent&)
{
    shouldPaintAsHighlighted = false;
}

void AudioThumbnailComp::mouseDrag(const juce::MouseEvent& e)
{
    if (CanMoveTransport())
    {
        transportSource.stop();
        transportSource.SetTransportPosition(juce::jmax(0.0, XToTime((float)e.x)));
    }
}

void AudioThumbnailComp::mouseUp(const juce::MouseEvent&)
{
    transportSource.start();
}

void AudioThumbnailComp::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    if (thumbnail.getTotalLength() > 0.0)
    {
        if (CanMoveTransport())
            transportSource.SetTransportPosition(transportSource.getTransportPosition() + 4 * (XToTime(wheel.deltaY)));
        repaint();
    }
}
//==============================================================================
float AudioThumbnailComp::TimeToX(const double time) const
{
    return getWidth() * (float)((time - visibleRange.getStart()) / (visibleRange.getLength()));
}

double AudioThumbnailComp::XToTime(const float x) const
{
    return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
}

bool AudioThumbnailComp::CanMoveTransport() const noexcept
{
    return !(isFollowingTransport && transportSource.IsTransportPlaying());
}

void AudioThumbnailComp::timerCallback()
{
    if (CanMoveTransport())
        UpdateCursorPosition();
    else
        SetRange(visibleRange.movedToStartAt(transportSource.getTransportPosition() - (visibleRange.getLength() / 2.0)));
}

void AudioThumbnailComp::UpdateCursorPosition()
{
    auto markerSize = shouldPaintAsHighlighted ? 3.f : 1.5f;
    currentPositionMarker.setRectangle(juce::Rectangle<float>( TimeToX(transportSource.getTransportPosition()) - 0.75f
                                                             , 0
                                                             , markerSize
                                                             , (float(getHeight()))));
    currentPositionMarker.setCornerSize(juce::Point<float>(2, 2));
    repaint();
}
