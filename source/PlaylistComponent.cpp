#include <juce_audio_utils/juce_audio_utils.h>

#include "PlaylistComponent.h"

std::vector<std::string> PlaylistComponent::trackTitles;
std::vector<juce::File> PlaylistComponent::songURL;
int PlaylistComponent::selectedRowNo;

PlaylistComponent::PlaylistComponent(juce::AudioFormatManager& _formatManager)
: formatManager(_formatManager)
{
    trackNumber.push_back(0);
    trackNo = 1;

    tableComponent.getHeader().addColumn("Track number", 1, 50);
    tableComponent.getHeader().addColumn("Track title", 2, 200);
    tableComponent.getHeader().addColumn("Duration", 3, 200);

    tableComponent.getHeader().setStretchToFitActive(true);

    tableComponent.setModel(this);

    addAndMakeVisible(tableComponent);
    addAndMakeVisible(searchBar);
    addAndMakeVisible(addButton);

    addButton.addListener(this);

    searchBar.setTextToShowWhenEmpty("Search playlist", juce::Colours::antiquewhite);

    searchBar.onTextChange = [this]
    {
        int rowCount = 0;
        for (std::string track : trackTitles)
        {
            std::string textTyped = searchBar.getText().toStdString();
            std::transform(textTyped.begin(), textTyped.end(), textTyped.begin(), ::tolower);
            std::transform(track.begin(), track.end(), track.begin(), ::tolower);
            if (track.find(textTyped) != std::string::npos) {
                tableComponent.selectRow(rowCount);
                tableComponent.scrollToEnsureRowIsOnscreen(rowCount);
            }
            rowCount = rowCount + 1;
        }
    };

    formatManager.registerBasicFormats();
//    std::string line;
//    std::ifstream playlistFile("playlist.txt");
//    if (playlistFile.is_open())
//    {
//        while (getline(playlistFile, line))
//        {
//            juce::String path = line;
//            juce::File id(path);
//            insertTracks(id);
//        }
//        playlistFile.close();
//    }
  
}

void PlaylistComponent::paint(juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

   
    getLookAndFeel().setColour(juce::TableListBox::backgroundColourId, juce::Colour(0xFF041f18));
    getLookAndFeel().setColour(juce::TableHeaderComponent::backgroundColourId, juce::Colour(0xFF122a2d));
    getLookAndFeel().setColour(juce::TableHeaderComponent::textColourId, juce::Colours::lightcyan);
}

void PlaylistComponent::resized()
{
    auto totalArea = getLocalBounds();
    auto rowHeight = totalArea.proportionOfHeight(0.1);
    auto rowArea   = totalArea.removeFromTop(rowHeight);
    searchBar.setBounds(rowArea.removeFromLeft(rowArea.proportionOfWidth(0.5)));
    addButton.setBounds(rowArea);
    tableComponent.setBounds(totalArea);
}

int PlaylistComponent::getNumRows()
{
    return trackTitles.size();
}

void PlaylistComponent::paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::mediumaquamarine);
    }
    else {
        g.fillAll(juce::Colour(0xFF0b3839));
    }
}
void PlaylistComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (rowNumber < getNumRows())
    {
        g.setColour(juce::Colours::white);

        if (columnId == 1)
        {
            g.drawText(std::to_string(trackNumber[rowNumber + 1]), 1, 0, width, height, juce::Justification::centredLeft, true);
        }
        if (columnId == 2)
        {
            g.drawText(trackTitles[rowNumber], 1, 0, width, height, juce::Justification::centredLeft, true);
        }
        if (columnId == 3)
        {
            g.drawText(duration[rowNumber], 1, 0, width, height, juce::Justification::centredLeft, true);
        }
    }
  
}

juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
    int columnId,
    bool isRowSelected,
    juce::Component* existingComponentToUpdate)
{
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{}
//        juce::FileChooser chooser{ "" };
//        if (chooser.browseForMultipleFilesToOpen())
//        {
//            juce::Array<juce::File> ids = chooser.getResults();
//            for (File id : ids)
//            {
////                savePlaylist(id.getFullPathName().toStdString());
//                insertTracks(id);
//            }
//        }
//    tableComponent.updateContent();
//}

std::string PlaylistComponent::secondsToMins(double seconds)
{
    int secs = int(seconds) % 60;
    int mins = ( seconds - (secs)) / 60;
    std::string songLength = std::to_string(mins) + ":" + std::to_string(secs);
    return songLength;
}

void PlaylistComponent::cellClicked(int rowNumber,
    int columnId,
    const juce::MouseEvent&)
{
    selectedRowNo = trackNumber[tableComponent.getSelectedRow() + 1];
    tableComponent.repaint();
}

void PlaylistComponent::selectedRowsChanged(int lastRowSelected)
{
    selectedRowNo = trackNumber[tableComponent.getSelectedRow() + 1];
    tableComponent.repaint();
}

void PlaylistComponent::savePlaylist(std::string playlistTracks)
{
    std::ofstream outlog("playlist.txt", std::ofstream::app);
    outlog << playlistTracks << std::endl;
    outlog.close();
}

void PlaylistComponent::insertTracks(juce::File id)
{
    auto* reader = formatManager.createReaderFor(id);
//    auto* reader = formatManager.createReaderFor(juce::URL{ id }.createInputStream(false));
    if (reader != nullptr)
    {
        auto lengthInSamples = reader->lengthInSamples;
        auto sampleRate = reader->sampleRate;

        std::string title = id.getFileNameWithoutExtension().toStdString();
        std::string artist = id.getFileNameWithoutExtension().toStdString();
        std::string timeLength = secondsToMins(lengthInSamples/sampleRate);

        trackNumber.push_back(trackNo);
        trackTitles.push_back(title);
        duration.push_back(timeLength);
        songURL.push_back(id);
        trackNo = trackNo + 1;
    }
    tableComponent.updateContent();
}
