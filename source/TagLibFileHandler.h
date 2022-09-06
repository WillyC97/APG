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

/// Handles getting and setting audio file metadata
///
/// Audio properties are returned as a struct
/// @see APG::TaggedFile
class TagLibFileHandler
{
public:
    enum class Attribute
    {
        TITLE,
        ALBUM,
        ARTIST,
        GENRE,
        YEAR
    };
    
    /// Gets the available ID3v2 audioproperties from an audio file
    ///
    /// @param file - The audio file to extact the data
    static APG::TaggedFile GetAudioFileProperties(const juce::File& file);
    
    /// Gets the album artwowrk from a file
    ///
    /// If the album artwork is not avaiable, returns a default image
    static juce::Image ExtractImageFromFile(const juce::File& file);
    
    ///Sets ID3v2 Tags of a given audio file
    ///
    ///A given attribute is set using the attribute enum class
    ///@see Attribute
    static void SetTagAttribute(const juce::File& file, TagLibFileHandler::Attribute attribute, juce::String value);
};
