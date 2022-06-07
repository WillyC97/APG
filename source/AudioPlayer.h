#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

class AudioPlayer : public juce::AudioSource
{
public:
    AudioPlayer(juce::AudioFormatManager& _formatManager);
    
    void start();
    void stop();
    void load(const juce::File& audioFile);
    
    //juce::AudioSource
    /** receives audio data in preparation of play */
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;

    /** get the following audio buffer block */
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    /** allows the source to release anything it no longer needs after playback has stopped */
    void releaseResources() override;
        
private:
    juce::AudioFormatManager&  formatManager;
    juce::AudioTransportSource transportSource;
};
