#include "AlbumArtworkDisplayer.h"

#include "Fonts.h"
#include "TagLibFileHandler.h"

using namespace APG::internal;
//==============================================================================
AlbumArtworkDisplayer::AlbumArtworkDisplayer()
: infoComp(std::make_unique<TrackInfoComponent>(true))
{
    infoComp->AddButtonListener(this);
    addAndMakeVisible(artworkImageButton);
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(artistLabel);

    artworkImageButton.onClick=[=]()
    {
        trackInfoWindow     = std::make_unique<NonModalAlertWindow>(500, 350);
        getParentComponent()->addAndMakeVisible(trackInfoWindow.get());
        trackInfoWindow     ->AddCustomComponent(infoComp.get());
        trackInfoWindow     ->resized();
    };
    
    titleLabel .setFont(Fonts::GetFont(Fonts::Type::Regular, 22.f));
    artistLabel.setFont(Fonts::GetFont(Fonts::Type::Thin,    18.f));
}
//==============================================================================
void AlbumArtworkDisplayer::SetTrackToLoad(const juce::XmlElement* track)
{
    const juce::File filePath = track->getStringAttribute("FileLocation");
    const auto title          = track->getStringAttribute("Title");
    const auto artist         = track->getStringAttribute("Artist");
    
    auto transparent  = juce::Colours::transparentBlack;
    auto artworkImage = TagLibFileHandler::ExtractImageFromFile(filePath);
    artworkImageButton.setImages(false, true, true, artworkImage, 0.9f, transparent, artworkImage, 0.5f, transparent, artworkImage, 1.0f, transparent);
    
    infoComp->SetTrackToLoad(filePath);
    
    titleLabel .setText(title,  juce::dontSendNotification);
    artistLabel.setText(artist, juce::dontSendNotification);
}
//------------------------------------------------------------------------------

void AlbumArtworkDisplayer::resized()
{
    auto totalBounds = getLocalBounds();
    
    auto artworkBounds     = totalBounds.removeFromLeft(totalBounds.proportionOfWidth(0.25));
    auto titleLabelBounds  = totalBounds.removeFromTop (totalBounds.proportionOfHeight(0.5));
    auto artistLabelBounds = totalBounds;
    
    artworkImageButton.setBounds(artworkBounds);
    titleLabel        .setBounds(titleLabelBounds);
    artistLabel       .setBounds(artistLabelBounds);
}
//------------------------------------------------------------------------------
void AlbumArtworkDisplayer::buttonClicked(juce::Button* button)
{
    if(button->getButtonText() == "Back")
        trackInfoWindow.reset();
}
