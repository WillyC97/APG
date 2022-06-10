# pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class SidePanelWithBrowser
    : public juce::SidePanel
{
public:
    SidePanelWithBrowser()
    : SidePanel("File Browser", 500, false)
    , filter(std::make_unique<juce::WildcardFileFilter>("*.wav;*.aiff;*.mp3", "", "audio files"))
    , browser(std::make_unique<juce::FileBrowserComponent>( juce::FileBrowserComponent::openMode
                                                          | juce::FileBrowserComponent::canSelectFiles
                                                          | juce::FileBrowserComponent::canSelectMultipleItems
                                                          , juce::File("~/Downloads")
                                                          , nullptr
                                                          , nullptr))
    {
        browser->setFileFilter(filter.get());
        setContent(browser.release());
    }
    
    juce::Array<juce::File> getFiles()
    {
        juce::Array<juce::File> files;
        auto currentBrowser = dynamic_cast<juce::FileBrowserComponent*>(getContent());
        auto numSelectedFiles = currentBrowser->getNumSelectedFiles();
        
        for (int i = 0; i < numSelectedFiles; ++i)
            if (currentBrowser->currentFileIsValid())
                files.add(currentBrowser->getSelectedFile(i));

        return files;
    }
    
private:
    const std::unique_ptr<juce::FileFilter>     filter;
    std::unique_ptr<juce::FileBrowserComponent> browser;
};
