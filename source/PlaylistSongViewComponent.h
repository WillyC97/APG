#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <vector>

#include "SidePanelWithBrowser.h"

class PlaylistSongViewComponent
    : public juce::Component
    , public juce::TableListBoxModel
    , public juce::Button::Listener
{
public:
    //==============================================================================
    PlaylistSongViewComponent(juce::AudioFormatManager& _formatManager);
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
    std::vector<std::string> trackTitles;

    /** insert track data into respective vectors */
    void insertTracks(juce::File& audioFile);
    
    juce::XmlElement* GetTrack(int index);
    juce::XmlElement* GetFirstSongInPlaylist();
    
    void SetLastTrackNoPlayed(int trackNo) { lastTrackNoPlayed = trackNo; tableComponent.repaint(); }
    int  GetLastTrackNoPlayed()            { return lastTrackNoPlayed; }
    
    void RowPlayButtonClicked(const int& row);

    void AddListener   (Listener &l) { listeners.add(&l); }
    void RemoveListener(Listener &l) { listeners.remove(&l); }
    
    void LoadPlaylist(const juce::File& xmlFile);
    //==============================================================================
private:
    class PlaylistDataSorter;
    
    juce::AudioFormatManager& formatManager;
    juce::File                playlistXmlFile;
    juce::Font                font{ 14.0f };
    
    juce::TableListBox    tableComponent;
    juce::TextEditor      searchBar;
    juce::TextButton      addButton;
    juce::Label           playlistNameLabel;
    juce::Label           playlistDurationLabel;
    SidePanelWithBrowser  sidePanel;
    
    std::unique_ptr<juce::XmlElement> playlistData;
    juce::XmlElement* columnList    = nullptr;
    juce::XmlElement* dataList      = nullptr;

    void         UpdateTrackID();
    void         UpdateDurationLabel();
    juce::String secondsToMins(double seconds);

    int          numRows = 0;
    int          totalTracksInPlaylist;
    int          lastTrackNoPlayed;
    double       playlistTotalDurationSecs = 0.0;
    juce::String currentTrackUUID;

    juce::ListenerList<Listener> listeners;
                              
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistSongViewComponent)
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

    void setRowAndColumn (int newRow, int newColumn);
    //==============================================================================
private:
    juce::ImageButton button;
    int row, columnId;
};

//==============================================================================
class PlaylistSongViewComponent::PlaylistDataSorter
{
public:
    PlaylistDataSorter(const juce::String& attributeToSortBy, bool forwards);

    int compareElements (juce::XmlElement* first, juce::XmlElement* second) const;

private:
    juce::String attributeToSort;
    int direction;
};

//==============================================================================
class PlaylistSongViewComponent::Listener
{
public:
    virtual ~Listener() = default;
    
    virtual void PlayButtonClicked(const int& row) = 0;
};

