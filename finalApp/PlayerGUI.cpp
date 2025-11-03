#pragma once
#include "PlayerGUI.h"

// Includes needed for functionality introduced in the finalApp branch
#include "MainComponent.h"
#include "GUI Components/TopBarComp.h"
#include "GUI Components/NormalViewComp.h"
#include "GUI Components/LeftNavComp.h"
#include "GUI Components/BottomControlComp.h"
#include "GUI Components/PlaylistViewComp.h"
#include "GUI Components/EditorComp.h"
#include "GUI Components/MarkerComp.h"
#include "GUI Components/MinimizedViewComp.h"
#include "GUI Components/MixerViewComp.h"
#include "Extra Functionalities/FloatingVolumeSlider.h"
#include "Extra Functionalities/Marker.h"

// The definition of a static member. Assuming Marker is the correct class name.
int Marker::Marker_cnt = 1;


// === PlayerGUI Constructor and Destructor ===

PlayerGUI::PlayerGUI(PlayerAudio& audio_player, PlayerAudio& mixer_player_1, PlayerAudio& mixer_player_2)
    : audio_player(audio_player),
    mixer_player_1(mixer_player_1),
    mixer_player_2(mixer_player_2)
{
    // Initialize components
    topBar = std::make_unique<TopBarComp>(audio_player);
    addAndMakeVisible(topBar.get());

    miniView = std::make_unique<MinimizedViewComp>(audio_player);
    addAndMakeVisible(miniView.get());

    normalView = std::make_unique<NormalViewComp>(audio_player);
    addAndMakeVisible(normalView.get());

    playlistView = std::make_unique<PlaylistViewComp>(audio_player);
    addAndMakeVisible(playlistView.get());

    navBar = std::make_unique<LeftNavComp>();
    addAndMakeVisible(navBar.get());

    markerView = std::make_unique<MarkerComp>(audio_player);
    addAndMakeVisible(markerView.get());

    controlBar = std::make_unique<BottomControlComp>(audio_player);
    addAndMakeVisible(controlBar.get());

    editorView = std::make_unique<EditorComp>(audio_player);
    addAndMakeVisible(editorView.get());

    mixerView = std::make_unique<MixerViewComp>(mixer_player_1, mixer_player_2);
    addAndMakeVisible(mixerView.get());

    volume_slider = std::make_unique<FloatingVolumeSlider>(audio_player);
    addAndMakeVisible(volume_slider.get());
    volume_slider->toFront(true); // Ensure slider is always on top


    // Set up Callbacks
    topBar->onFileLoaded = [&](const juce::File& file) {
        playlistView->add_playlist_entry(file);
        };

    playlistView->onAudioSelected = [&](int index) {
        audio_player.playFile(index);
        normalView->update(audio_player.getPlaylistFile(index));
        markerView->clear_markers();
        };

    navBar->set_view = [&](int view_index) {
        // Use static_cast for robustness and clarity
        this->setView(static_cast<View>(view_index));
        };


    markerView->add_marker_bottomBar = [&](double pos) {
        controlBar->add_marker(pos);
        };

    markerView->clear_markers_buttomBar = [&]() {
        controlBar->clear_markers();
        };

    controlBar->add_marker_in_markerView = [&](double pos) {
        double currentPosition = pos;
        int minutes = (int)currentPosition / 60;
        int seconds = (int)currentPosition % 60;
        juce::String timeText = juce::String::formatted("%02d:%02d", minutes, seconds);

        // Assuming Marker is the correct class name
        juce::String title = "Marker " + juce::String(Marker::get_Marker_cnt());
        markerView->add_markers_list_entry(title, timeText, Marker::get_Marker_cnt(), currentPosition);
        };

    controlBar->add_loaded_markers = [&]() {
        markerView->add_loaded_markers();
        };

    // Initial setup
    startTimerHz(30);

    // --- Load Session Logic FIX: Enclose in a scope to prevent C2362 errors ---
    {
        juce::File session_file("data.json");
        juce::var session;
        if (session_file.existsAsFile()) {
            juce::String json_data = session_file.loadFileAsString();
            session = juce::JSON::parse(json_data);
        }
        else goto skip_loading; // Use goto for clean exit from loading logic

        if (session.isVoid()) goto skip_loading;

        if (
            session["playlist"].isVoid() ||
            session["last_played_index"].isVoid() ||
            session["timestamp"].isVoid())
            goto skip_loading;

        juce::Array<juce::var>* paths_from_json_file = session["playlist"].getArray();
        if (paths_from_json_file == nullptr || paths_from_json_file->size() == 0) goto skip_loading; // Added size check

        // Error C2362 fix: these variables are now scoped and initialized within this block
        std::string last_played_path = session["last_audio_path"].toString().toStdString();

        for (auto& path : *paths_from_json_file) {
            juce::File f = juce::File(path.toString());
            if (audio_player.load(f)) playlistView->add_playlist_entry(f);
        }
        int last_played_index = (int)session["last_played_index"];

        // Error C2362 fix: this variable is now scoped and initialized within this block
        juce::File file = juce::File((juce::String)last_played_path);

        if (file.existsAsFile()) {
            playlistView->onAudioSelected(last_played_index);
            audio_player.setPosition((double)session["timestamp"]);
            audio_player.play_pause();
        }

        juce::Array<juce::var>* markers_from_json_file = session["markers"].getArray();
        if (markers_from_json_file != nullptr) {
            for (auto& p : *markers_from_json_file) {
                markerView->add_marker_pos((double)p);
            }
        }
    } // End of session loading scope

skip_loading:

    // --- Initial Visibility/Setup ---
    // Start listening to the parent component for minimized/maximized state
    if (auto* topLevel = getTopLevelComponent())
    {
        topLevel->addComponentListener(this);
    }

    // Set initial visibility of all views to false (will be set correctly in setView)
    normalView->setVisible(false);
    playlistView->setVisible(false);
    editorView->setVisible(false);
    miniView->setVisible(false);
    mixerView->setVisible(false);
    markerView->setVisible(false);

    setView(View::Normal); // Set initial view and visibility
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();

    // Remove listener from top level component
    if (auto* topLevel = getTopLevelComponent())
    {
        topLevel->removeComponentListener(this);
    }

    // --- Save Session Logic ---
    juce::DynamicObject* session = new juce::DynamicObject();

    int current_audio_playing = audio_player.getIndex();

    if (current_audio_playing != -1) // Use != -1 instead of ~ (bitwise NOT)
    {
        // FIX: Reverting to the logic that calls get_playlist_path, 
        // assuming it exists in PlaylistViewComp.
        session->setProperty("last_audio_path", juce::String(playlistView->get_playlist_path(current_audio_playing)));
    }
    session->setProperty("last_played_index", current_audio_playing);
    session->setProperty("timestamp", audio_player.getPosition());

    juce::Array<juce::var> arr;
    for (int i = 0; i < audio_player.getAudioCount(); i++) {
        // FIX: Reverting to the logic that calls get_playlist_path
        auto& path = playlistView->get_playlist_path(i);
        arr.add(juce::String(path));
    }
    session->setProperty("playlist", arr);

    arr.clear();
    // Assuming Marker is the correct class name
    for (int i = 0; i < Marker::get_Marker_cnt() - 1; i++) {
        arr.add(markerView->get_marker_pos(i));
    }
    session->setProperty("markers", arr);

    juce::var session_json(session);
    juce::String jsonOutput = juce::JSON::toString(session_json, false);
    juce::File sessionFile("data.json");
    sessionFile.replaceWithText(jsonOutput);
}

// === Member Functions ===

double PlayerGUI::getMixPercentage() {
    // New functionality from finalApp branch
    return mixerView->getPercentage();
}

void PlayerGUI::handleMinimisedStateChange(bool isMinimised) {
    // New functionality from finalApp branch
    if (isMinimised)
    {
        setView(View::Minimized);
    }
    else
    {
        setView(View::Normal);
    }
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void PlayerGUI::resized()
{
    auto bounds = getLocalBounds();

    // Define region sizes (using float multiplication for cleaner code)
    int topBarHeight = static_cast<int>(0.05 * getWidth());
    int leftBarWidth = static_cast<int>(0.1 * getWidth());
    int bottomBarHeight = static_cast<int>(0.05 * getWidth());

    // Place the fixed components
    navBar->setBounds(bounds.removeFromLeft(leftBarWidth));
    if (this->currentView != View::Mixer) {
        topBar->setBounds(bounds.removeFromTop(topBarHeight));
        controlBar->setBounds(bounds.removeFromBottom(bottomBarHeight));
    }

    // --- Volume Slider Positioning ---
    int sliderWidth = 30;
    int sliderHeight = 500;

    // Bounds now holds the remaining rectangle for the main view
    int xPos = bounds.getRight() - 40;
    int yPos = bounds.getCentreY() - (sliderHeight / 2);

    volume_slider->setBounds(xPos, yPos, sliderWidth, sliderHeight);

    // --- View-dependent Resizing ---
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
    case View::Minimized:
        miniView->setBounds(getLocalBounds()); // Minimized usually takes the whole window
        break;
    case View::Mixer:
        mixerView->setBounds(bounds);
        break;
    default:
        // Handle unexpected view state
        break;
    }
}

void PlayerGUI::setView(View newView)
{
    // --- Step 1: Hide the OLD view ---
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
    case View::Minimized:
        miniView->setVisible(false);
        break;
    case View::Mixer:
        mixerView->setVisible(false);
        break;
    }

    auto* mainComp = findParentComponentOfClass<MainComponent>();
    if (mainComp == nullptr)
    {
        // This should not happen in a properly structured JUCE app.
        return;
    }

    // --- Step 2: Stop/Start Mixer Mode if changing view ---
    if (currentView == View::Mixer && newView != View::Mixer) // leaving mixer
    {
        mixer_player_1.stop();
        mixer_player_2.stop();
        mainComp->setAudioMode(MainComponent::AudioMode::Normal);
    }

    // --- Step 3: Set Global Visibility for the NEW view type ---

    // Hide all main components first
    topBar->setVisible(false);
    navBar->setVisible(false);
    controlBar->setVisible(false);
    volume_slider->setVisible(false);

    currentView = newView; // Set the new view state

    if (currentView == View::Minimized)
    {
        miniView->setVisible(true);
        // topBar, navBar, controlBar, etc., remain hidden
    }
    else if (currentView == View::Mixer)
    {
        // Mixer view setup
        navBar->setVisible(true); // Keep nav bar
        mixerView->setVisible(true);
        audio_player.stop();
        mainComp->setAudioMode(MainComponent::AudioMode::Mixer);
    }
    else // Normal, Playlist, Editor, Marker views
    {
        // Standard view setup
        topBar->setVisible(true);
        navBar->setVisible(true);
        controlBar->setVisible(true);
        volume_slider->setVisible(true);

        // Show the specific main view component
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
        default:
            break;
        }
    }

    // --- Step 4: Final Update ---
    resized();
    repaint();
}


void PlayerGUI::timerCallback()
{
    if (audio_player.isWokring())
    {
        // Update control bar (position/time)
        controlBar->update();

        // Update normal view (waveform, track info)
        normalView->update(audio_player.getPlaylistFile(audio_player.getIndex()));
    }

    // Update the mixer view if active
    if (currentView == View::Mixer && mixerView)
    {
        mixerView->updateGains();
    }
}