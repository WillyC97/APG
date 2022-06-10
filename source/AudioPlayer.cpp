#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(juce::AudioFormatManager& _formatManager)
: formatManager(_formatManager)
{
}

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
    transportSource.stop();
    transportSource.setSource(nullptr);
    
    auto* reader = formatManager.createReaderFor(audioFile);
    if (reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
}

void AudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

/** get the following audio buffer block */
void AudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    transportSource.getNextAudioBlock(bufferToFill);
    if (transportSource.hasStreamFinished())
    {
        listeners.call([=](auto &l) { l.StreamFinished(); });
        transportSource.setPosition(0);
    }
        
}

/** allows the source to release anything it no longer needs after playback has stopped */
void AudioPlayer::releaseResources()
{
    transportSource.releaseResources();
}

void AudioPlayer::AddListener(Listener &l)
{
    listeners.add(&l);
}

void AudioPlayer::RemoveListener(Listener &l)
{
    listeners.remove(&l);
}
