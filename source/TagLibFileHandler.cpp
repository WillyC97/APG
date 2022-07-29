#include "TagLibFileHandler.h"

#include "AudioMetadataUtils.h"
#include "BinaryData.h"

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

juce::Image TagLibFileHandler::ExtractFrontCoverImageFromFile(const juce::File& file)
{
    auto missingArtworkImage = juce::ImageCache::getFromMemory( BinaryData::missing_artwork_png
                                                              , BinaryData::missing_artwork_pngSize);
    
    std::unique_ptr<TagLib::File> tagFile = APG::CreateTagFile(file);

    juce::Image frontCover;

    if (tagFile) 
    {
      frontCover = APG::FrontCoverImageFromFromTagFile(*tagFile);
    }

    return frontCover.isNull() ? missingArtworkImage : frontCover;
}


