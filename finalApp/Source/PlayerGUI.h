#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include <functional> // Included from finalApp branch

// Forward declarations
class BottomControlComp;
class LeftNavComp;
class TopBarComp;
class NormalViewComp;
class PlaylistViewComp;
class EditorComp;
class MarkerComp;

// Components added in finalApp branch
class MixerViewComp;
class FloatingVolumeSlider;


// Inherits juce::ComponentListener from finalApp branch
class PlayerGUI : public juce::Component,
    public juce::Timer,
    public juce::ComponentListener
{
public:

    // Updated constructor to include mixer players
    PlayerGUI(PlayerAudio& audio_player, PlayerAudio& mixer_player_1, PlayerAudio& mixer_player_2);
    ~PlayerGUI();

    // New public methods for minimization and mixer
    void handleMinimisedStateChange(bool isMinimised);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    double getMixPercentage();

    // Updated View enum to include Minimized and Mixer
    enum class View { Normal, Playlist, Editor, Marker, Mixer };

private:
    void setView(View newView);

    PlayerAudio& audio_player;

    // Added mixer player references
    PlayerAudio& mixer_player_1;
    PlayerAudio& mixer_player_2;

    // Updated view order
    View viewArr[5] = { View::Normal, View::Playlist, View::Editor, View::Mixer, View::Marker };

    // Bar components
    std::unique_ptr<TopBarComp> topBar;
    std::unique_ptr<LeftNavComp> navBar;
    std::unique_ptr<BottomControlComp> controlBar;

    // View components
    std::unique_ptr<NormalViewComp> normalView;
    std::unique_ptr<PlaylistViewComp> playlistView;
    std::unique_ptr<EditorComp> editorView;
    std::unique_ptr<MarkerComp> markerView;

    // New view components
    std::unique_ptr<MixerViewComp> mixerView;

    // Floating Volume Slider
    std::unique_ptr<FloatingVolumeSlider> volume_slider;

    View currentView = View::Normal;

    bool markers_added = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI);
};