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

int Marker::Marker_cnt = 1;

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
        markerView->clear_markers();
    };

    navBar = std::make_unique<LeftNavComp>();
    addAndMakeVisible(navBar.get());

    navBar->set_view = [&](int view_index) {
        this->setView(viewArr[view_index]);
    };

    markerView = std::make_unique<MarkerComp>(audio_player);
    addAndMakeVisible(markerView.get());

    markerView->add_marker_bottomBar = [&](double pos) {
        controlBar->add_marker(pos);
    };

    markerView->clear_markers_buttomBar = [&]() {
        controlBar->clear_markers();
    };

    controlBar = std::make_unique<BottomControlComp>(audio_player);
    addAndMakeVisible(controlBar.get());

    controlBar->add_marker_in_markerView = [&](double pos) {
        double currentPosition = pos;
        int minutes = (int)currentPosition / 60;
        int seconds = (int)currentPosition % 60;
        juce::String timeText = juce::String::formatted("%02d:%02d", minutes, seconds);
        juce::String title = "Marker " + juce::String(MarkerEntry::get_marker_cnt());

        markerView->add_markers_list_entry(title, timeText, MarkerEntry::get_marker_cnt(), currentPosition);
    };

    controlBar->add_loaded_markers = [&]() {
        markerView->add_loaded_markers();
    };

    editorView = std::make_unique<EditorComp>(audio_player);
    addAndMakeVisible(editorView.get());

    setView(View::Normal);
    startTimerHz(30);

    // Load Session
    juce::File session_file("data.json");
    juce::var session;
    if (session_file.existsAsFile()) {
        juce::String json_data = session_file.loadFileAsString();
        session = juce::JSON::parse(json_data);
    }
    else return;

    if (session.isVoid()) return;
    if (
        session["playlist"].isVoid() ||
        session["last_played_index"].isVoid() ||
        session["timestamp"].isVoid())
        return;

    juce::Array<juce::var>* paths_from_json_file = session["playlist"].getArray();
    if (paths_from_json_file == nullptr) return;
    std::string last_played_path = session["last_audio_path"].toString().toStdString();

    for (auto& path : *paths_from_json_file) {
        juce::File f = juce::File(path.toString());
        if (audio_player.load(f)) playlistView->add_playlist_entry(f);
    }
    int last_played_index = (int)session["last_played_index"];
    juce::File file = juce::File((juce::String)last_played_path);
    if (file.existsAsFile()) {
        playlistView->onAudioSelected(last_played_index);
        audio_player.setPosition((double)session["timestamp"]);
        audio_player.play_pause();
    }


    juce::Array<juce::var>* markers_from_json_file = session["markers"].getArray();
    if (markers_from_json_file == nullptr) return;
    for (auto &p : *markers_from_json_file) {
        markerView->add_marker_pos((double)p);
    }
    controlBar = std::make_unique<BottomControlComp>(audio_player);
    addAndMakeVisible(controlBar.get());

    setView(View::Normal);
    startTimerHz(30);
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();
    juce::DynamicObject* session = new juce::DynamicObject();

    int current_audio_playing = audio_player.getIndex();
    if (~current_audio_playing) session->setProperty("last_audio_path", juce::String(playlistView->get_playlist_path(current_audio_playing)));
    session->setProperty("last_played_index", current_audio_playing);
    session->setProperty("timestamp", audio_player.getPosition());
    juce::Array<juce::var> arr;
    for (int i = 0; i < audio_player.getAudioCount(); i++) {
        auto& path = playlistView->get_playlist_path(i);
        arr.add(juce::String(path));
    }
    session->setProperty("playlist", arr);

    arr.clear();
    for (int i = 0; i < MarkerEntry::get_marker_cnt()-1; i++) {
        arr.add(markerView->get_marker_pos(i));
    }
    session->setProperty("markers", arr);

    juce::var session_json(session);
    juce::String jsonOutput = juce::JSON::toString(session_json, false);
    juce::File sessionFile("data.json");
    sessionFile.replaceWithText(jsonOutput);
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
        // editorView->setBounds(bounds);
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
        // editorView->setVisible(true);
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
        // editorView->setVisible(true);
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
