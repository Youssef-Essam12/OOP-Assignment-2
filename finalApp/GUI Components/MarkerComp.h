#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"
#include "NormalViewComp.h"
#include "../Extra Functionalities/Marker.h"
#include "MarkerEntry.h"
#include "BottomControlComp.h"

#include <functional>

class MarkerComp : public juce::Component, juce::Button::Listener {
public:
    MarkerComp(PlayerAudio& player);
    ~MarkerComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void update();

    void buttonClicked(juce::Button* button);

    void display_markers_menu();
    void add_markers_list_entry(juce::String title, juce::String time, int i, double marker_time);
    void delete_marker(int index);

    // only used when loading the session
    void add_marker_pos(double p);
    void add_marker_title(juce::String title);
    void add_loaded_markers();

    void clear_markers();

    int get_marker_cnt();
    double get_marker_pos(int i);
    juce::String get_marker_title(int i);
    void set_market_cnt(int cnt);

    std::function<void(double)> add_marker_bottomBar;
    std::function<void(int)> delete_marker_bottomBar;
    std::function<void()> clear_markers_buttomBar;
private:
    PlayerAudio& audio_player;

    juce::Component* marker_list_component;
    juce::Viewport markerlistViewport;
    juce::TextButton addMarkerButton;
    juce::TextButton clearMarkersButton;

    std::vector<MarkerEntry*> markerList_entries;
    // used only when loading the saved session 
    std::vector<double> marker_pos;
    std::vector<juce::String> marker_titles;

    int marker_cnt = 1;

    juce::Component* markerHeader;
    juce::Label* titleHeaderLabel;
    juce::Label* timeHeaderLabel;

    void create_header();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerComp);

};