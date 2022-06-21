#include "WaveformView.h"

WaveformView::WaveformView( juce::AudioFormatManager& audioFormatManagerToUse
                          , AudioPlayer& audioPlayerToUse)
: thumbnailCache(1)
, thumbnailComp(std::make_unique<AudioThumbnailComp>( audioFormatManagerToUse
                                                    , audioPlayerToUse
                                                    , thumbnailCache))
{
    addAndMakeVisible(thumbnailComp.get());
}

void WaveformView::SetFile(const juce::File &filePath)
{
    thumbnailComp->SetFile(filePath);
}

void WaveformView::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}

void WaveformView::resized()
{
    thumbnailComp->setBounds(getLocalBounds().reduced(50));
}

