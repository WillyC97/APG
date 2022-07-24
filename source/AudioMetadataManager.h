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

struct ID3v2TagContainer
{
    ID3v2TagContainer(std::unique_ptr<TagLib::File> fileToKeep, TagLib::ID3v2::Tag* tagToUse)
    : file(std::move(fileToKeep))
    , tag(tagToUse)
    {}
    
    std::unique_ptr<TagLib::File> file;
    TagLib::ID3v2::Tag* tag;
};

class AudioMetadataManager
{
public:
    static std::unique_ptr<ID3v2TagContainer> CreateMetadataReader(const juce::File& file);
};

