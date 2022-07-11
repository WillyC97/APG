#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <tuple>

class PlaylistCreationComponent
    : public juce::Component
    , public juce::ListBoxModel
    , public juce::ChangeBroadcaster
{
public:
    PlaylistCreationComponent();
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void listBoxItemClicked (int row, const juce::MouseEvent& e) override;
    
    juce::File& GetPlaylist();
    
private:
    void CreatePlaylist    (const juce::String& playlistName);
    void UpdatePlaylistName(const juce::String& playlistName);
    void SetPlaylistNames();
    void UpdateNumPlaylists();
    void UpdateContent();
    
    void LaunchDialogBox(bool createPlaylist);
    
    void mouseEnter(const juce::MouseEvent& event) override { UpdateRowComponentState(event); }
    void mouseExit (const juce::MouseEvent& event) override { UpdateRowComponentState(event); }
    void UpdateRowComponentState(const juce::MouseEvent& event);
    
    int numPlaylists;
    int currentHoverRow;
    
    juce::File       lastPlaylistClicked;
    juce::TextButton addPlaylistButton;
    juce::ListBox    listBox;
    juce::Array<std::tuple<juce::String, juce::File>> playlistNames;
};
