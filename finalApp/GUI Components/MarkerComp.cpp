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

void MarkerComp::add_markers_list_entry(juce::String title, juce::String timeText, int i, float marker_time) {
    MarkerEntry* newEntry = new MarkerEntry(title, timeText, i, marker_time);
    newEntry->onClick = [&](int i) {
        double ratio = markerList_entries[i]->get_pos() / audio_player.getOriginalLength();
        double new_pos = ratio * audio_player.getLength();
        audio_player.setPosition(new_pos);
    };
    newEntry->onDeleteClick = [&](int i) {
        delete_marker(i);
    };
    marker_list_component->addAndMakeVisible(newEntry);

    markerList_entries.emplace_back(newEntry);
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

void MarkerComp::resized() {
    const int margin = 10;
    const int buttonHeight = 40;
    const int buttonMargin = 10;

    auto bounds = getLocalBounds().reduced(margin);

    // Position the Add Marker button at the top
    auto buttonArea = bounds.removeFromTop(buttonHeight);
    addMarkerButton.setBounds(buttonArea);

    // Add spacing between button and viewport
    bounds.removeFromTop(buttonMargin);

    // Set viewport bounds for the remaining area
    markerlistViewport.setBounds(bounds);
    markerlistViewport.setScrollBarsShown(true, false);

    // Calculate content size for marker list
    int contentWidth = bounds.getWidth();
    int requiredHeight = (int)markerList_entries.size() * (buttonHeight + buttonMargin) + buttonMargin;
    int contentHeight = std::max(bounds.getHeight(), requiredHeight);
    marker_list_component->setSize(contentWidth, contentHeight);

    const int entryComponentWidth = contentWidth - (2 * margin);
    int currentY = margin;
    for (int i = 0; i < markerList_entries.size(); ++i) {
        markerList_entries[i]->setBounds(margin,
            currentY,
            entryComponentWidth,
            buttonHeight);
        currentY += buttonHeight + buttonMargin;
    }
}

void MarkerComp::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1c1c1c));
}

void MarkerComp::buttonClicked(juce::Button* button) {
    if (button == &addMarkerButton) {
        double currentPosition = audio_player.getPosition();
        int minutes = (int)currentPosition / 60;
        int seconds = (int)currentPosition % 60;
        juce::String timeText = juce::String::formatted("%02d:%02d", minutes, seconds);
        juce::String title = "Marker " + juce::String(MarkerEntry::get_marker_cnt());

        add_markers_list_entry(title, timeText, markerList_entries.size(), currentPosition);
    }
}