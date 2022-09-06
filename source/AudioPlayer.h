#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <atomic>

///Class for processing audio
class AudioPlayer
    : public juce::AudioSource
    , public juce::ChangeListener
{
public:
    AudioPlayer(juce::AudioFormatManager& _formatManager);
    ~AudioPlayer() override;
    
    class Listener;
    
    void start();
    void stop();
    bool IsTransportPlaying();
    const double GetLengthInSeconds();
    const double getTransportPosition();
    void SetTransportPosition(const double newPos);
    void load(const juce::File& audioFile);
    
    //juce::AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    
    void AddListener   (Listener &l);
    void RemoveListener(Listener &l);
    
    //juce::ChangeListener
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    
    std::atomic<bool> streamFinished;
            
private:
    juce::AudioFormatManager&  formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    
    juce::ListenerList<Listener> listeners;
};

//=============================================================================
/// Enum class for the various states of the audio
enum class TransportState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

//=============================================================================
/// Listener interface for the AudioPlayer class
///
/// @see AudioPlayer
class AudioPlayer::Listener
{
public:
    virtual ~Listener() = default;
    
    /// Called when the state of the audio changes
    virtual void TransportStateChanged(const TransportState& state) = 0;
};
