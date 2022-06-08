#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include <vector>
#include <string>
#include <fstream>

class PlaylistComponent
    : public juce::Component
    , public juce::TableListBoxModel
    , public juce::Button::Listener
{
public:
    PlaylistComponent(juce::AudioFormatManager& _formatManager);

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
    static int selectedRowNo;

    /** triggers when a different row is selected */
    void selectedRowsChanged(int lastRowSelected) override;

    /** records to a txt file what tracks are loaded */
    void savePlaylist(std::string playlistTracks);

    /** insert track data into respective vectors */
    void insertTracks(juce::File id);

private:
    juce::AudioFormatManager& formatManager;
    
    juce::TableListBox tableComponent;
    
    std::vector<int>         trackNumber;
    std::vector<std::string> duration;
    
    juce::TextEditor searchBar;
    juce::TextButton addButton{ "Add tracks to playlist" };
    
    int trackNo;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistComponent)
};


