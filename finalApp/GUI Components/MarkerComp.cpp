#pragma once
#include <JuceHeader.h>
#include "MarkerComp.h"

MarkerComp::MarkerComp(PlayerAudio& player) : audio_player(player) {
    marker_list_component = new juce::Component;
    markerlistViewport.setViewedComponent(marker_list_component, true);
    markerlistViewport.setAlwaysOnTop(true);
    markerlistViewport.setScrollBarThickness(10);
    addAndMakeVisible(markerlistViewport);

    addMarkerButton.setButtonText("Add Marker");
    addMarkerButton.addListener(this);
    addAndMakeVisible(addMarkerButton);

    clearMarkersButton.setButtonText("Clear Markers");
    clearMarkersButton.addListener(this);
    addAndMakeVisible(clearMarkersButton);

    display_markers_menu();
}

MarkerComp::~MarkerComp() {
}

void MarkerComp::display_markers_menu() {
    juce::ComponentAnimator& animator = juce::Desktop::getInstance().getAnimator();
    juce::Rectangle<int> targetBounds;
    int panelWidth = 50, panelHeight = 50;
    targetBounds = juce::Rectangle<int>(getWidth() - panelWidth, 0, panelWidth, panelHeight);
    // Animate the component to the target bounds over 300ms
    animator.animateComponent(
        &markerlistViewport,  // Component to animate
        targetBounds,         // Target position and size
        1.0f,                 // Target opacity (1.0 = fully visible)
        300,                  // Time in milliseconds
        false,                // Use an ease-out curve
        0.0,                  // Start velocity
        0.0                   // End velocity
    );
}

void MarkerComp::add_markers_list_entry(juce::String title, juce::String timeText, int i, double marker_time) {
    MarkerEntry* newEntry = new MarkerEntry(title, timeText, i-1, marker_time);
    markerList_entries.emplace_back(newEntry);
    newEntry->onClick = [this](int i) {
        double ratio = markerList_entries[i]->get_pos() / audio_player.getOriginalLength();
        double new_pos = ratio * audio_player.getLength();
        if (audio_player.isSegmentActive()) {
            auto segment_bounds = audio_player.getSegmentBounds();
            if (new_pos < segment_bounds.first || new_pos > segment_bounds.second) return;
        }
        audio_player.setPosition(new_pos);
    };
    newEntry->onDeleteClick = [this](int i) {
        delete_marker(i);
    };
    marker_list_component->addAndMakeVisible(newEntry);

    add_marker_bottomBar(marker_time);
    resized();
}

void MarkerComp::delete_marker(int index)
{
    this->marker_cnt--;
    marker_list_component->removeChildComponent(markerList_entries[index]);
    delete markerList_entries[index];
    markerList_entries.erase(markerList_entries.begin() + index);
    for (int i = 0; i < (int)markerList_entries.size(); i++) markerList_entries[i]->setIndex(i);
    resized();
    repaint();
}

void MarkerComp::add_marker_pos(double p)
{
    marker_pos.push_back(p);
}

double MarkerComp::get_marker_pos(int i)
{
    return markerList_entries[i]->get_pos();
}

void MarkerComp::add_loaded_markers()
{
    for (int i = 0; i < (int)marker_pos.size(); i++) {
        ++marker_cnt;
        double currentPosition = marker_pos[i];
        int minutes = (int)currentPosition / 60;
        int seconds = (int)currentPosition % 60;
        juce::String timeText = juce::String::formatted("%02d:%02d", minutes, seconds);
        juce::String title = "Marker " + juce::String(marker_cnt);

        add_markers_list_entry(title, timeText, marker_cnt, currentPosition);
    }
    marker_pos.clear();
}

void MarkerComp::clear_markers() {
    for (auto* m : markerList_entries) {
        removeChildComponent(m);
        delete m;
    }
    markerList_entries.clear();
    marker_cnt = 0;
    clear_markers_buttomBar();
    resized();
    repaint();
}

int MarkerComp::get_marker_cnt()
{
    return marker_cnt;
}

void MarkerComp::set_market_cnt(int cnt)
{
    marker_cnt = cnt;
}

void MarkerComp::resized() {
    const int margin = 10;
    const int buttonHeight = 40;
    const int buttonSpacing = 10;

    auto bounds = getLocalBounds().reduced(margin);

    auto addMarkerButtonArea = bounds.removeFromTop(buttonHeight);
    addMarkerButton.setBounds(addMarkerButtonArea);

    bounds.removeFromTop(buttonSpacing);

    auto clearMarkersButtonArea = bounds.removeFromTop(buttonHeight);
    clearMarkersButton.setBounds(clearMarkersButtonArea);

    bounds.removeFromTop(buttonSpacing);

    markerlistViewport.setBounds(bounds);
    markerlistViewport.setScrollBarsShown(true, false);

    int contentWidth = bounds.getWidth();
    int requiredHeight = (int)markerList_entries.size() * (buttonHeight + buttonSpacing) + buttonSpacing;
    int contentHeight = std::max(bounds.getHeight(), requiredHeight);
    marker_list_component->setSize(contentWidth, contentHeight);

    const int entryComponentWidth = contentWidth - (2 * margin);
    int currentY = buttonSpacing;
    for (int i = 0; i < markerList_entries.size(); ++i) {
        markerList_entries[i]->setBounds(margin,
            currentY,
            entryComponentWidth,
            buttonHeight);
        currentY += buttonHeight + buttonSpacing;
    }
}

void MarkerComp::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1c1c1c));
}

void MarkerComp::buttonClicked(juce::Button* button) {
    if (button == &addMarkerButton) {
        if (audio_player.getIndex() == -1) return;
        double currentPosition = audio_player.getPosition();
        double originalPosition = (currentPosition / audio_player.getLength()) * audio_player.getOriginalLength();

        int minutes = (int)originalPosition / 60;
        int seconds = (int)originalPosition % 60;
        juce::String timeText = juce::String::formatted("%02d:%02d", minutes, seconds);
        ++marker_cnt;
        juce::String title = "Marker " + juce::String(marker_cnt);

        add_markers_list_entry(title, timeText, marker_cnt, originalPosition);
    }
    else if (button == &clearMarkersButton) {
        if (audio_player.getIndex() == -1) return;
        clear_markers();
    }
}