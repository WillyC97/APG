#include "NonModalAlertWindow.h"
#include "Fonts.h"

using namespace APG::internal;
//==============================================================================
namespace
{
    constexpr auto windowWidth  = 350;
    constexpr auto windowHeight = 200;
    constexpr auto padding      = 6;
}
//==============================================================================
NonModalAlertWindow::NonModalAlertWindow()
{
    setAlwaysOnTop(true);
    windowTitle.setFont(Fonts::GetFont(Fonts::Type::Regular, 24.f));
    windowTitle.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(windowTitle);
}
//------------------------------------------------------------------------------
NonModalAlertWindow::~NonModalAlertWindow()
{
    if(auto parent = getParentComponent())
        parent->removeComponentListener(this);
}
//------------------------------------------------------------------------------
void NonModalAlertWindow::parentHierarchyChanged()
{
    if(auto parent = getParentComponent())
    {
        toFront(true);
        parent->addComponentListener(this);
    }
}
//------------------------------------------------------------------------------
void NonModalAlertWindow::componentMovedOrResized( juce::Component &component
                                                 , bool             wasMoved
                                                 , bool             wasResized)
{
    juce::ignoreUnused(component, wasMoved);

    if (wasResized)
        resized();
}
//------------------------------------------------------------------------------
void NonModalAlertWindow::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.5f));
    g.setColour(juce::Colours::ghostwhite);
    auto bounds = getLocalBounds().withSizeKeepingCentre(windowWidth, windowHeight);
    auto cornerSize = 4.0f;
    g.drawRoundedRectangle(bounds.toFloat(), cornerSize, 1.0f);
        
    auto innerBounds = bounds.reduced(1);
    g.reduceClipRegion (innerBounds);

    g.setColour(juce::Colour(0xFF111212));
    g.fillRoundedRectangle (innerBounds.toFloat(), cornerSize);
}
//------------------------------------------------------------------------------
void NonModalAlertWindow::resized()
{
    setBounds(juce::Rectangle<int>(getParentWidth(), getParentHeight()));
    auto bounds      = getLocalBounds().withSizeKeepingCentre(windowWidth, windowHeight);
    auto innerBounds = bounds.reduced(padding);
    windowTitle.setBounds(innerBounds.removeFromTop(int(windowTitle.getFont().getHeight())));
}

//==================================================================================
//MARK: NonMondalAlertWindowOkCancel

NonModalAlertWindowOkCancel::NonModalAlertWindowOkCancel( const juce::String& title
                                                        , const juce::String& textBoxText)
{
    SetWindowTitle(title);

    okButton    .setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFb8b8b8));
    cancelButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFFb8b8b8));
    
    textEditor.setFont(Fonts::GetFont(Fonts::Type::Thin, 30.f));
    textEditor.setColour(juce::TextEditor::backgroundColourId,     juce::Colour(0xFF878787));
    textEditor.setColour(juce::TextEditor::textColourId,           juce::Colours::ghostwhite);
    textEditor.setColour(juce::TextEditor::outlineColourId,        juce::Colours::transparentWhite);
    textEditor.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentWhite);
    textEditor.setText(textBoxText);
    
    addAndMakeVisible(okButton);
    addAndMakeVisible(cancelButton);
    addAndMakeVisible(textEditor);
}
//------------------------------------------------------------------------------
void NonModalAlertWindowOkCancel::AddButtonListener(juce::Button::Listener* listener)
{
    okButton    .addListener(listener);
    cancelButton.addListener(listener);
}
//------------------------------------------------------------------------------
const juce::String NonModalAlertWindowOkCancel::GetText()
{
    return textEditor.getText();
}
//------------------------------------------------------------------------------

void NonModalAlertWindowOkCancel::resized()
{
    NonModalAlertWindow::resized();
    
    auto bounds             = getLocalBounds().withSizeKeepingCentre(windowWidth, windowHeight);
    auto buttonBounds       = bounds.removeFromBottom(50);
    auto cancelButtonBounds = buttonBounds.removeFromLeft(windowWidth/2);
    
    auto okayButtonBounds = buttonBounds;
    cancelButton.setBounds(cancelButtonBounds.reduced(padding));
    okButton    .setBounds(okayButtonBounds  .reduced(padding));
    
    bounds.removeFromBottom(10);
    auto texteditorBounds = bounds.removeFromBottom(60);
    textEditor.setBounds(texteditorBounds.reduced(10));
}
