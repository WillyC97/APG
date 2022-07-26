#include "TrackInfoComponent.h"

#include "TagLibFileHandler.h"

TrackInfoComponent::TrackInfoComponent()
{
    setAlwaysOnTop(true);
    backButton.setButtonText("Back");
    backButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
    
    addAndMakeVisible(artistNameLabel);
    addAndMakeVisible(trackNameLabel);
    addAndMakeVisible(albumNameLabel);
    addAndMakeVisible(genreLabel);
    addAndMakeVisible(bitrateLabel);
    addAndMakeVisible(samplerateLabel);
    addAndMakeVisible(formatLabel);
    addAndMakeVisible(albumArtwork);
    
    addAndMakeVisible(staticArtistNameLabel);
    addAndMakeVisible(staticTrackNameLabel);
    addAndMakeVisible(staticAlbumNameLabel);
    addAndMakeVisible(staticGenreLabel);
    addAndMakeVisible(staticBitrateLabel);
    addAndMakeVisible(staticSamplerateLabel);
    addAndMakeVisible(staticFormatLabel);
    addAndMakeVisible(albumArtwork);
    addAndMakeVisible(backButton);
    
    staticArtistNameLabel.attachToComponent(&artistNameLabel, true);
    staticTrackNameLabel .attachToComponent(&trackNameLabel, true);
    staticAlbumNameLabel .attachToComponent(&albumNameLabel, true);
    staticGenreLabel     .attachToComponent(&genreLabel, true);
    staticBitrateLabel   .attachToComponent(&bitrateLabel, true);
    staticSamplerateLabel.attachToComponent(&samplerateLabel, true);
    staticFormatLabel    .attachToComponent(&formatLabel, true);
}

void TrackInfoComponent::SetTrackToLoad(const juce::File &file)
{
    auto taggedFile = TagLibFileHandler::GetAudioFileProperties(file);
    
    artistNameLabel.setText(taggedFile.artist, juce::dontSendNotification);
    trackNameLabel .setText(taggedFile.title, juce::dontSendNotification);
    albumNameLabel .setText(taggedFile.album, juce::dontSendNotification);
    genreLabel     .setText(taggedFile.genre, juce::dontSendNotification);
    bitrateLabel   .setText(juce::String(taggedFile.bitrate), juce::dontSendNotification);
    samplerateLabel.setText(juce::String(taggedFile.sampleRate), juce::dontSendNotification);
    formatLabel    .setText(file.getFileExtension(), juce::dontSendNotification);

    staticArtistNameLabel.setText("Artist: ",     juce::dontSendNotification);
    staticTrackNameLabel .setText("Title: ",      juce::dontSendNotification);
    staticAlbumNameLabel .setText("Album: ",      juce::dontSendNotification);
    staticGenreLabel     .setText("Genre: ",      juce::dontSendNotification);
    staticBitrateLabel   .setText("Bitrate: ",    juce::dontSendNotification);
    staticSamplerateLabel.setText("Samplerate: ", juce::dontSendNotification);
    staticFormatLabel    .setText("Format: ",     juce::dontSendNotification);
}

void TrackInfoComponent::SetImageToShow(juce::Image &image)
{
    albumArtwork.setImage(image);
}

void TrackInfoComponent::resized()
{
    auto totalBounds = getLocalBounds();
         totalBounds = totalBounds.reduced(4);
    
    auto buttonBounds  = totalBounds.removeFromTop(50).reduced(6);
    auto artworkBounds = totalBounds.removeFromLeft(proportionOfWidth(0.5));
    auto labelSize     = totalBounds.proportionOfHeight(0.125);
           totalBounds = totalBounds.removeFromRight(totalBounds
                                                    .proportionOfWidth(0.6))
                                                    .reduced(6);
    
    artistNameLabel.setBounds(totalBounds.removeFromTop(labelSize));
    trackNameLabel .setBounds(totalBounds.removeFromTop(labelSize));
    albumNameLabel .setBounds(totalBounds.removeFromTop(labelSize));
    genreLabel     .setBounds(totalBounds.removeFromTop(labelSize));
    bitrateLabel   .setBounds(totalBounds.removeFromTop(labelSize));
    samplerateLabel.setBounds(totalBounds.removeFromTop(labelSize));
    formatLabel    .setBounds(totalBounds.removeFromTop(labelSize));
    albumArtwork   .setBounds(artworkBounds);
    backButton     .setBounds(buttonBounds);
}
