#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
#include "marker.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::ChangeListener,
    public juce::ComponentListener,
    public juce::Timer {
public:
    PlayerGUI(PlayerAudio& audio_player);
    ~PlayerGUI();

    void paint(juce::Graphics& g);
    void resized();
    void buttonClicked(juce::Button* button);
    void sliderValueChanged(juce::Slider* slider);

    void timerCallback() override;
    void updateTrackInfo();

    void changeListenerCallback(juce::ChangeBroadcaster* source);
    void display_playlist_menu();
    void add_playlist_entry(const juce::File& file);
    void delete_button(int index);
    void display_markers();
    void clear_markers();

    //void wave_form(juce::Graphics& g);
private:
    //-------Global variables-------
    PlayerAudio& player;
    bool playlist_componenet_visible = true;
    std::vector<juce::TextButton*> playlist_buttons;
    std::vector<juce::TextButton*> playlist_delete_buttons;
    std::vector<std::string> playlist_paths;
    std::vector<marker*> markers;
    juce::Component playlist_component;

    //int current_audio_playing = -1;


    // -------Buttons-------------
    juce::TextButton loadButton{ "Load Audio" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton playPauseButton{ "Play/Pause" };
    juce::TextButton toEnd{ "To End" };
    juce::TextButton toStart{ "To Start" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton backward{ "-10s" };
    juce::TextButton forward{ "+10s" };
    juce::TextButton playlist_menu{ "Playlist" };
    juce::TextButton addMarker{ "Add Marker" };
    juce::TextButton displayMarkers{ "Toggle Markers" };
    juce::TextButton clearMarkers{ "Clear Markers" };

    //----------Sliders-------------
    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Slider positionSlider;

    //----------Labels-------------
    juce::Label volumeLabel;
    juce::Label speedLabel;
    juce::Label titleLabel;
    juce::Label artistLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    //------------Sizes-----------
    int panelWidth;
    int panelHeight;

    bool markers_visible = 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI);
};

/*
*   Play list to do list:
    Add button for playlist menu
    On playlist button press -> display menu -> implement display menu function
    On load button press -> open file chooser -> implement add entry to playlist function
    Implement delete button to remove loaded images

*/