#pragma once

#include <memory>

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include "aifffile.h"
#include "fileref.h"
#include "flacfile.h"
#include "modfile.h"
#include "mpegfile.h"
#include "trueaudiofile.h"
#include "wavfile.h"
#include "wavpackfile.h"

#include <attachedpictureframe.h>

namespace APG 
{
  std::unique_ptr<TagLib::File> CreateTagFile(const juce::File &file) 
  {
    const auto fileAsUTF8 = file.getFullPathName().toRawUTF8();
    
    if (file.getFileExtension().compareIgnoreCase(".mp3") == 0)
    {
      return std::make_unique<TagLib::MPEG::File>(fileAsUTF8);
    }

    if(file.getFileExtension().compareIgnoreCase(".wav") == 0)
    {
      return std::make_unique<TagLib::RIFF::WAV::File>(fileAsUTF8);
    }

    if(file.getFileExtension().compareIgnoreCase(".aiff") == 0 ||
            file.getFileExtension().compareIgnoreCase(".aif")  == 0 ||
            file.getFileExtension().compareIgnoreCase(".aifc") == 0)
    {
      return std::make_unique<TagLib::RIFF::AIFF::File>(fileAsUTF8);
    }

    if(file.getFileExtension().compareIgnoreCase(".flac") == 0)
    {
      return std::make_unique<TagLib::FLAC::File>(fileAsUTF8);
    }

    if(file.getFileExtension().compareIgnoreCase(".tta") == 0)
    {
      return std::make_unique<TagLib::TrueAudio::File>(fileAsUTF8);
    }
        
    return nullptr;
  }

  TagLib::ID3v2::Tag* GetID3v2TagFromTagFile(TagLib::File &file)
  {
    if (auto f = dynamic_cast<TagLib::MPEG::File*>(&file))
    {
      return f->ID3v2Tag();
    }

    if (auto f = dynamic_cast<TagLib::RIFF::WAV::File*>(&file))
    {
      return f->ID3v2Tag();
    }

    if (auto f = dynamic_cast<TagLib::RIFF::AIFF::File*>(&file))
    {
      return f->tag();
    }

    if (auto f = dynamic_cast<TagLib::FLAC::File*>(&file))
    {
      return f->ID3v2Tag();
    }

    if (auto f = dynamic_cast<TagLib::TrueAudio::File*>(&file))
    {
      return f->ID3v2Tag();
    }

    return nullptr;
  }

  TagLib::ID3v2::AttachedPictureFrame* GetAttachedPictureFromFromTagFile(TagLib::File                              &file, 
                                                                         TagLib::ID3v2::AttachedPictureFrame::Type  type)
  {
      const auto id3v2Tag = GetID3v2TagFromTagFile(file);
      const unsigned int numPictureFrames = id3v2Tag->frameList("APIC").size();
      
      for (unsigned int i = 0; i < numPictureFrames; i++)
      {
          auto* apFrame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(id3v2Tag->frameList("APIC")[i]);

          if (apFrame) 
          {
            if (apFrame->type() == type)
              return apFrame;
          }
      }

      return nullptr;
  }

  juce::Image FrontCoverImageFromFromTagFile(TagLib::File &file)
  {
    auto *apFrame = GetAttachedPictureFromFromTagFile(file, TagLib::ID3v2::AttachedPictureFrame::FrontCover);

    if (apFrame) 
      return juce::ImageCache::getFromMemory(apFrame->picture().data(), static_cast<int>(apFrame->size()));
    else
      return {};
  }
}
