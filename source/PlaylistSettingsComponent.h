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
        playlistLengthSlider.setColour(juce::Slider::trackColourId,      juce::Colours::turquoise);
        playlistLengthSlider.setColour(juce::Slider::thumbColourId,      juce::Colours::whitesmoke);
        playlistLengthSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::slategrey);
        playlistLengthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        playlistLengthSlider.setRange(MinPlaylistLenMins, MaxPlaylistLenMins);
        playlistLengthSlider.setSkewFactorFromMidPoint(180);
        playlistLengthSlider.setNumDecimalPlacesToDisplay(2);
        playlistLengthSlider.setValue(MaxPlaylistLenMins);
        playlistLengthSlider.setTextValueSuffix(" mins");
        
        applyLimitButton.setButtonText("Apply Length Limit");
        applyLimitButton.onClick=[this](){ songViewComp.SetPlaylistLimit(playlistLengthSlider.getValue()); };
        applyLimitButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
        addAndMakeVisible(playlistLengthSlider);
        addAndMakeVisible(applyLimitButton);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF1c1c1c));
    }
    
    void resized() override
    {
        auto totalBounds  = getLocalBounds();
        auto sliderHeight = totalBounds.proportionOfHeight(0.2);
        auto widthPadding = totalBounds.proportionOfWidth(0.05);
                            totalBounds.removeFromLeft(widthPadding);
                            totalBounds.removeFromRight(widthPadding);
        
        playlistLengthSlider.setBounds(totalBounds.removeFromTop(sliderHeight));
        applyLimitButton    .setBounds(totalBounds.removeFromTop(sliderHeight/2));
    }
    
private:
    PlaylistSongViewComponent& songViewComp;
    juce::Slider               playlistLengthSlider;
    juce::TextButton           playlistBPMButton;
    juce::TextButton           applyLimitButton;
};
