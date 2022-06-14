#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class AudioPlayer
    : public juce::AudioSource
    , public juce::ChangeListener
{
public:
    AudioPlayer(juce::AudioFormatManager& _formatManager);
    ~AudioPlayer() override = default;
    
    class Listener;
    
    void start();
    void stop();
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
            
private:
    juce::AudioFormatManager&  formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    
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
    
    virtual void StreamFinished() = 0;
    
    virtual void TransportStateChanged(const TransportState& state) = 0;
};
