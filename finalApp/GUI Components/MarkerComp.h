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
    void add_markers_list_entry(juce::String title, juce::String time, int i, float marker_time);
    void delete_marker(int index);


private:
    PlayerAudio& audio_player;

    juce::Component* marker_list_component;
    juce::Viewport markerlistViewport;
    juce::TextButton addMarkerButton;

    std::vector<MarkerEntry*> markerList_entries;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerComp);

};