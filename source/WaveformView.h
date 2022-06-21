#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioPlayer.h"
#include "AudioThumbnailComp.h"

class WaveformView
    : public juce::Component
{
public:
    WaveformView( juce::AudioFormatManager& audioFormatManagerToUse
                , AudioPlayer& audioPlayerToUse);
    
    void paint (juce::Graphics& g) override;
    void resized() override;
    
    void SetFile(const juce::File& filePath);
    
private:
    juce::AudioThumbnailCache           thumbnailCache;
    std::unique_ptr<AudioThumbnailComp> thumbnailComp;
};

