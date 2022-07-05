# pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class BrowserWithButton
    : public juce::Component
{
public:
    BrowserWithButton()
    : filter(std::make_unique<juce::WildcardFileFilter>("*.wav;*.aiff;*.mp3", "", "audio files"))
    , browser(std::make_unique<juce::FileBrowserComponent>( juce::FileBrowserComponent::openMode
                                                          | juce::FileBrowserComponent::canSelectFiles
                                                          | juce::FileBrowserComponent::canSelectMultipleItems
                                                          , juce::File("~/Downloads")
                                                          , nullptr
                                                          , nullptr))
    {
        browser->setColour(juce::FileBrowserComponent::currentPathBoxBackgroundColourId, juce::Colour(0xFF1c1c1c));
        browser->setColour(juce::FileBrowserComponent::filenameBoxBackgroundColourId, juce::Colour(0xFF1c1c1c));
        browser->lookAndFeelChanged();
        browser->setFileFilter(filter.get());
        addAndMakeVisible(browser.get());
        
        addButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF1c1c1c));
        addButton.setButtonText("Add");
        addButton.setComponentID("add");
        addAndMakeVisible(addButton);
    }
    
    int GetNumSelectedFiles()
    {
        return browser->getNumSelectedFiles();
    }
    
    bool CurrentFileIsValid()
    {
        return browser->currentFileIsValid();
    }
    
    const juce::File GetSelectedFile(int index)
    {
        return browser->getSelectedFile(index);
    }
    
    void resized() override
    {
        const auto buttonHeight = 30;
        const auto margin = proportionOfHeight(0.01f);
        const auto buttonWidthMargin = proportionOfWidth(0.2f);
        auto bounds = getLocalBounds();
        bounds.removeFromBottom(margin);
        auto buttonArea = bounds.removeFromBottom(buttonHeight)
                                .withLeft(buttonWidthMargin)
                                .withRight(getWidth() - buttonWidthMargin);
        bounds.removeFromBottom(margin);
        addButton.setBounds(buttonArea);
        browser->setBounds(bounds);
    }
    
    void AddButtonListener(juce::Button::Listener* l)
    {
        addButton.addListener(l);
    }

    void RemoveButtonListener(juce::Button::Listener* l)
    {
        addButton.removeListener(l);
    }
        
private:
    const std::unique_ptr<juce::FileFilter>     filter;
    std::unique_ptr<juce::FileBrowserComponent> browser;
    juce::TextButton                            addButton;
};

class SidePanelWithBrowser
    : public juce::SidePanel
{
public:
    SidePanelWithBrowser(juce::Button::Listener* parent)
    : SidePanel("File Browser", 500, false)
    , browserWithButton(std::make_unique<BrowserWithButton>())
    {
        browserWithButton->AddButtonListener(parent);
        setContent(browserWithButton.release());
    }
    
    juce::Array<juce::File> GetFiles()
    {
        juce::Array<juce::File> files;
        auto currentBrowser = dynamic_cast<BrowserWithButton*>(getContent());
        auto numSelectedFiles = currentBrowser->GetNumSelectedFiles();

        for (int i = 0; i < numSelectedFiles; ++i)
            if (currentBrowser->CurrentFileIsValid())
                files.add(currentBrowser->GetSelectedFile(i));

        return files;
    }
    
private:
    std::unique_ptr<BrowserWithButton> browserWithButton;
};
