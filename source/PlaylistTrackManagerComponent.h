#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <vector>

#include "MIR.h"
#include "NonModalAlertWindow.h"
#include "SidePanelWithBrowser.h"
#include "TrackInfoComponent.h"

class PlaylistTrackManagerComponent
    : public juce::Component
    , public juce::TableListBoxModel
    , public juce::Button::Listener
    , public juce::ChangeBroadcaster
{
public:
    //==============================================================================
    PlaylistTrackManagerComponent(juce::AudioFormatManager& _formatManager);
    //==============================================================================
    class Listener;
    //==============================================================================
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
    //==============================================================================
    int getColumnAutoSizeWidth(int columnId) override;
    void sortOrderChanged(int newSortColumnId, bool isForwards) override;
    void cellDoubleClicked (int rowNumber, int columnId, const juce::MouseEvent&) override;
    //==============================================================================

//    /** triggers when cell is clicked */
//    void cellClicked( int rowNumber
//                    , int columnId
//                    , const juce::MouseEvent&) override;
    
//    /** triggers when a different row is selected */
//    void selectedRowsChanged(int lastRowSelected) override;
    
    /** triggers when button clicked */
    void buttonClicked(juce::Button* button) override;
    //==============================================================================

    /** insert track data into respective vectors */
    void insertTracks(juce::File& audioFile);
    
    juce::XmlElement* GetTrack(int index);
    juce::XmlElement* GetFirstSongInPlaylist();
    
    void SetLastTrackNoPlayed(int trackNo) { lastTrackNoPlayed = trackNo; tableComponent.repaint(); }
    int  GetLastTrackNoPlayed()            { return lastTrackNoPlayed; }
    
    void   SetPlaylistLimit(double limit);
    double GetPlaylistLimit();
    
    void RowPlayButtonClicked(const int& row);

    void AddListener   (Listener &l) { listeners.add(&l); }
    void RemoveListener(Listener &l) { listeners.remove(&l); }
    
    void LoadPlaylist(const juce::File& xmlFile);
    
    void ExtractBPM(bool shouldReanalyse);
    //==============================================================================
private:
    class PlaylistDataSorter;
    
    juce::AudioFormatManager& formatManager;
    juce::File                playlistXmlFile;
    juce::Font                font{ 14.0f };
    
    juce::TableListBox    tableComponent;
    juce::TextEditor      searchBar;
    juce::TextButton      addButton;
    juce::ImageButton     settingsButton;
    juce::Label           playlistNameLabel;
    juce::Label           playlistDurationLabel;
    juce::Label           playlistLimitReachedLabel;
    SidePanelWithBrowser  sidePanel;
    
    std::unique_ptr<juce::XmlElement> playlistData;
    juce::XmlElement* columnList    = nullptr;
    juce::XmlElement* dataList      = nullptr;

    void         UpdateTrackID();
    void         UpdateDurationLabel();
    void         UpdateFiles();
    void         SetID3v2Tags(juce::XmlElement* track, juce::File& audioFile);
    void         RemoveTrackFromPlaylist(int row);
    juce::String secondsToMins(double seconds, bool asText);
    void         SettingsButtonClicked();
    void         EditButtonClicked(int row);

    int          numRows = 0;
    int          rowToEdit;
    int          totalTracksInPlaylist;
    int          lastTrackNoPlayed;
    double       playlistTotalDurationSecs = 0.0;
    double       playlistTotalTimeLimitSecs = 36000.0;
    juce::String currentTrackUUID;

    juce::ListenerList<Listener> listeners;
    
    std::unique_ptr<MIRProcessThread>    MIR;
    std::unique_ptr<TrackInfoComponent>  infoComp;
    std::unique_ptr<NonModalAlertWindow> trackInfoWindow;
                              
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistTrackManagerComponent)
};

//==============================================================================
class TableImageButtonCustomComponent
    : public juce::Component
{
public:
    TableImageButtonCustomComponent();
    
    std::function<void(int)> ButtonPressed;
    
    void SetButtonImages( const bool resizeButtonNowToFitThisImage
                        , const bool rescaleImagesWhenButtonSizeChanges
                        , const bool preserveImageProportions
                        , const juce::Image& normalImage_
                        , const float imageOpacityWhenNormal
                        , juce::Colour overlayColourWhenNormal
                        , const juce::Image& overImage_
                        , const float imageOpacityWhenOver
                        , juce::Colour overlayColourWhenOver
                        , const juce::Image& downImage_
                        , const float imageOpacityWhenDown
                        , juce::Colour overlayColourWhenDown);

    
    void resized() override;

    void SetIsVisible(bool shouldBeVisible);
    void setRowAndColumn (int newRow, int newColumn);
    //==============================================================================
private:
    juce::ImageButton button;
    int row, columnId;
};

//==============================================================================
class PlaylistTrackManagerComponent::PlaylistDataSorter
{
public:
    PlaylistDataSorter(const juce::String& attributeToSortBy, bool forwards);

    int compareElements (juce::XmlElement* first, juce::XmlElement* second) const;

private:
    juce::String attributeToSort;
    int direction;
};

//==============================================================================
class PlaylistTrackManagerComponent::Listener
{
public:
    virtual ~Listener() = default;
    
    virtual void PlayButtonClicked(const int& row) = 0;
};

