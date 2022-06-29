#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PlaylistSongViewComponent.h"

namespace
{
    constexpr auto MaxPlaylistLenMins = 600.0f;
    constexpr auto MinPlaylistLenMins = 0.0f;
}

class PlaylistSettingsComponent
    : public juce::Component
{
public:
    PlaylistSettingsComponent(PlaylistSongViewComponent& songViewCompToUse)
    : songViewComp(songViewCompToUse)
    {
        playlistLengthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        playlistLengthSlider.setRange(MinPlaylistLenMins, MaxPlaylistLenMins);
        playlistLengthSlider.setSkewFactorFromMidPoint(180);
        playlistLengthSlider.setNumDecimalPlacesToDisplay(2);
        playlistLengthSlider.setValue(MaxPlaylistLenMins);
        playlistLengthSlider.setTextValueSuffix(" mins");
        
        applyLimitButton.setButtonText("Apply Length Limit");
        applyLimitButton.onClick=[this](){ songViewComp.SetPlaylistLimit(playlistLengthSlider.getValue()); };
        addAndMakeVisible(playlistLengthSlider);
        addAndMakeVisible(applyLimitButton);
    }
    
    void resized() override
    {
        auto totalBounds = getLocalBounds();
        auto sliderHeight = totalBounds.proportionOfHeight(0.2);
        playlistLengthSlider.setBounds(totalBounds.removeFromTop(sliderHeight));
        applyLimitButton.setBounds(totalBounds.removeFromTop(sliderHeight/2));
    }
    
private:
    PlaylistSongViewComponent& songViewComp;
    juce::Slider               playlistLengthSlider;
    juce::TextButton           playlistBPMButton;
    juce::TextButton           applyLimitButton;
};
