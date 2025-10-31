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
    void add_marker(double pos, bool f);
    void display_markers_menu();
    void add_markers_list_entry(std::string text);
    void delete_marker(int index);

private:
    //-------Global variables-------
    PlayerAudio& player;
    std::vector<juce::TextButton*> playlist_buttons;
    std::vector<juce::TextButton*> playlist_delete_buttons;
    std::vector<juce::TextButton*> marker_buttons;
    std::vector<juce::TextButton*> marker_delete_buttons;
    std::vector<std::string> playlist_paths;
    std::vector<marker*> markers;
    std::vector<juce::Label*> markers_labels;
    std::vector<double> markers_pos;

    juce::Component* playlist_component, *marker_list_component;
    juce::Viewport playlistViewport, markerlistViewport;

    bool playlist_componenet_visible = true, markers_list_visible = false;



    // -------Buttons-------------
    juce::TextButton loadButton{ "Load Audio" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton playPauseButton{ "Play/Pause" };
    juce::TextButton toEnd{ "To End" };
    juce::TextButton toStart{ "To Start" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
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
    juce::Slider low_eq, mid_eq, high_eq;
    juce::Slider reverbSlider;
    //----------Labels-------------
    juce::Label volumeLabel;
    juce::Label speedLabel;
    juce::Label titleLabel;
    juce::Label artistLabel;
    juce::Label lowLabel, midLabel, highLabel, reverbLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    //------------Sizes-----------
    int panelWidth;
    int panelHeight;

    bool markers_visible = 0;
    bool markers_added = 0;

    float low_eq_value, mid_eq_value, high_eq_value;

    int aIndex = -1;        
    int bIndex = -1;        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI);
};