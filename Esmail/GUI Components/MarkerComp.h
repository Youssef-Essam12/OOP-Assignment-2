#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"
#include "NormalViewComp.h"
#include "../Extra Functionalities/Marker.h"
#include "MarkerEntry.h"

#include <functional>

class MarkerComp : public juce::Component, juce::Button::Listener {
public:
    MarkerComp(PlayerAudio& player);
    ~MarkerComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button);

    void display_markers_menu();
    void add_markers_list_entry(juce::String title, juce::String time, int i, double marker_time);
    void delete_marker(int index);

    // only used when loading the session
    void add_marker_pos(double p);
    double get_marker_pos(int i);
    void add_loaded_markers();

    void clear_markers();

    std::function<void(double)> add_marker_bottomBar;
    std::function<void()> clear_markers_buttomBar;
private:
    PlayerAudio& audio_player;

    juce::Component* marker_list_component;
    juce::Viewport markerlistViewport;
    juce::TextButton addMarkerButton;
    juce::TextButton clearMarkersButton;

    std::vector<MarkerEntry*> markerList_entries;

    // only used when loading the session
    std::vector<double> marker_pos;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerComp);

};