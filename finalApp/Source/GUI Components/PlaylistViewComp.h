#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"
#include "NormalViewComp.h"
#include "PlaylistEntry.h"

#include <functional>

class PlaylistViewComp : public juce::Component, juce::Button::Listener {
public:
    PlaylistViewComp(PlayerAudio &player);
    ~PlaylistViewComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void update(juce::File& file);

    void display_playlist_menu();
    void add_playlist_entry(const juce::File& file);
    void delete_button(int index);

    void buttonClicked(juce::Button* button);

    // talk to normal, player
    std::function<void(int index)> onAudioSelected;

    std::string get_playlist_path(int index);
private:
    PlayerAudio& audio_player;
    std::vector<juce::TextButton*> playlist_buttons;
    std::vector<juce::TextButton*> playlist_delete_buttons;
    std::vector<PlaylistEntry*> playlist_entries;
    std::vector<std::string> playlist_paths;

    juce::Component* playlist_component;
    juce::Viewport playlistViewport;

    bool playlist_componenet_visible = true;

    juce::Component* playlistHeader;
    juce::Label* titleLabel;
    juce::Label* authorLabel;

    void create_header();
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistViewComp);

};