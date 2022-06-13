#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
    : audioPlayer(audioFormatManager)
    , playlistComponent(audioFormatManager)
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
    
    playButton.setButtonText("Play");
    playButton.onClick=[this](){
        audioPlayer.start();
    };
    addAndMakeVisible(playButton);

    loadButton.setButtonText("Load");
    loadButton.onClick=[=]()
    {
        auto selectedTrack = PlaylistComponent::tracks[PlaylistComponent::selectedRowNo];
        LoadAndPlayTrack(selectedTrack);
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
}

void MainComponent::resized()
{
    auto totalBounds = getLocalBounds();
    auto panelWidth = totalBounds.proportionOfWidth(0.6);
    auto playlistBounds = totalBounds.removeFromRight(panelWidth);
    addButton.setBounds(300, 350, 50, 50);
    sidePanelButton.setBounds(360, 350, 50, 50);
    playlistComponent.setBounds(playlistBounds);
    playButton.setBounds(250, 350, 50, 50);
    loadButton.setBounds(200, 350, 50, 50);
}

void MainComponent::StreamFinished()
{
    DBG("Stream finished");
    auto finalSongInPlaylistTrackNo = playlistComponent.getFinalSongInPlaylist().trackNumber;
    auto lastSongPlayedTrackNo      = playlistComponent.GetLastTrackNoPlayed();
    if (finalSongInPlaylistTrackNo != lastSongPlayedTrackNo)
        LoadAndPlayTrack(PlaylistComponent::tracks[unsigned(lastSongPlayedTrackNo)]);
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
