#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "AlbumArtworkDisplayer.h"
#include "AudioPlayer.h"
#include "AudioThumbnailComp.h"
#include "PlaylistCreationComponent.h"
#include "PlaylistSettingsComponent.h"
#include "PlaylistTrackManagerComponent.h"
#include "TransportSlider.h"
#include "WaveformView.h"

//==============================================================================
/// The MainComponent Class
///
/// Holds all the other components that make up the application.
/// Also handles the play functionality.
class MainComponent
    : public juce::AudioAppComponent
    , public juce::Timer
    , public juce::ChangeListener
    , public juce::KeyListener
    , private AudioPlayer::Listener
    , private PlaylistTrackManagerComponent::Listener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;
    
    //==============================================================================
    //juce::AudioAppComponent
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    //==============================================================================
    //juce::Component
    void paint (juce::Graphics&) override;
    void resized() override;
    
    //juce::Timer
    void timerCallback() override;
    //juce::KeyListener
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
    
private:
    //==============================================================================
    juce::Array<juce::File> getFiles();
    
    void TransportStateChanged(const TransportState& state) override;
    void StreamFinished();
    
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    void PlayButtonClicked(const int& row) override;
    
    void SkipBackward();
    void LoadAndPlayTrack(const juce::XmlElement* track);
    
    AudioPlayer                   audioPlayer;
    std::unique_ptr<WaveformView> waveFormView;
    juce::AudioFormatManager      audioFormatManager;
    juce::AudioTransportSource    transportSource;

    PlaylistCreationComponent     playlistCreationComponent;
    PlaylistTrackManagerComponent trackManager;
    juce::File                    lastTrackPlayedDir;
                
    juce::ImageButton             playButton;
    juce::ImageButton             pauseButton;
    juce::ImageButton             skipForwardButton;
    juce::ImageButton             skipBackwardButton;
    juce::ImageButton             waveformViewButton;
    juce::TextButton              playlistSettingsButton;
    
    TransportSlider               transportSlider;
        
    TransportState state;
    std::unique_ptr<PlaylistSettingsComponent> playlistSettingsComponent;
    juce::SidePanel                            playlistSettingsSidePanel;
    std::unique_ptr<AlbumArtworkDisplayer>     artworkDisplayer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
