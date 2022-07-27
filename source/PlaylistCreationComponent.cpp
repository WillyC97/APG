#include "PlaylistCreationComponent.h"
#include "Fonts.h"

using namespace APG::internal;
//==============================================================================
namespace
{
    const juce::File UserAppData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);

    juce::File playlistXmlLocation()
    {
        #ifdef JUCE_MAC
            juce::File fullPath = UserAppData.getChildFile("Application Support/APG/");
        #elif defined(JUCE_WINDOWS) || defined(JUCE_LINUX)
            juce::File fullPath = UserAppData.getChildFile("APG/");
        #endif
        return fullPath;
    }
}
//==============================================================================
PlaylistCreationComponent::PlaylistCreationComponent()
{
    UpdateNumPlaylists();
    if(numPlaylists == 0)
        CreatePlaylist("My Playlist");
    
    SetPlaylistNames();
    
    addPlaylistButton.setButtonText("Create Playlist");
    addPlaylistButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentWhite);
    addPlaylistButton.onClick=[this](){ LaunchDialogBox(); createPlaylistSwitch = true; };
    addAndMakeVisible(addPlaylistButton);
    
    #ifdef APG_TOUCH_UI
        listBox.getViewport()->setScrollOnDragMode(juce::Viewport::ScrollOnDragMode::all);
    #endif
    listBox.setRowHeight(40);
    listBox.setModel(this);
    listBox.addMouseListener(this, true);
    listBox.setColour(juce::ListBox::textColourId,       juce::Colours::ghostwhite);
    listBox.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xFF111212));
    listBox.getVerticalScrollBar().setColour( juce::ScrollBar::thumbColourId
                                            , juce::Colour(0xFFb8b8b8));
    addAndMakeVisible(listBox);
}
//==============================================================================
void PlaylistCreationComponent::paint(juce::Graphics& g)
{
    getLookAndFeel().setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xFF212121));
    getLookAndFeel().setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xFF878787));
    g.setColour(juce::Colour(0xFF111212));
    g.fillAll();
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::UpdateRowComponentState(const juce::MouseEvent& event)
{
    auto e = event.getEventRelativeTo(&listBox);
    currentHoverRow = listBox.getRowContainingPosition(e.x, e.y);
    listBox.repaint();
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::resized()
{
    auto totalBounds = getLocalBounds();
    auto widthPadding = totalBounds.proportionOfWidth(0.05);
    auto heightPadding = totalBounds.proportionOfHeight(0.01);
    auto buttonHeight = totalBounds.proportionOfHeight(0.1);
    auto buttonBounds = totalBounds.removeFromTop(buttonHeight);
    
    buttonBounds.removeFromRight(widthPadding);
    buttonBounds.removeFromLeft(widthPadding);
    buttonBounds.removeFromBottom(heightPadding);
    buttonBounds.removeFromTop(heightPadding);
    
    addPlaylistButton.setBounds(buttonBounds);
    listBox.setBounds(totalBounds);
}

//==============================================================================
void PlaylistCreationComponent::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected || currentHoverRow == rowNumber)
        g.setColour(juce::Colours::ghostwhite);
    else
        g.setColour(juce::Colour(0xFFb8b8b8));
    
    g.setFont(Fonts::GetFont(Fonts::Type::Regular, height * 0.5f));
   
    g.drawText(std::get<0>(playlistNames[rowNumber]), 5, 0, width, height,
               juce::Justification::centredLeft, true);
}
//------------------------------------------------------------------------------
int PlaylistCreationComponent::getNumRows()
{
    return numPlaylists;
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::selectedRowsChanged(int lastRowSelected)
{
    lastPlaylistClicked = std::get<1>(playlistNames[lastRowSelected]);
    sendChangeMessage();
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::listBoxItemClicked(int row, const juce::MouseEvent& e)
{
    if(e.mods.isRightButtonDown())
        ShowPopupMenu(row);
}
//------------------------------------------------------------------------------

void PlaylistCreationComponent::listBoxItemDoubleClicked(int row, const juce::MouseEvent&)
{
#ifdef APG_TOUCH_UI
    ShowPopupMenu(row);
#endif
}
//------------------------------------------------------------------------------

void PlaylistCreationComponent::buttonClicked(juce::Button *button)
{
    if (button->getName() == "Ok")
    {
        const juce::String playlistName(nmaw->GetText());
        if(playlistName.isEmpty())
            return;
        
        if (createPlaylistSwitch)
            CreatePlaylist(playlistName);
        else
            UpdatePlaylistName(playlistName);
    }
    nmaw.reset();
}
//==============================================================================
juce::File& PlaylistCreationComponent::GetPlaylist()
{
    return lastPlaylistClicked;
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::CreatePlaylist(const juce::String& playlistName)
{
    auto date = juce::Time::getCurrentTime().toISO8601(true);
    auto playlistFileName = "Playlist-" + date + ".xml";
    auto playlistFile     = playlistXmlLocation().getChildFile(playlistFileName);
    playlistFile.create();
    
    juce::XmlElement playlist("APG");
    auto playlistNameElement = std::make_unique<juce::XmlElement>("PLAYLISTINFO");
    playlistNameElement->setAttribute("PlaylistName", playlistName);
    auto data = std::make_unique<juce::XmlElement>("DATA");
    playlist.addChildElement(playlistNameElement.release());
    playlist.addChildElement(data.release());
    playlist.writeTo(playlistFile);
    UpdateContent();
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::UpdatePlaylistName(const juce::String &playlistName)
{
    auto playlist = juce::XmlDocument::parse(lastPlaylistClicked);
    playlist->getChildByName("PLAYLISTINFO")->setAttribute("PlaylistName", playlistName);
    playlist->writeTo(lastPlaylistClicked);
    UpdateContent();
    sendChangeMessage();
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::UpdateNumPlaylists()
{
    numPlaylists = playlistXmlLocation().getNumberOfChildFiles(juce::File::TypesOfFileToFind::findFiles, "*.xml");
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::UpdateContent()
{
    SetPlaylistNames();
    UpdateNumPlaylists();
    listBox.updateContent();
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::LaunchDialogBox()
{
    auto titleText = createPlaylistSwitch ? "Create Playlist" : "Update Playlist Name";
    auto initialText = "My Playlist " + juce::String(numPlaylists);

    auto parent = getParentComponent();
    nmaw = std::make_unique<NonModalAlertWindowOkCancel>(titleText, initialText);
    nmaw->AddButtonListener(this);
    parent->addAndMakeVisible(nmaw.get());
    nmaw->resized();
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::ShowPopupMenu(int row)
{
    juce::PopupMenu m;
    m.addItem (1, "Delete Playlist");
    m.addItem (2, "Rename Playlist");

    m.showMenuAsync (juce::PopupMenu::Options(),
                     [=] (int result)
                    {
                        if (result == 0)
                        {
                            DBG("user dismissed the menu without picking anything");
                        }
                        else if (result == 1)
                        {
                            if (getNumRows() != 1)
                            {
                                lastPlaylistClicked.deleteFile();
                                UpdateContent();
                                selectedRowsChanged(0);
                            }
                        }
                        else if (result == 2)
                        {
                            createPlaylistSwitch = false;
                            LaunchDialogBox();
                            selectedRowsChanged(row);
                        }
                    });
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::SetPlaylistNames()
{
    auto playlists = playlistXmlLocation().findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*xml");
    
    playlistNames.clear();
    for(juce::File& playlist : playlists)
    {
        auto currentPlaylist = juce::XmlDocument::parse(playlist);
        auto playlistName = currentPlaylist->getChildByName("PLAYLISTINFO")
                                           ->getStringAttribute("PlaylistName");
        
        playlistNames.add(std::make_tuple(playlistName, playlist));
    }
}
//==============================================================================
