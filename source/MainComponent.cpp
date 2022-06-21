#include "MainComponent.h"
#include "BinaryData.h"

//==============================================================================
MainComponent::MainComponent()
    : thumbnailCache(1)
    , audioPlayer(audioFormatManager)
    , playlistComponent(audioFormatManager)
    , state(TransportState::Stopped)
    , transportSlider(audioPlayer)
    , thumbnailComp(std::make_unique<AudioThumbnailComp>( audioFormatManager
                                                        , audioPlayer
                                                        , thumbnailCache))
{
    audioPlayer.AddListener(*this);
    playlistComponent.AddListener(*this);
    
    addAndMakeVisible(sidePanel);
    addAndMakeVisible(playlistComponent);
    addAndMakeVisible(thumbnailComp.get());
    
    addButton.setButtonText("Add");
    addButton.onClick=[this]()
    {
        auto files = sidePanel.getFiles();
        for (auto id : files)
            playlistComponent.insertTracks(id);
    };
    addAndMakeVisible(addButton);
    
    auto playImage         = juce::ImageCache::getFromMemory( BinaryData::play_png
                                                            , BinaryData::play_pngSize);
    auto pauseImage        = juce::ImageCache::getFromMemory( BinaryData::pause_png
                                                            , BinaryData::pause_pngSize);
    auto skipImageForward  = juce::ImageCache::getFromMemory( BinaryData::skip_forward_png
                                                            , BinaryData::skip_forward_pngSize);
    auto skipImageBackward = juce::ImageCache::getFromMemory( BinaryData::skip_backward_png
                                                            , BinaryData::skip_backward_pngSize);

    auto transparent = juce::Colours::transparentBlack;
    playButton.setImages(false, true, true, playImage, 0.9f, transparent, playImage, 0.5f, transparent, playImage, 1.0f, transparent);
    playButton.onClick=[=](){ audioPlayer.start(); };
    addAndMakeVisible(playButton);

    pauseButton.setImages(false, true, true, pauseImage, 0.9f, transparent, pauseImage, 0.5f, transparent, pauseImage, 1.0f, transparent);
    pauseButton.onClick=[=](){ audioPlayer.stop(); };
    addChildComponent(pauseButton);
    
    skipForwardButton.setImages(false, true, true, skipImageForward, 0.9f, transparent, skipImageForward, 0.5f, transparent, skipImageForward, 1.0f, transparent);
    skipForwardButton.onClick=[=](){ StreamFinished(); };
    addAndMakeVisible(skipForwardButton);
    
    skipBackwardButton.setImages(false, true, true, skipImageBackward, 0.9f, transparent, skipImageBackward, 0.5f, transparent, skipImageBackward, 1.0f, transparent);
    skipBackwardButton.onClick=[=](){ SkipBackward(); };
    addAndMakeVisible(skipBackwardButton);
    
    addAndMakeVisible(transportSlider);
    
    sidePanelButton.setButtonText("Browse Files");
    sidePanelButton.onClick=[this](){ sidePanel.showOrHide(!sidePanel.isPanelShowing()); };
    addAndMakeVisible(sidePanelButton);
    
    setSize (1000, 600);
    
    setAudioChannels(0, 2);
    
    startTimer(10);
}

MainComponent::~MainComponent()
{
    audioPlayer.RemoveListener(*this);
    shutdownAudio();
}

void MainComponent::timerCallback()
{
    if (audioPlayer.streamFinished.load())
        StreamFinished();
}
//==============================================================================
void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    audioPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    audioPlayer.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    audioPlayer.releaseResources();
}
//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
    
    auto totalBounds = getLocalBounds();
    auto panelWidth = totalBounds.proportionOfWidth(0.6);
    auto buttonBarBounds = totalBounds.removeFromBottom(90);
    totalBounds.removeFromRight(panelWidth);

    g.setColour (juce::Colour(0xFF212121));
    g.fillRect(buttonBarBounds);
}

void MainComponent::resized()
{
    const auto buttonSize = 50;
    const auto buttonGap  = 35;
    
    const auto transportMargin           = 13;
    const auto transportBarBoundsSize    = 42;
    const auto transportButtonBoundsSize = 35;

    
    auto totalBounds = getLocalBounds();
    auto panelWidth  = totalBounds.proportionOfWidth(0.6);
    
    auto transportBarBounds     = totalBounds.removeFromBottom(transportBarBoundsSize);
    auto transportButtonsBounds = totalBounds.removeFromBottom(transportButtonBoundsSize);
                                  totalBounds.removeFromBottom(transportMargin);
    auto playlistBounds         = totalBounds.removeFromRight(panelWidth);
    
    playButton.setBounds(transportButtonsBounds
                        .withRight(transportButtonsBounds.getWidth() /2 + buttonSize)
                        .withLeft(transportButtonsBounds.getWidth() / 2 - buttonSize));
    
    pauseButton.setBounds(transportButtonsBounds
                         .withRight(transportButtonsBounds.getWidth() /2 + buttonSize)
                         .withLeft(transportButtonsBounds.getWidth() / 2 - buttonSize));
    
    skipForwardButton.setBounds(transportButtonsBounds
                               .withRight(transportButtonsBounds.getWidth() /2 + buttonGap + buttonSize)
                               .withLeft(transportButtonsBounds.getWidth() / 2 + buttonGap));
    
    skipBackwardButton.setBounds(transportButtonsBounds
                                .withRight(transportButtonsBounds.getWidth() /2 - buttonGap)
                                .withLeft(transportButtonsBounds.getWidth() / 2 - buttonGap - buttonSize));
    
    addButton.setBounds(300, 350, 50, 50);
    sidePanelButton.setBounds(360, 350, 50, 50);
    playlistComponent.setBounds(playlistBounds);

//    thumbnailComp->setBounds(transportBarBounds);
    transportSlider.setBounds(transportBarBounds);
}

void MainComponent::StreamFinished()
{
    if (playlistComponent.tracks.size() > 1)
    {
        auto finalSongInPlaylistTrackNo = playlistComponent.getFinalSongInPlaylist()->trackNumber;
        auto lastSongPlayedTrackNo      = playlistComponent.GetLastTrackNoPlayed();
        if (finalSongInPlaylistTrackNo != lastSongPlayedTrackNo)
        {
            auto track = playlistComponent.tracks[unsigned(lastSongPlayedTrackNo)];
            LoadAndPlayTrack(*track);
        }
    }
}

void MainComponent::TransportStateChanged(const TransportState &newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
            case TransportState::Stopped:
                pauseButton.setVisible(false);
                playButton .setVisible(true);
                break;

            case TransportState::Starting:
                playButton .setVisible(false);
                audioPlayer.start();
                break;

            case TransportState::Playing:
                pauseButton.setVisible(true);
                playButton .setVisible(false);
                break;

            case TransportState::Stopping:
                audioPlayer.stop();
                break;
        }
    }
}

void MainComponent::PlayButtonClicked(const int &row)
{
    if (!playlistComponent.tracks.isEmpty())
    {
        auto selectedTrack = playlistComponent.tracks[unsigned(row)];
        LoadAndPlayTrack(*selectedTrack);
    }
}

void MainComponent::LoadAndPlayTrack(const PlaylistComponent::TrackInformation& track)
{
    auto filePath = track.songFileLocation;
    auto trackNo  = track.trackNumber;
    DBG(filePath.getFullPathName());
    
    audioPlayer.load(filePath);
    thumbnailComp->SetFile(filePath);
    transportSlider.SetRange();
    audioPlayer.start();
    
    playlistComponent.SetLastTrackNoPlayed(trackNo);
}

void MainComponent::SkipBackward()
{
    if(audioPlayer.getTransportPosition() > 3.0)
        audioPlayer.SetTransportPosition(0.0);
    else
    {
        if (!playlistComponent.tracks.isEmpty())
        {
            auto firstSongInPlaylistTrackNo = playlistComponent.GetFirstSongInPlaylist()->trackNumber;
            auto lastSongPlayedTrackNo      = playlistComponent.GetLastTrackNoPlayed();
            if (firstSongInPlaylistTrackNo != lastSongPlayedTrackNo)
                LoadAndPlayTrack(*playlistComponent.tracks[unsigned(lastSongPlayedTrackNo) - 2]);
        }
    }
}
