#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

//==============================================================================
class NonModalAlertWindow
    : public juce::Component
    , private juce::ComponentListener
{
public:
    NonModalAlertWindow();
    ~NonModalAlertWindow() override;
    
    void resized() override;
    void paint(juce::Graphics& g) override;
    void SetWindowTitle(const juce::String& title) { windowTitle.setText(title, juce::dontSendNotification); }
    void WithButton() { okButton.setVisible(true); }
    
private:
    void parentHierarchyChanged() override;
    void componentMovedOrResized( juce::Component &component
                                , bool             wasMoved
                                , bool             wasResized) override;
    juce::Label      windowTitle;
    juce::TextButton okButton{ "Ok" };
};

//==============================================================================
class NonModalAlertWindowOkCancel
    : public NonModalAlertWindow
{
public:
    NonModalAlertWindowOkCancel( const juce::String& title
                               , const juce::String& textBoxText);
    
    void AddButtonListener(juce::Button::Listener* listener);
    void resized() override;
    
    const juce::String GetText();
    
private:
    juce::TextButton okButton     { "Ok" };
    juce::TextButton cancelButton { "Cancel" };
    juce::TextEditor textEditor;
};

//==============================================================================
class NonModalAlertWindowWithProgressBar
: public NonModalAlertWindow
{
public:
    NonModalAlertWindowWithProgressBar(const juce::String& title, double& progressValue);
    
    void SetMessageText(const juce::String& message);
    void AddButtonListener(juce::Button::Listener* listener);
    
    void resized() override;

private:
    std::unique_ptr<juce::ProgressBar> progressBar;
    juce::TextButton                   cancelButton { "Cancel" };
    juce::TextButton                   okButton          { "Ok" };
    juce::Label                        progressMessage;
};