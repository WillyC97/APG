#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioPlayer.h"
#include "AudioThumbnailComp.h"
#include "PlaylistComponent.h"
#include "SidePanelWithBrowser.h"
#include "TransportSlider.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent
    : public juce::AudioAppComponent
    , public juce::Timer
    , private AudioPlayer::Listener
    , private PlaylistComponent::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;
    
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

    juce::AudioThumbnailCache thumbnailCache;
    
private:
    //==============================================================================
    // Your private member variables go here...
    juce::Array<juce::File> getFiles();
    
    void TransportStateChanged(const TransportState& state) override;
    void StreamFinished();
    
    void PlayButtonClicked(const int& row) override;
    
    void SkipBackward();
    void LoadAndPlayTrack(const PlaylistComponent::TrackInformation& fileToPlay);
    
    AudioPlayer                audioPlayer;
    std::unique_ptr<AudioThumbnailComp> thumbnailComp;
    juce::AudioFormatManager   audioFormatManager;
    juce::AudioTransportSource transportSource;

    PlaylistComponent          playlistComponent;
    juce::File                 lastTrackPlayedDir;
        
    SidePanelWithBrowser       sidePanel;
    
    juce::TextButton           addButton;
    juce::TextButton           sidePanelButton;
    
    juce::ImageButton          playButton;
    juce::ImageButton          pauseButton;
    juce::ImageButton          skipForwardButton;
    juce::ImageButton          skipBackwardButton;
    
    TransportSlider            transportSlider;
        
    TransportState state;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
