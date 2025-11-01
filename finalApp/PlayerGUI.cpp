#pragma once
#include "PlayerGUI.h"

#include "GUI Components/TopBarComp.h"
#include "GUI Components/NormalViewComp.h"
#include "GUI Components/LeftNavComp.h"
#include "GUI Components/BottomControlComp.h"
#include "GUI Components/PlaylistViewComp.h"
#include "GUI Components/EditorComp.h"
#include "GUI Components/MarkerComp.h"
#include "Extra Functionalities/Marker.h"


PlayerGUI::PlayerGUI(PlayerAudio& audio_player) : audio_player(audio_player) {

    topBar = std::make_unique<TopBarComp>(audio_player);
    addAndMakeVisible(topBar.get());

    topBar->onFileLoaded = [&](const juce::File& file) {
        playlistView->add_playlist_entry(file);
    };

    normalView = std::make_unique<NormalViewComp>(audio_player);
    addAndMakeVisible(normalView.get());

    playlistView = std::make_unique<PlaylistViewComp>(audio_player);
    addAndMakeVisible(playlistView.get());

    playlistView->onAudioSelected= [&](int index) {
        audio_player.playFile(index);
        normalView->update(audio_player.getPlaylistFile(index));
    };

    navBar = std::make_unique<LeftNavComp>();
    addAndMakeVisible(navBar.get());

    navBar->set_view = [&](int view_index) {
        this->setView(viewArr[view_index]);
    };

    controlBar = std::make_unique<BottomControlComp>(audio_player);
    addAndMakeVisible(controlBar.get());

    editorView = std::make_unique<EditorComp>(audio_player);
    addAndMakeVisible(editorView.get());

    markerView = std::make_unique<MarkerComp>(audio_player);
    addAndMakeVisible(markerView.get());

    setView(View::Normal);
    startTimerHz(30);
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void PlayerGUI::resized()
{
    auto bounds = getLocalBounds();

    // Define region sizes
    int topBarHeight = 0.05*getWidth();
    int leftBarWidth = 0.1 * getWidth();
    int bottomBarHeight = 0.05*getWidth();

    // Place the components
    topBar->setBounds(bounds.removeFromTop(topBarHeight));
    navBar->setBounds(bounds.removeFromLeft(leftBarWidth));
    controlBar->setBounds(bounds.removeFromBottom(bottomBarHeight));

    // 'bounds' now holds the remaining rectangle for the main view
    switch (currentView)
    {
    case View::Normal:
        normalView->setBounds(bounds);
        break;
    case View::Playlist:
        playlistView->setBounds(bounds);
        break;
    case View::Editor:
        editorView->setBounds(bounds);
        break;
    case View::Marker:
        markerView->setBounds(bounds);
        break;
    }
    
}

void PlayerGUI::setView(View newView)
{
    switch (currentView)
    {
    case View::Normal:
        normalView->setVisible(false);
        break;
    case View::Playlist:
        playlistView->setVisible(false);
        break;
    case View::Editor:
        editorView->setVisible(false);
        break;
    case View::Marker:
        markerView->setVisible(false);
        break;
    }

    currentView = newView;

    switch (currentView)
    {
    case View::Normal:
        normalView->setVisible(true);
        break;
    case View::Playlist:
        playlistView->setVisible(true);
        break;
    case View::Editor:
        editorView->setVisible(true);
        break;
    case View::Marker:
        markerView->setVisible(true);
        break;
    }
    resized();
    repaint();
}

void PlayerGUI::timerCallback()
{
    // This timer now just updates the transport bar
    if (audio_player.isWokring())
    {
        controlBar->update();
        normalView->update(audio_player.getPlaylistFile(audio_player.getIndex()));
    }
}
