#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "aifffile.h"
#include "fileref.h"
#include "flacfile.h"
#include "modfile.h"
#include "mpegfile.h"
#include "trueaudiofile.h"
#include "wavfile.h"
#include "wavpackfile.h"

///Holds an ID3v2 Tag and file
///
///In order for the ID3v2 Tag to stay valid, the file from which the tag was created must be kept alive
///This is done by holding both in this container struct
struct ID3v2TagContainer
{
    ID3v2TagContainer(std::unique_ptr<TagLib::File> fileToKeep, TagLib::ID3v2::Tag* tagToUse)
    : file(std::move(fileToKeep))
    , tag(tagToUse)
    {}
    
    std::unique_ptr<TagLib::File> file;
    TagLib::ID3v2::Tag* tag;
};

///Creates an ID3v2 tag from a given audio file type
///
///If a file type does not have a tag or the file is not an audio file, it wll return nullptr
///@see ID3v2TagContainer
class AudioMetadataManager
{
public:
    static std::unique_ptr<ID3v2TagContainer> CreateMetadataReader(const juce::File& file);
};

