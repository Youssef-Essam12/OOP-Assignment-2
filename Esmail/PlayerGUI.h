#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener, 
    public juce::ChangeListener,
    public juce::Timer {
public:
    PlayerGUI(PlayerAudio &audio_player);
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

    //void wave_form(juce::Graphics& g);
private:
    //-------Global variables-------
    PlayerAudio& player;
    bool playlist_componenet_visible = false;
	std::vector<juce::TextButton*> playlist_buttons;
	juce::Component playlist_component;
    
        
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI);
};

/*
*   Play list to do list:
    Add button for playlist menu
    On playlist button press -> display menu -> implement display menu function
	On load button press -> open file chooser -> implement add entry to playlist function
    Implement delet button to remove loaded images

*/