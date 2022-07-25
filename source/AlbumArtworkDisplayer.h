#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class AlbumArtworkDisplayer
    : public juce::Component
{
public:
    AlbumArtworkDisplayer();

    void SetTrackToLoad(const juce::XmlElement* track);
    
private:
    void resized() override;
    
    juce::ImageButton artworkImageButton;
    juce::Label       titleLabel;
    juce::Label       artistLabel;
};
