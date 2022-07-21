#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PlaylistTrackManagerComponent.h"
#include "Fonts.h"

using namespace APG::internal;
//==============================================================================
namespace
{
    constexpr auto MaxPlaylistLenMins = 600.0f;
    constexpr auto MinPlaylistLenMins = 0.0f;
}
//==============================================================================

class PlaylistSettingsComponent
    : public juce::Component
    , public juce::ChangeListener
{
public:
    PlaylistSettingsComponent(PlaylistTrackManagerComponent& trackManagerToUse)
    : trackManager(trackManagerToUse)
    {
        trackManager.addChangeListener(this);
        
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
        applyLimitButton.onClick=[this]()
        {
            trackManager.SetPlaylistLimit(playlistLengthSlider.getValue());
            SetLabelText();
        };
        applyLimitButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
        
        findBPMButton.setButtonText("Find BPM of Tracks");
        findBPMButton.onClick=[this](){ trackManager.ExtractBPM(); };
        findBPMButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
        
        addAndMakeVisible(playlistLengthSlider);
        addAndMakeVisible(applyLimitButton);
        addAndMakeVisible(findBPMButton);
        
        SetLabelText();
        addAndMakeVisible(currentLimitLabel);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xFF1c1c1c));
    }
    
    void resized() override
    {
        auto totalBounds  = getLocalBounds();
        auto compHeight   = totalBounds.proportionOfHeight(0.1);
        auto widthPadding = totalBounds.proportionOfWidth(0.05);
                            totalBounds.removeFromLeft(widthPadding);
                            totalBounds.removeFromRight(widthPadding);
        
        currentLimitLabel   .setBounds(totalBounds.removeFromTop(compHeight));
        currentLimitLabel   .setFont(Fonts::GetFont(Fonts::Type::Regular, compHeight/2));
        playlistLengthSlider.setBounds(totalBounds.removeFromTop(compHeight));
        applyLimitButton    .setBounds(totalBounds.removeFromTop(compHeight));
        totalBounds         .removeFromTop(compHeight/2);
        findBPMButton       .setBounds(totalBounds.removeFromTop(compHeight));
    }
    
    void changeListenerCallback (juce::ChangeBroadcaster* source) override
    {
        if (source == &trackManager)
        {
            playlistLengthSlider.setValue(trackManager.GetPlaylistLimit() / 60);
            SetLabelText();
        }
    }
    
    void SetLabelText()
    {
        auto message = "Current Limit: " + juce::String(int(playlistLengthSlider.getValue())) + " mins";
        currentLimitLabel.setText(message, juce::dontSendNotification);
    }
    
private:
    PlaylistTrackManagerComponent& trackManager;
    juce::Slider                   playlistLengthSlider;
    juce::TextButton               findBPMButton;
    juce::TextButton               applyLimitButton;
    juce::Label                    currentLimitLabel;
};
