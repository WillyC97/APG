#include "MainComponent.h"
#include "BinaryData.h"

//==============================================================================
MainComponent::MainComponent()
    : audioPlayer(audioFormatManager)
    , trackManager(audioFormatManager)
    , state(TransportState::Stopped)
    , transportSlider(audioPlayer)
    , waveFormView(std::make_unique<WaveformView>( audioFormatManager
                                                 , audioPlayer))
    , playlistSettingsComponent(std::make_unique<PlaylistSettingsComponent>(trackManager))
    , playlistSettingsSidePanel("Playlist Settings", 400, true)
    , artworkDisplayer(std::make_unique<AlbumArtworkDisplayer>())
{
    addKeyListener(this);
    audioPlayer.AddListener(*this);
    trackManager.AddListener(*this);
    playlistCreationComponent.addChangeListener(this);
    
    playlistSettingsSidePanel.setContent(playlistSettingsComponent.release());
    
    addAndMakeVisible(trackManager);
    addAndMakeVisible(playlistCreationComponent);
    addAndMakeVisible(playlistSettingsButton);
    addChildComponent(waveFormView.get());
    addAndMakeVisible(playlistSettingsSidePanel);

    auto playImage         = juce::ImageCache::getFromMemory( BinaryData::play_png
                                                            , BinaryData::play_pngSize);
    auto pauseImage        = juce::ImageCache::getFromMemory( BinaryData::pause_png
                                                            , BinaryData::pause_pngSize);
    auto skipImageForward  = juce::ImageCache::getFromMemory( BinaryData::skip_forward_png
                                                            , BinaryData::skip_forward_pngSize);
    auto skipImageBackward = juce::ImageCache::getFromMemory( BinaryData::skip_backward_png
                                                            , BinaryData::skip_backward_pngSize);
    auto waveformImage     = juce::ImageCache::getFromMemory( BinaryData::waveform_png
                                                            , BinaryData::waveform_pngSize);

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
    
    waveformViewButton.setImages(false, true, true, waveformImage, 0.9f, transparent, waveformImage, 0.5f, transparent, waveformImage, 1.0f, transparent);
    waveformViewButton.setClickingTogglesState(true);
    waveformViewButton.onClick=[=](){ waveFormView->setVisible(!waveFormView->isShowing()); };
    addAndMakeVisible(waveformViewButton);
    
    playlistSettingsButton.setButtonText("Playlist Settings");
    playlistSettingsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
    playlistSettingsButton.onClick=[this]()
    {
        playlistSettingsSidePanel.showOrHide(!playlistSettingsSidePanel.isPanelShowing());
    };

    addAndMakeVisible(transportSlider);
    addAndMakeVisible(artworkDisplayer.get());
        
    setSize (1000, 600);
    
    setAudioChannels(0, 2);
    
    startTimerHz(60);
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
    getLookAndFeel().setColour(juce::SidePanel::backgroundColour, juce::Colour(0xFF1c1c1c));
    getLookAndFeel().setColour(juce::SidePanel::shadowBaseColour, juce::Colour(0xFF1c1c1c));
    getLookAndFeel().setColour(juce::DirectoryContentsDisplayComponent::highlightColourId, juce::Colour(0xFF229487));
    
    g.fillAll (juce::Colour(0xFF111212));
    
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
    
    const auto transportMargin              = 13;
    const auto transportBarBoundsSize       = 42;
    const auto transportButtonBoundsSize    = 35;
    const auto transportTotalBoundsSize     = transportMargin
                                            + transportBarBoundsSize
                                            + transportButtonBoundsSize;
    const auto availablePlaylistsBoundsSize = 240;
    
    auto       totalBounds = getLocalBounds();
    const auto split       = totalBounds.proportionOfWidth(0.25);
    
    auto transportTotalBounds   = totalBounds.removeFromBottom(transportTotalBoundsSize).reduced(8);
    auto transportTotalDirty    = transportTotalBounds;
    auto transportLeftThird     = transportTotalDirty.removeFromLeft(split);
    auto transportRightThird    = transportTotalDirty.removeFromRight(split);
    auto transportMiddle        = transportTotalDirty;
    auto transportButtonsBounds = transportMiddle.removeFromTop(transportButtonBoundsSize);
    
    auto waveformBounds         = totalBounds;
    auto playlistCreationBounds = totalBounds.removeFromLeft(availablePlaylistsBoundsSize);
    auto playlistSettingsBounds = playlistCreationBounds.removeFromTop(buttonSize);
    auto playlistBounds         = totalBounds;
    
    playButton.setBounds(transportButtonsBounds
                        .withRight(transportTotalBounds.getWidth() / 2 + buttonSize)
                        .withLeft (transportTotalBounds.getWidth() / 2 - buttonSize));

    pauseButton.setBounds(transportButtonsBounds
                         .withRight(transportTotalBounds.getWidth() / 2 + buttonSize)
                         .withLeft (transportTotalBounds.getWidth() / 2 - buttonSize));

    skipForwardButton.setBounds(transportButtonsBounds
                               .withRight(transportTotalBounds.getWidth() / 2 + buttonGap + buttonSize)
                               .withLeft (transportTotalBounds.getWidth() / 2 + buttonGap));

    skipBackwardButton.setBounds(transportButtonsBounds
                                .withRight(transportTotalBounds.getWidth() / 2 - buttonGap)
                                .withLeft (transportTotalBounds.getWidth() / 2 - buttonGap - buttonSize));
    
    waveformViewButton.setBounds(transportRightThird
                                .withLeft(transportTotalBounds.getWidth() - (2 * buttonSize)));
    
    artworkDisplayer->setBounds(transportButtonsBounds.withLeft(buttonSize).withRight(4*buttonSize));

    playlistSettingsButton   .setBounds(playlistSettingsBounds);
    trackManager             .setBounds(playlistBounds);
    playlistCreationComponent.setBounds(playlistCreationBounds);

    artworkDisplayer->setBounds(transportLeftThird);
    waveFormView    ->setBounds(waveformBounds);
    transportSlider  .setBounds(transportMiddle);
}

void MainComponent::StreamFinished()
{
    audioPlayer.SetTransportPosition(0.0);
    auto currentNumRows = trackManager.getNumRows();
    if (currentNumRows > 1)
    {
        auto lastSongPlayedTrackNo      = trackManager.GetLastTrackNoPlayed();
        if ((currentNumRows != lastSongPlayedTrackNo) &&
            (currentNumRows >= lastSongPlayedTrackNo))
        {
            auto track = trackManager.GetTrack(lastSongPlayedTrackNo);
            LoadAndPlayTrack(track);
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

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &playlistCreationComponent)
        trackManager.LoadPlaylist(playlistCreationComponent.GetPlaylist());
}

bool MainComponent::keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent)
{
    if(key.getKeyCode() == juce::KeyPress::spaceKey)
    {
        if (audioPlayer.IsTransportPlaying())
            audioPlayer.stop();
        else
            audioPlayer.start();
    }

    return true;
}

void MainComponent::PlayButtonClicked(const int &row)
{
        auto selectedTrack = trackManager.GetTrack(row);
        LoadAndPlayTrack(selectedTrack);
}

void MainComponent::LoadAndPlayTrack(const juce::XmlElement* track)
{
    juce::File filePath = track->getStringAttribute("FileLocation");
    auto trackNo  = track->getStringAttribute("No.").getIntValue();
    DBG(filePath.getFullPathName());
    
    artworkDisplayer->SetTrackToLoad(track);
    audioPlayer.load(filePath);
    waveFormView->SetFile(filePath);
    transportSlider.SetRange();
    audioPlayer.start();
    
    trackManager.SetLastTrackNoPlayed(trackNo);
}

void MainComponent::SkipBackward()
{
    if(audioPlayer.getTransportPosition() > 3.0)
        audioPlayer.SetTransportPosition(0.0);
    else
    {
        if (!(trackManager.getNumRows() == 0))
        {
            auto lastSongPlayedTrackNo = trackManager.GetLastTrackNoPlayed();
            if (lastSongPlayedTrackNo != 1)
                LoadAndPlayTrack(trackManager.GetTrack(lastSongPlayedTrackNo - 2));
        }
    }
}
