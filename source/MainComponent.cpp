#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
: audioPlayer(audioFormatManager)
, filter(std::make_unique<juce::WildcardFileFilter>("*.wav;*.aiff;*.mp3", "", "audio files"))
, browser(std::make_unique<juce::FileBrowserComponent> ( juce::FileBrowserComponent::openMode
                                                       | juce::FileBrowserComponent::canSelectFiles
                                                       | juce::FileBrowserComponent::canSelectMultipleItems
                                                       , juce::File()
                                                       , nullptr
                                                       , nullptr))
{
    browser->setFileFilter(filter.get());
    
    sidePanel = std::make_unique<juce::SidePanel>("File Browser", 500, false, browser.release());
    addAndMakeVisible(sidePanel.get());
    
    addButton.setButtonText("Add");
    addButton.onClick=[this]()
    {
        auto currentBrowser = dynamic_cast<juce::FileBrowserComponent*>(sidePanel->getContent());
        DBG(currentBrowser->getSelectedFile(0).getFileName().toStdString());
//        auto files = getFiles();
//        for (auto id : files)
//            DBG((id.getFileNameWithoutExtension().toStdString()));
    };
    addAndMakeVisible(addButton);

    sidePanelButton.setButtonText("Browse Files");
    sidePanelButton.onClick=[this](){ sidePanel->showOrHide(!sidePanel->isPanelShowing()); };
    addAndMakeVisible(sidePanelButton);
//    addAndMakeVisible(browser);
    
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
//    browser.setBounds(0, 0, 600, 300);
    addButton.setBounds(300, 350, 50, 50);
    sidePanelButton.setBounds(360, 350, 50, 50);
    
    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
}

juce::Array<juce::File> MainComponent::getFiles()
{
    juce::Array<juce::File> files;
    auto numSelectedFiles = browser->getNumSelectedFiles();
    
    for (int i = 0; i < numSelectedFiles; ++i)
        if (browser->currentFileIsValid())
            files.add(browser->getSelectedFile(i));

    return files;
}
