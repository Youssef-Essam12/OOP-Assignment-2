#pragma once
#include <JuceHeader.h>
#include "PlaylistViewComp.h"

PlaylistViewComp::PlaylistViewComp(PlayerAudio& player) : audio_player(player) {
    playlist_component = new juce::Component;
    playlistViewport.setViewedComponent(playlist_component, true);
    playlistViewport.setAlwaysOnTop(true);
    playlistViewport.setScrollBarThickness(10);
    addAndMakeVisible(playlistViewport);

    display_playlist_menu();
}


PlaylistViewComp::~PlaylistViewComp() {

}

void PlaylistViewComp::display_playlist_menu() {

    juce::ComponentAnimator& animator = juce::Desktop::getInstance().getAnimator();


    juce::Rectangle<int> targetBounds;

    int panelWidth = 50, panelHeight = 50;
    if (playlist_componenet_visible)
    {
        // Target is ON-screen
        targetBounds = juce::Rectangle<int>(getWidth() - panelWidth, 0, panelWidth, panelHeight);
    }
    else
    {
        // Target is OFF-screen (to the right)
        targetBounds = juce::Rectangle<int>(getWidth(), 0, panelWidth, panelHeight);
    }

    // Animate the component to the target bounds over 300ms
    animator.animateComponent(
        &playlistViewport,  // Component to animate
        targetBounds,         // Target position and size
        1.0f,                 // Target opacity (1.0 = fully visible)
        300,                  // Time in milliseconds
        false,                // Use an ease-out curve
        0.0,                  // Start velocity
        0.0                   // End velocity
    );
}

void PlaylistViewComp::add_playlist_entry(const juce::File& file) {
    juce::String fileName = file.getFileNameWithoutExtension();

    auto* newEntry = new juce::TextButton(fileName);
    newEntry->setButtonText(fileName);
    newEntry->addListener(this);

    auto* deleteButton = new juce::TextButton();
    deleteButton->setButtonText("X");
    deleteButton->addListener(this);

    playlist_component->addAndMakeVisible(newEntry);
    playlist_component->addAndMakeVisible(deleteButton);

    playlist_buttons.push_back(newEntry);
    playlist_delete_buttons.push_back(deleteButton);

    playlist_paths.push_back(file.getFullPathName().toStdString());

    resized();
}

void PlaylistViewComp::delete_button(int index)
{
    // remove from playlist_buttons, playlist_delete_buttons, playlist_paths
    if (index == audio_player.getIndex()) {
        audio_player.remove_source();
        //clear_markers();
        audio_player.setIndex(-1);
    }
    if (index < audio_player.getIndex()) {
        audio_player.setIndex(audio_player.getIndex() - 1);
    }
    playlist_component->removeChildComponent(playlist_buttons[index]);
    playlist_component->removeChildComponent(playlist_delete_buttons[index]);
    delete playlist_buttons[index];
    delete playlist_delete_buttons[index];
    playlist_buttons.erase(playlist_buttons.begin() + index);
    playlist_delete_buttons.erase(playlist_delete_buttons.begin() + index);
    playlist_paths.erase(playlist_paths.begin() + index);
}

void PlaylistViewComp::resized() {
    int panelWidth = 0.3 * getWidth();
    int panelHeight = 0.15 * getHeight();

    int buttonMargin = 10;
    int buttonHeight = 30;
    int startY = 10;

    if (playlist_componenet_visible)
    {
        playlistViewport.setBounds(getWidth() - panelWidth, 0, panelWidth, panelHeight);
        playlistViewport.setScrollBarsShown(true, false);
    }

    // Make sure the viewed component size is updated
    int height = std::max(panelHeight, (int)playlist_buttons.size() * (buttonHeight + buttonMargin));
    playlist_component->setSize(panelWidth, height);


    const int deleteButtonWidth = 30;

    const int entryButtonWidth = panelWidth - (3 * buttonMargin) - deleteButtonWidth;
    for (int i = 0; i < playlist_buttons.size(); ++i) {
        playlist_buttons[i]->setBounds(buttonMargin,
            startY + i * (buttonHeight + buttonMargin),
            panelWidth - 2 * buttonMargin,
            buttonHeight);

        int yPos = startY + i * (buttonHeight + buttonMargin);
        playlist_delete_buttons[i]->setBounds(buttonMargin + entryButtonWidth + buttonMargin, // X position: Main Button End + Margin
            yPos,
            deleteButtonWidth,
            buttonHeight);
    }

}

void PlaylistViewComp::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::blue);
}

//
//void PlaylistViewComp::update(juce::File& file)
//{
//
//}


void PlaylistViewComp::buttonClicked(juce::Button* button) {
    for (size_t i = 0; i < playlist_buttons.size(); ++i) {
        if (button == playlist_buttons[i]) {
            if (i == audio_player.getIndex()) break;
            const juce::File& file = audio_player.getPlaylistFile(i);
            if (file.existsAsFile()) {
                onAudioSelected(i);
                //thumbnail.setSource(new juce::FileInputSource(file))
            }
            break;
        }
        else if (button == playlist_delete_buttons[i]) {
            delete_button(i);
            audio_player.delete_button(i);
            resized();
            repaint();
            break;
        }
    }
}
