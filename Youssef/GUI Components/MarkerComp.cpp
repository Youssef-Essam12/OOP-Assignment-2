#pragma once
#include <JuceHeader.h>
#include "MarkerComp.h"

int MarkerEntry::cnt = 1;

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
    MarkerEntry* newEntry = new MarkerEntry(title, timeText, i, marker_time);
    newEntry->onClick = [this](int i) {
        double ratio = markerList_entries[i]->get_pos() / audio_player.getOriginalLength();
        double new_pos = ratio * audio_player.getLength();
        audio_player.setPosition(new_pos);
    };
    newEntry->onDeleteClick = [this](int i) {
        delete_marker(i);
    };
    marker_list_component->addAndMakeVisible(newEntry);

    markerList_entries.emplace_back(newEntry);
    add_marker_bottomBar(marker_time);
    resized();
}

void MarkerComp::delete_marker(int index)
{
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
        double currentPosition = marker_pos[i];
        int minutes = (int)currentPosition / 60;
        int seconds = (int)currentPosition % 60;
        juce::String timeText = juce::String::formatted("%02d:%02d", minutes, seconds);
        juce::String title = "Marker " + juce::String(MarkerEntry::get_marker_cnt());

        add_markers_list_entry(title, timeText, markerList_entries.size(), currentPosition);
    }
    marker_pos.clear();
}

void MarkerComp::clear_markers() {
    for (auto* m : markerList_entries) {
        removeChildComponent(m);
        delete m;
    }
    markerList_entries.clear();
    clear_markers_buttomBar();
    resized();
    repaint();
}

void MarkerComp::resized() {
    const int margin = 10;
    const int buttonHeight = 40;
    const int buttonSpacing = 10; // Use a more descriptive name for the margin between components

    auto bounds = getLocalBounds().reduced(margin);

    // 1. Position the Add Marker button
    // Take the top section for the button
    auto addMarkerButtonArea = bounds.removeFromTop(buttonHeight);
    addMarkerButton.setBounds(addMarkerButtonArea);

    // Remove spacing below the Add Marker button
    bounds.removeFromTop(buttonSpacing);

    // 2. Position the Clear Markers button
    // Take the next section for the button
    auto clearMarkersButtonArea = bounds.removeFromTop(buttonHeight);
    clearMarkersButton.setBounds(clearMarkersButtonArea);

    // Remove spacing below the Clear Markers button
    bounds.removeFromTop(buttonSpacing);

    // 3. Set viewport bounds for the remaining area
    markerlistViewport.setBounds(bounds);
    markerlistViewport.setScrollBarsShown(true, false);

    // 4. Calculate content size for marker list
    int contentWidth = bounds.getWidth();
    int requiredHeight = (int)markerList_entries.size() * (buttonHeight + buttonSpacing) + buttonSpacing;
    int contentHeight = std::max(bounds.getHeight(), requiredHeight);
    marker_list_component->setSize(contentWidth, contentHeight);

    // 5. Position the MarkerEntry components inside the viewport's content component
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
        juce::String title = "Marker " + juce::String(MarkerEntry::get_marker_cnt());

        add_markers_list_entry(title, timeText, markerList_entries.size(), originalPosition);
    }
    else if (button == &clearMarkersButton) {
        if (audio_player.getIndex() == -1) return;
        clear_markers();
    }
}