#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
: audioPlayer(audioFormatManager)
, playlistComponent(audioFormatManager)
{
    addAndMakeVisible(sidePanel);
    addAndMakeVisible(playlistComponent);
    
    addButton.setButtonText("Add");
    addButton.onClick=[this]()
    {
        auto files = sidePanel.getFiles();
        for (auto id : files)
            playlistComponent.insertTracks(id);
    };
    addAndMakeVisible(addButton);

    sidePanelButton.setButtonText("Browse Files");
    sidePanelButton.onClick=[this](){ sidePanel.showOrHide(!sidePanel.isPanelShowing()); };
    addAndMakeVisible(sidePanelButton);
    
    setSize (600, 400);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setFont (juce::Font (16.0f));
    g.setColour (juce::Colours::white);
    g.drawText ("Hello World!", getLocalBounds(), juce::Justification::centred, true);
}

void MainComponent::resized()
{
    auto totalBounds = getLocalBounds();
    auto panelWidth = totalBounds.proportionOfWidth(0.6);
    auto playlistBounds = totalBounds.removeFromRight(panelWidth);
    addButton.setBounds(300, 350, 50, 50);
    sidePanelButton.setBounds(360, 350, 50, 50);
    playlistComponent.setBounds(playlistBounds);
    
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}
