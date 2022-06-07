#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(juce::AudioFormatManager& _formatManager)
: formatManager(_formatManager)
{}

void AudioPlayer::start()
{
    transportSource.start();
}

void AudioPlayer::stop()
{
    transportSource.stop();
}

void AudioPlayer::load(const juce::File& audioFile)
{
    
}

void AudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

/** get the following audio buffer block */
void AudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

/** allows the source to release anything it no longer needs after playback has stopped */
void AudioPlayer::releaseResources()
{
    transportSource.releaseResources();
}

