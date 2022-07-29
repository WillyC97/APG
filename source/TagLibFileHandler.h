#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <string>
#include <trueaudiofile.h>

namespace APG
{
    struct TaggedFile
    {
        std::string filePath;
        std::string title;
        std::string album;
        std::string artist;
        std::string genre;
        std::string year;
        int         bitrate;
        int         sampleRate;
    };
}

class TagLibFileHandler
{
public:
    static APG::TaggedFile GetAudioFileProperties(const juce::File& file);
    
    static juce::Image ExtractFrontCoverImageFromFile(const juce::File& file);
};
