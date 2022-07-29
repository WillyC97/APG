#include <algorithm>

#include "PlaylistTrackManagerComponent.h"
#include "BinaryData.h"
#include "Fonts.h"

#include "TagLibFileHandler.h"

using namespace APG::internal;

PlaylistTrackManagerComponent::PlaylistTrackManagerComponent(juce::AudioFormatManager& _formatManager)
: formatManager(_formatManager)
, sidePanel(this)
, infoComp(std::make_unique<TrackInfoComponent>(false))
{
    #ifndef JUCE_LINUX
        auto& lnf = getLookAndFeel();
        lnf.setDefaultSansSerifTypeface(lnf.getTypefaceForFont(Fonts::GetFont(Fonts::Regular, 14.f)));
    #endif

    tableComponent.getHeader().addColumn("No.",      1, 12);
    tableComponent.getHeader().addColumn("Title",    2, 300);
    tableComponent.getHeader().addColumn("Artist",   3, 300);
    tableComponent.getHeader().addColumn("Album",    4, 300);
    tableComponent.getHeader().addColumn("Duration", 5, 200);
    tableComponent.getHeader().addColumn("BPM",      6, 50);
    tableComponent.getHeader().addColumn("Play",     7, 50, 50, 50);
    tableComponent.getHeader().addColumn("Edit",     8, 65, 65, 65);

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
    
    infoComp->AddButtonListener(this);

    auto transparent = juce::Colours::transparentBlack;
    auto settingsButtonImage = juce::ImageCache::getFromMemory(BinaryData::settings_png, BinaryData::settings_pngSize);
    settingsButton.setImages(false, true, true, settingsButtonImage, 1.f, transparent, settingsButtonImage,
                             1.f, juce::Colours::white, settingsButtonImage, 0.8f, transparent);
    settingsButton.onClick=[this](){ SettingsButtonClicked(); };

    searchBar.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF878787));
    searchBar.setColour(juce::TextEditor::textColourId, juce::Colours::ghostwhite);
    searchBar.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentWhite);
    searchBar.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentWhite);
    searchBar.setTextToShowWhenEmpty("Search playlist", juce::Colours::ghostwhite);
    searchBar.setKeyboardType(juce::TextInputTarget::VirtualKeyboardType::textKeyboard);
    searchBar.onTextChange = [this]
    {
        std::string textTyped = searchBar.getText().toLowerCase().toStdString();
        for(auto* element : dataList->getChildWithTagNameIterator("TRACK"))
        {
            auto track = element->getStringAttribute("Title").toLowerCase().toStdString();
            if(track.find(textTyped) != std::string::npos)
            {
                auto row = element->getStringAttribute("No.").getIntValue() - 1;
                tableComponent.selectRow(row);
                tableComponent.scrollToEnsureRowIsOnscreen(row);
            }
        }
        if(textTyped.empty())
           tableComponent.deselectAllRows();
    };

    playlistLimitReachedLabel.setText("Playlist Duration Limit Reached...", juce::dontSendNotification);
    playlistLimitReachedLabel.setColour(juce::Label::textColourId, juce::Colours::turquoise);

    addAndMakeVisible(tableComponent);
    addAndMakeVisible(searchBar);
    addAndMakeVisible(addButton);
    addAndMakeVisible(settingsButton);
    addAndMakeVisible(playlistNameLabel);
    addAndMakeVisible(playlistDurationLabel);
    addChildComponent(playlistLimitReachedLabel);
    addAndMakeVisible(sidePanel);

    formatManager.registerBasicFormats();
}
//==============================================================================
void PlaylistTrackManagerComponent::paint(juce::Graphics& g)
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
void PlaylistTrackManagerComponent::resized()
{
    auto totalArea       = getLocalBounds();
    auto widthPadding    = totalArea.proportionOfWidth(0.01f);
    auto heightPadding   = totalArea.proportionOfHeight(0.01f);
    auto bannerHeight    = totalArea.proportionOfHeight(0.15);
    auto bannerArea      = totalArea.removeFromTop(bannerHeight);
                            bannerArea.removeFromLeft(widthPadding);
    auto leftBannerArea  = bannerArea.removeFromLeft(bannerArea.proportionOfWidth(0.5));
    auto labelArea       = leftBannerArea.removeFromBottom(bannerArea.proportionOfHeight(0.2));
    auto durationLblArea = labelArea.removeFromLeft(labelArea.proportionOfWidth(0.5));

    playlistNameLabel        .setBounds(leftBannerArea);
    playlistDurationLabel    .setBounds(durationLblArea);
    playlistLimitReachedLabel.setBounds(labelArea);
    playlistNameLabel        .setFont(Fonts::GetFont(Fonts::Type::SemiBold, leftBannerArea.getHeight()));
    playlistDurationLabel    .setFont(Fonts::GetFont(Fonts::Type::Thin, labelArea.getHeight()));
    playlistLimitReachedLabel.setFont(Fonts::GetFont(Fonts::Type::Regular, labelArea.getHeight()));

    auto settingsButtonArea = bannerArea.removeFromLeft(bannerArea.proportionOfWidth(0.1));
    bannerArea.removeFromLeft(widthPadding);
    bannerArea.removeFromRight(widthPadding);
    bannerArea.removeFromBottom(heightPadding);
    addButton.setBounds(bannerArea.removeFromBottom(bannerArea.proportionOfHeight(0.5)));
    settingsButton.setBounds(settingsButtonArea.removeFromBottom(settingsButtonArea.proportionOfHeight(0.5)));
    bannerArea.removeFromTop(heightPadding);
    bannerArea.removeFromBottom(heightPadding);
    searchBar.setBounds(bannerArea);
    searchBar.setFont(Fonts::GetFont(Fonts::Type::Thin, bannerArea.getHeight() * 0.9f));
    tableComponent.setBounds(totalArea);
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::paintRowBackground(juce::Graphics& g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
    if(rowIsSelected)
        g.fillAll(juce::Colour(0xFF5c5c5c));
    else
        g.fillAll(juce::Colours::transparentBlack);
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
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
juce::Component* PlaylistTrackManagerComponent::refreshComponentForCell( int rowNumber
                                                                   , int columnId
                                                                   , bool isRowSelected
                                                                   , juce::Component* existingComponentToUpdate)
{
    if (columnId == 7)
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

    if (columnId == 8)
    {
        auto* deleteButton = static_cast<TableImageButtonCustomComponent*> (existingComponentToUpdate);

        if (deleteButton == nullptr)
            deleteButton = new TableImageButtonCustomComponent();

        auto deleteButtonImage = juce::ImageCache::getFromMemory( BinaryData::settings_png
                                                                , BinaryData::settings_pngSize);

        deleteButton->setRowAndColumn (rowNumber, columnId);
        auto transparent = juce::Colours::transparentBlack;
        deleteButton->SetIsVisible(isRowSelected);
        deleteButton->SetButtonImages(false, true, true, deleteButtonImage, 0.9f, transparent, deleteButtonImage, 0.5f, transparent, deleteButtonImage, 1.0f, transparent);
        deleteButton->ButtonPressed = [=] (int row) { EditButtonClicked(row); };
        return deleteButton;
    }

    return nullptr;
}
//-----------------------------------------------------------------------------
int PlaylistTrackManagerComponent::getColumnAutoSizeWidth (int columnId)
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
void PlaylistTrackManagerComponent::cellDoubleClicked (int rowNumber, int /*columnId*/, const juce::MouseEvent&)
{
    RowPlayButtonClicked(rowNumber);
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::buttonClicked(juce::Button* button)
{
    if(button->getComponentID() == "add")
    {
        auto files = sidePanel.GetFiles();
        for(auto id : files)
            insertTracks(id);
    }
    else if(button->getButtonText() == "Back")
    {
        UpdateFiles();
        trackInfoWindow.reset();
    }
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::SettingsButtonClicked()
{
    juce::PopupMenu m;
    m.addItem (1, "Clear Playlist");

    m.showMenuAsync (juce::PopupMenu::Options(),
                    [this] (int result)
                    {
                        if (result == 0)
                        {
                            DBG("user dismissed the menu without picking anything");
                        }
                        else if (result == 1)
                        {
                            auto currentRows = getNumRows() - 1;
                            for (int i = currentRows; i >= 0; i--)
                                RemoveTrackFromPlaylist(i);
                        }
                    });
}
//-----------------------------------------------------------------------------

void PlaylistTrackManagerComponent::EditButtonClicked(int row)
{
    juce::PopupMenu m;
    m.addItem (1, "Edit track");
    m.addItem (2, "Remove track");

    m.showMenuAsync (juce::PopupMenu::Options(),
                    [=] (int result)
                    {
                        if (result == 0)
                        {
                            DBG("user dismissed the menu without picking anything");
                        }
                        else if (result == 1)
                        {
                            rowToEdit = row;
                            auto fileAsString = dataList->getChildElement(row)
                                                        ->getStringAttribute("FileLocation");
                            infoComp->SetTrackToLoad(juce::File(fileAsString));
                            trackInfoWindow     = std::make_unique<NonModalAlertWindow>(500, 350);
                            getParentComponent()->addAndMakeVisible(trackInfoWindow.get());
                            trackInfoWindow     ->AddCustomComponent(infoComp.get());
                            trackInfoWindow     ->resized();
                        }
                        else if ( result == 2)
                        {
                            RemoveTrackFromPlaylist(row);
                        }
                    });
}
//==============================================================================
int PlaylistTrackManagerComponent::getNumRows()
{
    return numRows;
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::sortOrderChanged (int newSortColumnId, bool isForwards)
{
    if (newSortColumnId != 0)
    {
        if (dataList)
        {
            PlaylistDataSorter sorter(tableComponent.getHeader().getColumnName(newSortColumnId), isForwards);
            dataList->sortChildElements (sorter);
            UpdateTrackID();
            playlistData->writeTo(playlistXmlFile);
            tableComponent.updateContent();
        }
    }
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::UpdateTrackID()
{
    for(int i = 0; i < numRows; ++i)
        dataList->getChildElement(i)->setAttribute ("No.", i + 1);
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::UpdateDurationLabel()
{
    playlistDurationLabel.setText( "Duration : " + secondsToMins(playlistTotalDurationSecs, true)
                                 , juce::dontSendNotification);
}
//==============================================================================
juce::XmlElement* PlaylistTrackManagerComponent::GetTrack(int index)
{
    auto track = dataList->getChildElement(index);
    currentTrackUUID = track->getStringAttribute("UUID");
    return track;
}
//-----------------------------------------------------------------------------
juce::XmlElement* PlaylistTrackManagerComponent::GetFirstSongInPlaylist()
{
    return dataList->getFirstChildElement();
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::RemoveTrackFromPlaylist(int row)
{
    if (row >= 0)
    {
        auto track = dataList->getChildElement(row);
        auto trackLen = track->getDoubleAttribute("DurationInSecs");
        playlistTotalDurationSecs -= trackLen;
        playlistData->getChildByName("PLAYLISTINFO")
                    ->setAttribute("PlaylistDurationSecs", playlistTotalDurationSecs);
        dataList->removeChildElement(track, true);
        totalTracksInPlaylist -= 1;
        numRows -= 1;
        UpdateTrackID();
        UpdateDurationLabel();
        playlistData->writeTo(playlistXmlFile);
        tableComponent.updateContent();
        tableComponent.repaint();
    }
}
//==============================================================================
void PlaylistTrackManagerComponent::RowPlayButtonClicked(const int& row)
{
    listeners.call([=](auto &l) { l.PlayButtonClicked(row); });
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::SetPlaylistLimit(double limit)
{
    if (playlistData)
    {
        playlistTotalTimeLimitSecs = limit * 60.0;
        DBG("Playlist limit: " << playlistTotalTimeLimitSecs);
        playlistData->getChildByName("PLAYLISTINFO")
                    ->setAttribute("PlaylistDurationLimit", playlistTotalTimeLimitSecs);
        playlistData->writeTo(playlistXmlFile);

        while (playlistTotalDurationSecs > playlistTotalTimeLimitSecs)
            RemoveTrackFromPlaylist(getNumRows() - 1);
    }
}
//-----------------------------------------------------------------------------
double PlaylistTrackManagerComponent::GetPlaylistLimit()
{
    return playlistData->getChildByName("PLAYLISTINFO")
                       ->getDoubleAttribute("PlaylistDurationLimit");
}
//-----------------------------------------------------------------------------
juce::String PlaylistTrackManagerComponent::secondsToMins(double seconds, bool asText)
{
    juce::String songLengthString = "";
    juce::String lhs;
    juce::String rhs;

    int secs = int(seconds) % 60;
    int mins = (int(seconds) - (secs)) / 60;

    lhs = juce::String(mins);
    rhs = juce::String(secs);

    auto lhsDesc = mins > 60 ? " hr "  : " min ";
    auto rhsDesc = mins > 60 ? " min"  : " sec";
    auto additionalZero = secs < 10 ? "0" : "";

    if (mins > 60)
    {
        int hrs = mins / 60;
        mins = mins % 60;
        lhs = juce::String(hrs);
        rhs = juce::String(mins);
    }

    if (asText)
        songLengthString = lhs + lhsDesc + rhs + rhsDesc;
    else
        songLengthString = lhs + ":" + additionalZero + rhs;

    return songLengthString;
}
//==============================================================================
void PlaylistTrackManagerComponent::LoadPlaylist(const juce::File &xmlFile)
{
    if (xmlFile == juce::File() || ! xmlFile.exists())
        return;

    playlistXmlFile            = xmlFile;
    playlistData               = juce::XmlDocument::parse(xmlFile);
    dataList                   = playlistData->getChildByName("DATA");
    auto playlistInfo          = playlistData->getChildByName("PLAYLISTINFO");
    auto playlistName          = playlistInfo->getStringAttribute("PlaylistName");
    playlistTotalDurationSecs  = playlistInfo->getDoubleAttribute("PlaylistDurationSecs");
    playlistTotalTimeLimitSecs = playlistInfo->getDoubleAttribute("PlaylistDurationLimit", 600);
    numRows                    = dataList->getNumChildElements();

    UpdateDurationLabel();
    playlistNameLabel.setText(playlistName, juce::dontSendNotification);
    tableComponent.updateContent();
    sendChangeMessage();
    repaint();
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::insertTracks(juce::File& audioFile)
{
    if (dataList)
    {
        std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor(audioFile));
        if (reader)
        {
            auto lengthInSamples       = reader->lengthInSamples;
            auto sampleRate            = reader->sampleRate;
            auto trackDurationSecs     = lengthInSamples/sampleRate;
            playlistTotalDurationSecs += trackDurationSecs;

            if (playlistTotalDurationSecs > playlistTotalTimeLimitSecs)
            {
                playlistLimitReachedLabel.setVisible(true);
                playlistLimitReachedLabel.setAlpha(1.0);
                juce::Desktop::getInstance().getAnimator().fadeOut(&playlistLimitReachedLabel, 4000);
                playlistTotalDurationSecs -= trackDurationSecs;
                return;
            }

            auto trackDurationString = secondsToMins(trackDurationSecs, false);

            totalTracksInPlaylist = getNumRows() + 1;
            UpdateDurationLabel();

            
            auto songUUID   = juce::Uuid();

            std::unique_ptr<juce::XmlElement> track;
            track = std::make_unique<juce::XmlElement>("TRACK");

            SetID3v2Tags(track.get(), audioFile);
            track->setAttribute("No.",          totalTracksInPlaylist);
            track->setAttribute("FileLocation", audioFile.getFullPathName());
            track->setAttribute("Duration",       trackDurationString);
            track->setAttribute("DurationInSecs", trackDurationSecs);
            track->setAttribute("UUID",           songUUID.toString());

            dataList    ->addChildElement(track.release());
            playlistData->getChildByName("PLAYLISTINFO")
                        ->setAttribute("PlaylistDurationSecs", playlistTotalDurationSecs);
            playlistData->writeTo(playlistXmlFile);
            track.reset();

            numRows += 1;
        }
        tableComponent.updateContent();
        tableComponent.repaint();
    }
}
//-----------------------------------------------------------------------------
void PlaylistTrackManagerComponent::SetID3v2Tags(juce::XmlElement* track, juce::File& audioFile)
{
    auto taggedFile = TagLibFileHandler::GetAudioFileProperties(audioFile);
    
    track->setAttribute("Title",  taggedFile.title);
    track->setAttribute("Artist", taggedFile.artist);
    track->setAttribute("Album",  taggedFile.album);
    track->setAttribute("Year",   taggedFile.year);
    track->setAttribute("Genre",  taggedFile.genre);
    playlistData->writeTo(playlistXmlFile);
}

void PlaylistTrackManagerComponent::UpdateFiles()
{
    auto editedTrackString = dataList->getChildElement(rowToEdit)
                                     ->getStringAttribute("FileLocation");
    auto editedTrackFile   = juce::File(editedTrackString);
    
    for(auto* element : dataList->getChildWithTagNameIterator("TRACK"))
    {
        auto trackFileString = element->getStringAttribute("FileLocation");
        if(editedTrackString == trackFileString)
            SetID3v2Tags(element, editedTrackFile);
    }
    tableComponent.updateContent();
    tableComponent.repaint();
}
void PlaylistTrackManagerComponent::ExtractBPM(bool shouldReanalyse)
{
    if(playlistData)
    {
        MIR = std::make_unique<MIRProcessThread>(dataList, getParentComponent(), shouldReanalyse);
        MIR->runThread();
        playlistData->writeTo(playlistXmlFile);
        tableComponent.updateContent();
        tableComponent.repaint();
    }
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
void TableImageButtonCustomComponent::SetIsVisible(bool shouldBeVisible)
{
    button.setVisible(shouldBeVisible);
}
//-----------------------------------------------------------------------------
void TableImageButtonCustomComponent::setRowAndColumn (int newRow, int newColumn)
{
    row      = newRow;
    columnId = newColumn;
}

//==============================================================================
//Mark - PlaylistDataSorter

PlaylistTrackManagerComponent::PlaylistDataSorter::PlaylistDataSorter(const juce::String& attributeToSortBy, bool forwards)
    : attributeToSort(attributeToSortBy)
    , direction(forwards ? 1 : -1)
{}

int PlaylistTrackManagerComponent::PlaylistDataSorter::compareElements(juce::XmlElement* first, juce::XmlElement* second) const
{
    auto result = first->getStringAttribute (attributeToSort)
                        .compareNatural (second->getStringAttribute (attributeToSort));

    if (result == 0)
        result = first->getStringAttribute ("ID")
                       .compareNatural (second->getStringAttribute ("ID"));

    return direction * result;
}
