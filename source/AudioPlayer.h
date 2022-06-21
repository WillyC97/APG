#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <atomic>

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
    /** receives audio data in preparation of play */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    /** get the following audio buffer block */
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    /** allows the source to release anything it no longer needs after playback has stopped */
    void releaseResources() override;
    
    void AddListener   (Listener &l);
    void RemoveListener(Listener &l);
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    
    std::atomic<bool> streamFinished;
            
private:
    juce::AudioFormatManager&  formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    
    juce::ListenerList<Listener> listeners;
};

//=============================================================================
enum class TransportState
{
    Stopped,
    Starting,
    Playing,
    Stopping
};

//=============================================================================
class AudioPlayer::Listener
{
public:
    virtual ~Listener() = default;
        
    virtual void TransportStateChanged(const TransportState& state) = 0;
};
