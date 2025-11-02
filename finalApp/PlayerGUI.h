#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"



class BottomControlComp;
class LeftNavComp;
class TopBarComp;
class NormalViewComp;
class PlaylistViewComp;
class EditorComp;
class MarkerComp;


class PlayerGUI : public juce::Component,
    public juce::Timer
{
public:
    
    PlayerGUI(PlayerAudio& audio_player);
    ~PlayerGUI();

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    enum class View { Normal, Playlist, Editor, Marker, Mixer };
private:
    void setView(View newView);
    
    PlayerAudio& audio_player;

    View viewArr[5] = { View::Normal, View::Playlist, View::Editor, View::Marker, View::Mixer};
    // Bar components
    std::unique_ptr<TopBarComp> topBar;
    std::unique_ptr<LeftNavComp> navBar;
    std::unique_ptr<BottomControlComp> controlBar;

    // View components
    std::unique_ptr<NormalViewComp> normalView;
    std::unique_ptr<PlaylistViewComp> playlistView;
    std::unique_ptr<EditorComp> editorView;
    std::unique_ptr<MarkerComp> markerView;
    View currentView = View::Normal;

    bool markers_added = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI);
};