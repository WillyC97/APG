#pragma once

// CMake builds don't use an AppConfig.h, so it's safe to include juce module headers
// directly. If you need to remain compatible with Projucer-generated builds, and
// have called `juce_generate_juce_header(<thisTarget>)` in your CMakeLists.txt,
// you could `#include <JuceHeader.h>` here instead, to make all your module headers visible.
#include <juce_gui_extra/juce_gui_extra.h>
#include "AudioPlayer.h"
#include "AudioThumbnailComp.h"
#include "PlaylistCreationComponent.h"
#include "PlaylistSettingsComponent.h"
#include "PlaylistTrackManagerComponent.h"
#include "TransportSlider.h"
#include "WaveformView.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
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
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
    bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
    
private:
    //==============================================================================
    // Your private member variables go here...
    juce::Array<juce::File> getFiles();
    
    void TransportStateChanged(const TransportState& state) override;
    void StreamFinished();
    
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    void PlayButtonClicked(const int& row) override;
    
    void SkipBackward();
    void LoadAndPlayTrack(const juce::XmlElement& track);
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
