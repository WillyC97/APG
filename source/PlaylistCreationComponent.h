#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <tuple>

#include "NonModalAlertWindow.h"

/// A component that manages playlists
///
/// Populates a listbox with the available playlists
/// Clicking a playlists triggers a change message for all listeners
/// Can create, update and delete playlists
class PlaylistCreationComponent
    : public juce::Component
    , public juce::ListBoxModel
    , public juce::ChangeBroadcaster
    , public juce::Button::Listener
{
public:
    PlaylistCreationComponent();
    
    //juce::Component
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    //juce::ListBoxModel
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void listBoxItemClicked (int row, const juce::MouseEvent& e) override;
    void listBoxItemDoubleClicked (int row, const juce::MouseEvent&) override;
    
    //juce::Button::Listener
    void buttonClicked(juce::Button* button) override;
    
    juce::File& GetPlaylist();
    
private:
    void CreatePlaylist    (const juce::String& playlistName);
    void UpdatePlaylistName(const juce::String& playlistName);
    void SetPlaylistNames();
    void UpdateNumPlaylists();
    void UpdateContent();
    
    void LaunchDialogBox();
    void ShowPopupMenu(int row);
    
    void mouseEnter(const juce::MouseEvent& event) override { UpdateRowComponentState(event); }
    void mouseExit (const juce::MouseEvent& event) override { UpdateRowComponentState(event); }
    void UpdateRowComponentState(const juce::MouseEvent& event);
    
    int numPlaylists;
    int currentHoverRow;
    
    bool createPlaylistSwitch = true;
    
    juce::File       lastPlaylistClicked;
    juce::TextButton addPlaylistButton;
    juce::ListBox    listBox;
    juce::Array<std::tuple<juce::String, juce::File>> playlistNames;
    std::unique_ptr<NonModalAlertWindowOkCancel> nmaw;
};
