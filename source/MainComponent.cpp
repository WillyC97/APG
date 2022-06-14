#include "MainComponent.h"
#include "BinaryData.h"

//==============================================================================
MainComponent::MainComponent()
    : audioPlayer(audioFormatManager)
    , playlistComponent(audioFormatManager)
    , playPauseButton("PlayPause", juce::DrawableButton::ImageFitted)
    , state(TransportState::Stopped)
{
    audioPlayer.AddListener(*this);
    addAndMakeVisible(sidePanel);
    addAndMakeVisible(playlistComponent);
    
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
    
    loadButton.setButtonText("Load");
    loadButton.onClick=[=]()
    {
        auto tracks = PlaylistComponent::tracks;
        if (!tracks.empty())
        {
            auto selectedTrack = tracks[PlaylistComponent::selectedRowNo];
            LoadAndPlayTrack(selectedTrack);
        }
    };
    addAndMakeVisible(loadButton);
    
    sidePanelButton.setButtonText("Browse Files");
    sidePanelButton.onClick=[this](){ sidePanel.showOrHide(!sidePanel.isPanelShowing()); };
    addAndMakeVisible(sidePanelButton);
    
    setSize (1000, 600);
    
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    audioPlayer.RemoveListener(*this);
    shutdownAudio();
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
    totalBounds.removeFromRight(panelWidth);
    auto buttonBarBounds = totalBounds.removeFromBottom(90);
    g.setColour (juce::Colour(0xFF212121));
    g.fillRect(buttonBarBounds);
}

void MainComponent::resized()
{
    auto totalBounds = getLocalBounds();
    auto panelWidth = totalBounds.proportionOfWidth(0.6);
    auto playlistBounds = totalBounds.removeFromRight(panelWidth);
    auto buttonBarBounds = totalBounds.removeFromBottom(90);
    
    addButton.setBounds(300, 350, 50, 50);
    sidePanelButton.setBounds(360, 350, 50, 50);
    playlistComponent.setBounds(playlistBounds);
    playButton.setBounds(250, 350, 50, 50);
    pauseButton.setBounds(250, 350, 50, 50);
    loadButton.setBounds(200, 350, 50, 50);
    skipForwardButton.setBounds(100, 350, 40, 40);
    skipBackwardButton.setBounds(50, 350, 40, 40);
    playPauseButton.setBounds(100, 350, 50, 50);
}

void MainComponent::StreamFinished()
{
    if (!PlaylistComponent::tracks.empty())
    {
        auto finalSongInPlaylistTrackNo = playlistComponent.getFinalSongInPlaylist().trackNumber;
        auto lastSongPlayedTrackNo      = playlistComponent.GetLastTrackNoPlayed();
        if (finalSongInPlaylistTrackNo != lastSongPlayedTrackNo)
            LoadAndPlayTrack(PlaylistComponent::tracks[unsigned(lastSongPlayedTrackNo)]);
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

void MainComponent::LoadAndPlayTrack(PlaylistComponent::TrackInformation& track)
{
    auto filePath = track.songFileLocation;
    auto trackNo  = track.trackNumber;
    DBG(filePath.getFullPathName());
    
    audioPlayer.load(filePath);
    audioPlayer.start();
    
    playlistComponent.SetLastTrackNoPlayed(trackNo);
}

void MainComponent::SkipBackward()
{
    if(audioPlayer.getTransportPosition() > 3.0)
        audioPlayer.SetTransportPosition(0.0);
    else
    {
        if (!PlaylistComponent::tracks.empty())
        {
            auto firstSongInPlaylistTrackNo = playlistComponent.GetFirstSongInPlaylist().trackNumber;
            auto lastSongPlayedTrackNo      = playlistComponent.GetLastTrackNoPlayed();
            if (firstSongInPlaylistTrackNo != lastSongPlayedTrackNo)
                LoadAndPlayTrack(PlaylistComponent::tracks[unsigned(lastSongPlayedTrackNo) - 2]);
        }
    }
}
