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
        #elif JUCE_WINDOWS
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
    addPlaylistButton.onClick=[this](){ LaunchDialogBox(); };
    addAndMakeVisible(addPlaylistButton);
    
    listBox.setRowHeight(40);
    listBox.setModel(this);
    listBox.setColour(juce::ListBox::textColourId,       juce::Colours::ghostwhite);
    listBox.setColour(juce::ListBox::backgroundColourId, juce::Colour(0xFF111212));
    addAndMakeVisible(listBox);
}
//==============================================================================
void PlaylistCreationComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::blue);
    g.fillAll();
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::resized()
{
    auto totalBounds = getLocalBounds();
    auto buttonHeight = totalBounds.proportionOfHeight(0.1);
    auto buttonBounds = totalBounds.removeFromTop(buttonHeight);
    addPlaylistButton.setBounds(buttonBounds);
    listBox.setBounds(totalBounds);
}

//==============================================================================
void PlaylistCreationComponent::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
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
//==============================================================================
juce::File& PlaylistCreationComponent::GetPlaylist()
{
    return lastPlaylistClicked;
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::CreatePlaylist(const juce::String& playlistName)
{
    auto playlistFileName = "Playlist" + juce::String(numPlaylists) + ".xml";
    auto playlistFile     = playlistXmlLocation().getChildFile(playlistFileName);
    playlistFile.create();
    
    juce::XmlElement playlist("APG");
    auto playlistNameElement = std::make_unique<juce::XmlElement>("PLAYLISTNAME");
    playlistNameElement->setAttribute("PlaylistName", playlistName);
    auto data = std::make_unique<juce::XmlElement>("DATA");
    playlist.addChildElement(playlistNameElement.release());
    playlist.addChildElement(data.release());
    playlist.writeTo(playlistFile);
    SetPlaylistNames();
    UpdateNumPlaylists();
    listBox.updateContent();
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::UpdateNumPlaylists()
{
    numPlaylists = playlistXmlLocation().getNumberOfChildFiles(juce::File::TypesOfFileToFind::findFiles, "*.xml");
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::LaunchDialogBox()
{
    juce::AlertWindow aw("Create Playlist", "Create Playlist", juce::AlertWindow::QuestionIcon, this);
    auto initialText = "My Playlist " + juce::String(numPlaylists);
    aw.addTextEditor("name", initialText, "Preset name");
    aw.addButton("OK",     1, juce::KeyPress(juce::KeyPress::returnKey));
    aw.addButton("Cancel", 2, juce::KeyPress(juce::KeyPress::escapeKey));

    if (aw.runModalLoop() == 1)
    {
        const juce::String playlistName(aw.getTextEditor("name")->getText());
        
        if (playlistName.isEmpty())
        {
            juce::AlertWindow::showMessageBox( juce::AlertWindow::WarningIcon
                                             , "Invalid name"
                                             , "No Playlist name specified"
                                             , "OK");
            return;
        }
        
        CreatePlaylist(playlistName);
    }
}
//------------------------------------------------------------------------------
void PlaylistCreationComponent::SetPlaylistNames()
{
    auto playlists = playlistXmlLocation().findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*xml");
    
    playlistNames.clear();
    for(juce::File& playlist : playlists)
    {
        auto currentPlaylist = juce::XmlDocument::parse(playlist);
        auto playlistName = currentPlaylist->getChildByName("PLAYLISTNAME")
                                           ->getStringAttribute("PlaylistName");
        
        playlistNames.add(std::make_tuple(playlistName, playlist));
    }
}
//==============================================================================
