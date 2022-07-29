#include "AudioMetadataManager.h"

std::unique_ptr<ID3v2TagContainer> AudioMetadataManager::CreateMetadataReader(const juce::File& file)
{
    const auto fileAsUTF8 = file.getFullPathName().toRawUTF8();
    
    if(file.getFileExtension().compareIgnoreCase(".mp3") == 0)
    {
        std::unique_ptr<TagLib::MPEG::File> mpegFile = std::make_unique<TagLib::MPEG::File>(fileAsUTF8);
        
        if(mpegFile->hasID3v2Tag())
            return std::make_unique<ID3v2TagContainer>(std::move(mpegFile), mpegFile->ID3v2Tag());
    }
    else if(file.getFileExtension().compareIgnoreCase(".wav") == 0)
    {
        std::unique_ptr<TagLib::RIFF::WAV::File> wavFile = std::make_unique<TagLib::RIFF::WAV::File>(fileAsUTF8);
        
        if(wavFile->hasID3v2Tag())
            return std::make_unique<ID3v2TagContainer>(std::move(wavFile), wavFile->ID3v2Tag());
    }
    else if(file.getFileExtension().compareIgnoreCase(".aiff") == 0 ||
            file.getFileExtension().compareIgnoreCase(".aif")  == 0 ||
            file.getFileExtension().compareIgnoreCase(".aifc") == 0)
    {
        std::unique_ptr<TagLib::RIFF::AIFF::File> aiffFile = std::make_unique<TagLib::RIFF::AIFF::File>(fileAsUTF8);

        if(aiffFile->hasID3v2Tag())
            return std::make_unique<ID3v2TagContainer>(std::move(aiffFile), aiffFile->tag());
    }
    else if(file.getFileExtension().compareIgnoreCase(".flac") == 0)
    {
        std::unique_ptr<TagLib::FLAC::File> flacFile = std::make_unique<TagLib::FLAC::File>(fileAsUTF8);

        if(flacFile->hasID3v2Tag())
            return std::make_unique<ID3v2TagContainer>(std::move(flacFile), flacFile->ID3v2Tag());
    }
    else if(file.getFileExtension().compareIgnoreCase(".tta") == 0)
    {
        std::unique_ptr<TagLib::TrueAudio::File> ttaFile = std::make_unique<TagLib::TrueAudio::File>(fileAsUTF8);

        if(ttaFile->hasID3v2Tag())
            return std::make_unique<ID3v2TagContainer>(std::move(ttaFile), ttaFile->ID3v2Tag());
    }
        
    return nullptr;
}
