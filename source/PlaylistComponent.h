#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include <vector>
//#include <string>
//#include <fstream>
#include "BinaryData.h"

class PlaylistComponent
    : public juce::Component
    , public juce::TableListBoxModel
    , public juce::Button::Listener
{
public:
    PlaylistComponent(juce::AudioFormatManager& _formatManager);
    
    struct TrackInformation;
    class Listener;

   /** fills the component's background and draws text */
    void paint(juce::Graphics&) override;

    /** set bounds of child components */
    void resized() override;

    /** get number of rows in playlist */
    int getNumRows() override;

    /** fills row background */
    void paintRowBackground( juce::Graphics&
                           , int rowNumber
                           , int width
                           , int height
                           , bool rowIsSelected) override;

    /** fills row with text and colour */
    void paintCell( juce::Graphics&
                  , int rowNumber
                  , int columnId
                  , int width
                  , int height
                  , bool rowIsSelected) override;

    /** updates cell component */
    Component* refreshComponentForCell( int rowNumber
                                      , int columnId
                                      , bool isRowSelected
                                      , Component* existingComponentToUpdate) override;

    /** converts seconds into minutes with seconds in this etc(2:30) format */
    std::string secondsToMins(double seconds);

    /** triggers when cell is clicked */
    void cellClicked( int rowNumber
                    , int columnId
                    , const juce::MouseEvent&) override;

    /** triggers when button clicked */
    void buttonClicked(juce::Button* button) override;

    static std::vector<juce::File> songURL;
    static std::vector<std::string> trackTitles;
    static std::vector<PlaylistComponent::TrackInformation> tracks;
    static unsigned int selectedRowNo;

    /** triggers when a different row is selected */
    void selectedRowsChanged(int lastRowSelected) override;

    /** records to a txt file what tracks are loaded */
    void savePlaylist(std::string playlistTracks);

    /** insert track data into respective vectors */
    void insertTracks(juce::File& audioFile);
    
    PlaylistComponent::TrackInformation GetFirstSongInPlaylist();
    PlaylistComponent::TrackInformation getFinalSongInPlaylist();
    
    void SetLastTrackNoPlayed(int trackNo) { lastTrackNoPlayed = trackNo; }
    int  GetLastTrackNoPlayed()            { return lastTrackNoPlayed; }
    
    void RowPlayButtonClicked(const int& row);

    void AddListener   (Listener &l) { listeners.add(&l); }
    void RemoveListener(Listener &l) { listeners.remove(&l); }
    
private:
    juce::AudioFormatManager& formatManager;
    
    juce::TableListBox tableComponent;
    
    std::vector<int>         trackNumber;
    std::vector<std::string> duration;
    
    juce::TextEditor searchBar;
    juce::TextButton addButton{ "Add tracks to playlist" };
    
    int totalTracksInPlaylist;
    int lastTrackNoPlayed;
    
    juce::ListenerList<Listener> listeners;
                              
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};

struct PlaylistComponent::TrackInformation
{
    const int          trackNumber;
    const juce::String trackName;
    const juce::String songDuration;
    const juce::File   songFileLocation;
};

//==============================================================================
class TableImageButtonCustomComponent
    : public juce::Component
{
public:
    TableImageButtonCustomComponent(PlaylistComponent& td)
        : owner (td)
    {
        auto playImage = juce::ImageCache::getFromMemory( BinaryData::play_png
                                                        , BinaryData::play_pngSize);
        auto transparent = juce::Colours::transparentBlack;
        playButton.setImages(false, true, true, playImage, 0.9f, transparent, playImage, 0.5f, transparent, playImage, 1.0f, transparent);
        addAndMakeVisible(playButton);

        playButton.onClick = [this] { owner.RowPlayButtonClicked(row); };
    }

    void resized() override
    {
        playButton.setBoundsInset (juce::BorderSize<int> (2));
    }

    void setRowAndColumn (int newRow, int newColumn)
    {
        row      = newRow;
        columnId = newColumn;
    }

private:
    PlaylistComponent& owner;
    juce::ImageButton playButton;
    int row, columnId;
};

//==============================================================================
class PlaylistComponent::Listener
{
public:
    virtual ~Listener() = default;
    
    virtual void PlayButtonClicked(const int& row) = 0;
};

