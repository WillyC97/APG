#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(juce::AudioFormatManager& _formatManager)
: streamFinished(false)
, formatManager(_formatManager)

{
    transportSource.addChangeListener(this);
}

AudioPlayer::~AudioPlayer()
{
    transportSource.setSource(nullptr);
}
//==============================================================================
void AudioPlayer::start()
{
    transportSource.start();
}

void AudioPlayer::stop()
{
    transportSource.stop();
}

bool AudioPlayer::IsTransportPlaying()
{
    return transportSource.isPlaying();
}

void AudioPlayer::SetTransportPosition(const double newPos)
{
    transportSource.setPosition(newPos);
}

const double AudioPlayer::getTransportPosition()
{
    return transportSource.getCurrentPosition();
}

const double AudioPlayer::GetLengthInSeconds()
{
    return transportSource.getLengthInSeconds();
}

void AudioPlayer::load(const juce::File& audioFile)
{
    transportSource.stop();
    
    auto* reader = formatManager.createReaderFor(audioFile);
    if (reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
}

//==============================================================================
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
    
    streamFinished = transportSource.hasStreamFinished();
}

/** allows the source to release anything it no longer needs after playback has stopped */
void AudioPlayer::releaseResources()
{
    transportSource.releaseResources();
}
//==============================================================================

void AudioPlayer::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            listeners.call([=](auto &l) { l.TransportStateChanged(TransportState::Playing); });
        else
            listeners.call([=](auto &l) { l.TransportStateChanged(TransportState::Stopped); });
    }
}
//==============================================================================

void AudioPlayer::AddListener(Listener &l)
{
    listeners.add(&l);
}

void AudioPlayer::RemoveListener(Listener &l)
{
    listeners.remove(&l);
}
//==============================================================================
