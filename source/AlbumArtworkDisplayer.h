#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "NonModalAlertWindow.h"
#include "TrackInfoComponent.h"

///Displays album artwork in an ImageButton
class AlbumArtworkDisplayer
    : public juce::Component
    , public juce::Button::Listener
{
public:
    AlbumArtworkDisplayer();

    /** Set the track with which to extract the album artwork
     
        If no artwork is present, it will show the default artwork image
        @param track - The track to extract the artwork from
    */
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
