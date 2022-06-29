#include <algorithm>

#include "PlaylistSongViewComponent.h"
#include "BinaryData.h"
#include "Fonts.h"

using namespace APG::internal;

PlaylistSongViewComponent::PlaylistSongViewComponent(juce::AudioFormatManager& _formatManager)
: formatManager(_formatManager)
, sidePanel(this)
{
    auto& lnf = getLookAndFeel();
    lnf.setDefaultSansSerifTypeface(lnf.getTypefaceForFont(Fonts::GetFont(Fonts::Regular, 14.f)));
    
    tableComponent.getHeader().addColumn("No.",      1, 12);
    tableComponent.getHeader().addColumn("Title",    2, 300);
    tableComponent.getHeader().addColumn("Duration", 3, 200);
    tableComponent.getHeader().addColumn("Play",     4, 50, 50, 50);
    tableComponent.getHeader().addColumn("Remove",   5, 65, 65, 65);

    tableComponent.getHeader().setStretchToFitActive(true);
    tableComponent.setHeaderHeight(35);
    tableComponent.setRowHeight(52);
    tableComponent.setModel(this);
    
    tableComponent.getVerticalScrollBar()  .setColour( juce::ScrollBar::thumbColourId
                                                      , juce::Colour(0xFFb8b8b8));
    tableComponent.getHorizontalScrollBar().setColour( juce::ScrollBar::thumbColourId
                                                     , juce::Colour(0xFFb8b8b8));
    
    addButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xFF1c1c1c));
    addButton.setButtonText("Browse Files");
    addButton.onClick=[this](){ sidePanel.showOrHide(!sidePanel.isPanelShowing()); };
    
    searchBar.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF878787));
    searchBar.setColour(juce::TextEditor::textColourId, juce::Colours::ghostwhite);
    searchBar.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentWhite);
    searchBar.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentWhite);
    searchBar.setTextToShowWhenEmpty("Search playlist", juce::Colours::ghostwhite);
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

    addAndMakeVisible(tableComponent);
    addAndMakeVisible(searchBar);
    addAndMakeVisible(addButton);
    addAndMakeVisible(playlistNameLabel);
    addAndMakeVisible(sidePanel);

    formatManager.registerBasicFormats();
}
//==============================================================================
void PlaylistSongViewComponent::paint(juce::Graphics& g)
{
    getLookAndFeel().setColour(juce::TableListBox::backgroundColourId,         juce::Colour(0xFF1c1c1c));
    getLookAndFeel().setColour(juce::TableHeaderComponent::backgroundColourId, juce::Colour(0xFF1c1c1c));
    getLookAndFeel().setColour(juce::TableHeaderComponent::textColourId,       juce::Colour(0xFFb8b8b8));
    getLookAndFeel().setColour(juce::TableHeaderComponent::outlineColourId,    juce::Colours::transparentBlack);
    
    auto totalArea    = getLocalBounds();
    auto bannerHeight = totalArea.proportionOfHeight(0.15);
    auto bannerArea   = totalArea.removeFromTop(bannerHeight);
    g.setGradientFill(juce::ColourGradient::vertical( juce::Colours::grey
                                                    , 0.f
                                                    , juce::Colour(0xFF1c1c1c)
                                                    , bannerHeight * 0.8f));
    g.fillRect(bannerArea);
}
//-----------------------------------------------------------------------------
void PlaylistSongViewComponent::resized()
{
    auto totalArea     = getLocalBounds();
    auto widthPadding  = totalArea.proportionOfWidth(0.01f);
    auto heightPadding = totalArea.proportionOfHeight(0.01f);
    auto bannerHeight  = totalArea.proportionOfHeight(0.15);
    auto bannerArea    = totalArea.removeFromTop(bannerHeight);
                          bannerArea.removeFromLeft(widthPadding);
    auto leftBannerArea = bannerArea.removeFromLeft(bannerArea.proportionOfWidth(0.5));
    
    playlistNameLabel.setBounds(leftBannerArea);
    playlistNameLabel.setFont(Fonts::GetFont(Fonts::Type::SemiBold, bannerHeight));
        
    bannerArea.removeFromRight(widthPadding);
    bannerArea.removeFromBottom(heightPadding);
    addButton.setBounds(bannerArea.removeFromBottom(bannerArea.proportionOfHeight(0.5)));
    bannerArea.removeFromTop(heightPadding);
    bannerArea.removeFromBottom(heightPadding);
    searchBar.setBounds(bannerArea);
    searchBar.setFont(Fonts::GetFont(Fonts::Type::Thin, bannerArea.getHeight() * 0.9f));
    tableComponent.setBounds(totalArea);
}
//-----------------------------------------------------------------------------
void PlaylistSongViewComponent::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
    if(rowIsSelected)
        g.fillAll(juce::Colour(0xFF5c5c5c));
    else
        g.fillAll(juce::Colours::transparentBlack);
}
//-----------------------------------------------------------------------------
void PlaylistSongViewComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    if (auto* rowElement = dataList->getChildElement(rowNumber))
    {
        if(rowElement->getStringAttribute("UUID") == currentTrackUUID)
            g.setColour(juce::Colours::turquoise);
        else
            g.setColour(juce::Colours::ghostwhite);
            
        auto columnName = tableComponent.getHeader().getColumnName(columnId);
        auto justification = columnName == "No." ? juce::Justification::centred : juce::Justification::centredLeft;
        auto text = rowElement->getStringAttribute(columnName);

        g.setFont(Fonts::GetFont(Fonts::Type::Regular, 16.f));
        g.drawText (text, 2, 0, width - 4, height, justification, true);
    }
}
//-----------------------------------------------------------------------------
juce::Component* PlaylistSongViewComponent::refreshComponentForCell( int rowNumber
                                                                   , int columnId
                                                                   , bool /*isRowSelected*/
                                                                   , juce::Component* existingComponentToUpdate)
{
    if (columnId == 4)
    {
        auto* playButton = static_cast<TableImageButtonCustomComponent*> (existingComponentToUpdate);
        
        if (playButton == nullptr)
            playButton = new TableImageButtonCustomComponent();
        
        auto playImage = juce::ImageCache::getFromMemory( BinaryData::play_png
                                                        , BinaryData::play_pngSize);

        playButton->setRowAndColumn (rowNumber, columnId);
        auto transparent = juce::Colours::transparentBlack;
        playButton->SetButtonImages(false, true, true, playImage, 0.9f, transparent, playImage, 0.5f, transparent, playImage, 1.0f, transparent);
        playButton->ButtonPressed = [=] (int row) { RowPlayButtonClicked(row); };
        return playButton;
    }
    
    if (columnId == 5)
    {
        auto* deleteButton = static_cast<TableImageButtonCustomComponent*> (existingComponentToUpdate);

        if (deleteButton == nullptr)
            deleteButton = new TableImageButtonCustomComponent();
        
        auto deleteButtonImage = juce::ImageCache::getFromMemory( BinaryData::cross_png
                                                                , BinaryData::cross_pngSize);

        deleteButton->setRowAndColumn (rowNumber, columnId);
        auto transparent = juce::Colours::transparentBlack;
        deleteButton->SetButtonImages(false, true, true, juce::Image(), 0.9f, transparent, deleteButtonImage, 0.5f, transparent, deleteButtonImage, 1.0f, transparent);
        deleteButton->ButtonPressed = [=] (int row)
        {
            dataList->removeChildElement(dataList->getChildElement(row), true);
            totalTracksInPlaylist -= 1;
            numRows -= 1;
            UpdateTrackID();
            playlistData->writeTo(playlistXmlFile);
            tableComponent.updateContent();
            tableComponent.repaint();
        };
        return deleteButton;
    }

    return nullptr;
}
//-----------------------------------------------------------------------------
int PlaylistSongViewComponent::getColumnAutoSizeWidth (int columnId)
{
    if (columnId == 9)
        return 50;

    int widest = 32;

    for (auto i = getNumRows(); --i >= 0;)
    {
        if (auto* rowElement = dataList->getChildElement (i))
        {
            auto text = rowElement->getStringAttribute (tableComponent.getHeader().getColumnName(columnId));

            widest = juce::jmax (widest, font.getStringWidth (text));
        }
    }

    return widest + 8;
}
//-----------------------------------------------------------------------------
void PlaylistSongViewComponent::cellDoubleClicked (int rowNumber, int /*columnId*/, const juce::MouseEvent&)
{
    RowPlayButtonClicked(rowNumber);
}
//-----------------------------------------------------------------------------
void PlaylistSongViewComponent::buttonClicked(juce::Button* button)
{
    auto buttonName = button->getComponentID();
    if(buttonName == "add")
    {
        auto files = sidePanel.GetFiles();
        for(auto id : files)
            insertTracks(id);
    }
}
//==============================================================================
int PlaylistSongViewComponent::getNumRows()
{
    return numRows;
}
//-----------------------------------------------------------------------------
void PlaylistSongViewComponent::sortOrderChanged (int newSortColumnId, bool isForwards)
{
    if (newSortColumnId != 0)
    {
        PlaylistDataSorter sorter(tableComponent.getHeader().getColumnName(newSortColumnId), isForwards);
        dataList->sortChildElements (sorter);
        UpdateTrackID();
        playlistData->writeTo(playlistXmlFile);
        tableComponent.updateContent();
    }
}
//-----------------------------------------------------------------------------
void PlaylistSongViewComponent::UpdateTrackID()
{
    for(int i = 0; i < numRows; ++i)
        dataList->getChildElement(i)->setAttribute ("No.", i + 1);
}
//==============================================================================
juce::XmlElement* PlaylistSongViewComponent::GetTrack(int index)
{
    auto track = dataList->getChildElement(index);
    currentTrackUUID = track->getStringAttribute("UUID");
    return track;
}
//-----------------------------------------------------------------------------
juce::XmlElement* PlaylistSongViewComponent::GetFirstSongInPlaylist()
{
    return dataList->getFirstChildElement();
}
//==============================================================================
void PlaylistSongViewComponent::RowPlayButtonClicked(const int& row)
{
    listeners.call([=](auto &l) { l.PlayButtonClicked(row); });
}
//-----------------------------------------------------------------------------
std::string PlaylistSongViewComponent::secondsToMins(double seconds)
{
    int secs = int(seconds) % 60;
    int mins = (int(seconds) - (secs)) / 60;
    std::string songLength = std::to_string(mins) + ":" + std::to_string(secs);
    return songLength;
}
//==============================================================================
void PlaylistSongViewComponent::LoadPlaylist(const juce::File &xmlFile)
{
    if (xmlFile == juce::File() || ! xmlFile.exists())
        return;

    playlistXmlFile   = xmlFile;
    playlistData      = juce::XmlDocument::parse(xmlFile);
    dataList          = playlistData->getChildByName("DATA");
    auto playlistName = playlistData->getChildByName("PLAYLISTNAME")
                                    ->getStringAttribute("PlaylistName");
    numRows           = dataList->getNumChildElements();
    
    playlistNameLabel.setText(playlistName, juce::dontSendNotification);
    tableComponent.updateContent();
    repaint();
}
//-----------------------------------------------------------------------------
void PlaylistSongViewComponent::insertTracks(juce::File& audioFile)
{
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor(audioFile));
    if (reader)
    {
        auto lengthInSamples = reader->lengthInSamples;
        auto sampleRate      = reader->sampleRate;

        auto title         = audioFile.getFileNameWithoutExtension().toStdString();
        auto artist        = audioFile.getFileNameWithoutExtension().toStdString();
        auto trackDuration = secondsToMins(lengthInSamples/sampleRate);
        totalTracksInPlaylist = getNumRows() + 1;
        
        std::unique_ptr<juce::XmlElement> track;
        track = std::make_unique<juce::XmlElement>("TRACK");
        track->setAttribute("No.", totalTracksInPlaylist);
        track->setAttribute("Title", title);
        track->setAttribute("Duration", trackDuration);
        track->setAttribute("FileLocation", audioFile.getFullPathName());
        track->setAttribute("UUID", juce::Uuid().toString());
        dataList    ->addChildElement(track.release());
        playlistData->writeTo(playlistXmlFile);
        track.reset();
        
        trackTitles.push_back(title);
        numRows += 1;
    }
    tableComponent.updateContent();
    tableComponent.repaint();
}

//==============================================================================
//Mark - TableImageButtonCustomComponent
TableImageButtonCustomComponent::TableImageButtonCustomComponent()
{
    addAndMakeVisible(button);
    button.onClick = [this] { if(ButtonPressed) ButtonPressed(row); };
}

void TableImageButtonCustomComponent::SetButtonImages( const bool resizeButtonNowToFitThisImage
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
                                                     , juce::Colour overlayColourWhenDown)
{
    button.setImages( resizeButtonNowToFitThisImage
                    , rescaleImagesWhenButtonSizeChanges
                    , preserveImageProportions
                    , normalImage_
                    , imageOpacityWhenNormal
                    , overlayColourWhenNormal
                    , overImage_
                    , imageOpacityWhenOver
                    , overlayColourWhenOver
                    , downImage_
                    , imageOpacityWhenDown
                    , overlayColourWhenDown);
}
//-----------------------------------------------------------------------------
void TableImageButtonCustomComponent::resized()
{
    button.setBoundsInset(juce::BorderSize<int> (14));
}
//-----------------------------------------------------------------------------
void TableImageButtonCustomComponent::setRowAndColumn (int newRow, int newColumn)
{
    row      = newRow;
    columnId = newColumn;
}

//==============================================================================
//Mark - PlaylistDataSorter

PlaylistSongViewComponent::PlaylistDataSorter::PlaylistDataSorter(const juce::String& attributeToSortBy, bool forwards)
    : attributeToSort(attributeToSortBy)
    , direction(forwards ? 1 : -1)
{}

int PlaylistSongViewComponent::PlaylistDataSorter::compareElements(juce::XmlElement* first, juce::XmlElement* second) const
{
    auto result = first->getStringAttribute (attributeToSort)
                        .compareNatural (second->getStringAttribute (attributeToSort));

    if (result == 0)
        result = first->getStringAttribute ("ID")
                       .compareNatural (second->getStringAttribute ("ID"));

    return direction * result;
}

