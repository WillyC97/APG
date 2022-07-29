#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class TrackInfoComponent
    : public juce::Component
{
public:
    TrackInfoComponent(bool readOnly);
    
    void SetTrackToLoad(const juce::File& file);
    void SetImageToShow(juce::Image& image);
    void resized() override;
    void AddButtonListener   (juce::Button::Listener* l) { backButton.addListener(l); }
    void RemoveButtonListener(juce::Button::Listener* l) { backButton.removeListener(l); }
    
private:
    juce::File       trackFile;
    juce::Label      artistNameLabel;
    juce::Label      trackNameLabel;
    juce::Label      albumNameLabel;
    juce::Label      genreLabel;
    juce::Label      bitrateLabel;
    juce::Label      samplerateLabel;
    juce::Label      formatLabel;
    
    juce::Label      staticArtistNameLabel;
    juce::Label      staticTrackNameLabel;
    juce::Label      staticAlbumNameLabel;
    juce::Label      staticGenreLabel;
    juce::Label      staticBitrateLabel;
    juce::Label      staticSamplerateLabel;
    juce::Label      staticFormatLabel;
    juce::TextButton backButton;
    
    juce::ImageComponent albumArtwork;
};
