#include "TagLibFileHandler.h"
#include "AudioMetadataManager.h"

#include <fileref.h>
#include <tag.h>
#include <tpropertymap.h>
#include <trueaudiofile.h>
#include <attachedpictureframe.h>

APG::TaggedFile TagLibFileHandler::GetAudioFileProperties(const juce::File& file)
{
    APG::TaggedFile fileToTag;
    
    auto filepath      = file.getFullPathName();
    fileToTag.filePath = filepath.toStdString();
    
    TagLib::FileRef f(filepath.toUTF8());

    if(!f.isNull() && f.tag())
    {
        TagLib::Tag *tag = f.tag();

        auto backupTrackName = file.getFileNameWithoutExtension().toStdString();
        fileToTag.title  = tag->title() .isEmpty() ? backupTrackName  : tag->title() .to8Bit();
        fileToTag.artist = tag->artist().isEmpty() ? "Unknown Artist" : tag->artist().to8Bit();
        fileToTag.album  = tag->album() .isEmpty() ? "Unknown Album"  : tag->album() .to8Bit();
        fileToTag.genre  = tag->genre() .isEmpty() ? "Unknown Genre"  : tag->genre() .to8Bit();
//        fileToTag.year   = tag->year()  == 0       ? "Unknown Year"   : tag->year()  .to8Bit();
    }
    
    if(!f.isNull() && f.audioProperties())
    {
        TagLib::AudioProperties *properties = f.audioProperties();

        fileToTag.bitrate    = properties->bitrate();
        fileToTag.sampleRate = properties->sampleRate();
    }
    
    return fileToTag;
}

juce::Image TagLibFileHandler::ExtractImageFromFile(const juce::File& file)
{
    std::unique_ptr<ID3v2TagContainer> tagContainer;
    tagContainer = AudioMetadataManager::CreateMetadataReader(file);
    
    if (tagContainer)
    {
        auto metadata = tagContainer->tag;
        unsigned int numFrames = metadata->frameList("APIC").size();
        
        for(unsigned int i = 0; i < numFrames; i++)
        {
            TagLib::ID3v2::AttachedPictureFrame* frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(metadata->frameList("APIC")[i]);
                
            if(frame->type() == TagLib::ID3v2::AttachedPictureFrame::FrontCover)
            {
                auto coverImage = juce::ImageCache::getFromMemory(frame->picture().data(), frame->size());
                return coverImage;
            }
        }
    }
    return juce::Image();
}


