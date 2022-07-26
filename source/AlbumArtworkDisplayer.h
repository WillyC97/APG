#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "NonModalAlertWindow.h"
#include "TrackInfoComponent.h"

class AlbumArtworkDisplayer
    : public juce::Component
    , public juce::Button::Listener
{
public:
    AlbumArtworkDisplayer();

    void SetTrackToLoad(const juce::XmlElement* track);
    
private:
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    
    juce::ImageButton artworkImageButton;
    juce::Label       titleLabel;
    juce::Label       artistLabel;
    
    std::unique_ptr<TrackInfoComponent>  infoComp;
    std::unique_ptr<NonModalAlertWindow> trackInfoWindow;
};
