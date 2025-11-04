#include <JuceHeader.h>
#include "PlaylistViewComp.h"

// Assuming playlist_componenet_visible is a private bool member of PlaylistViewComp
// from a previous merge, and is used to track the visibility state.

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

    // Define the size of the animated panel
    int panelWidth = 50, panelHeight = 50;

    if (playlist_componenet_visible) // Logic to determine target position (from finalApp)
    {
        // Target is ON-screen (slide in)
        targetBounds = juce::Rectangle<int>(getWidth() - panelWidth, 0, panelWidth, panelHeight);
    }
    else
    {
        // Target is OFF-screen (to the right, slide out)
        targetBounds = juce::Rectangle<int>(getWidth(), 0, panelWidth, panelHeight);
    }

    // Animate the component to the target bounds over 300ms
    animator.animateComponent(
        &playlistViewport,      // Component to animate
        targetBounds,           // Target position and size
        1.0f,                   // Target opacity (1.0 = fully visible)
        300,                    // Time in milliseconds
        false,                  // Use an ease-out curve
        0.0,                    // Start velocity
        0.0                     // End velocity
    );
}

void PlaylistViewComp::add_playlist_entry(const juce::File file) {
    
    int file_index = audio_player.getPlaylistSize()-1;
    

    PlaylistEntry* newEntry = new PlaylistEntry(audio_player.getTitle(file_index), audio_player.getArtist(file_index), audio_player.getAudioCount() - 1);
    newEntry->onClick = [&](int i) {
        this->onAudioSelected(i);
        };

    newEntry->onDeleteClick = [&](int i) {
        delete_button(i);
        };

    playlist_component->addAndMakeVisible(newEntry);
    playlist_entries.emplace_back(newEntry);

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
    if (index >= 0 && index < playlist_entries.size()) {
        playlist_component->removeChildComponent(playlist_entries[index]);
        delete playlist_entries[index];
        playlist_entries.erase(playlist_entries.begin() + index);
        playlist_paths.erase(playlist_paths.begin() + index);
        audio_player.delete_button(index);
    }
    for (int i = 0; i < (int)playlist_entries.size(); i++) playlist_entries[i]->setIndex(i);
    resized();
    repaint();
}

void PlaylistViewComp::resized() {
    const int margin = 10;
    auto parentBounds = getLocalBounds();

    // These local variables are not used for setting bounds of viewport, 
    // but might be part of the initial state logic not shown.
    int panelWidth = parentBounds.getWidth();
    int panelHeight = parentBounds.getHeight();

    auto viewportArea = parentBounds.reduced(margin);
    playlistViewport.setBounds(viewportArea);

    playlistViewport.setScrollBarsShown(true, false);

    int buttonHeight = 40;
    int buttonMargin = 10;

    int contentWidth = viewportArea.getWidth();

    int requiredHeight = (int)playlist_entries.size() * (buttonHeight + buttonMargin) + buttonMargin;
    int contentHeight = std::max(viewportArea.getHeight(), requiredHeight);

    playlist_component->setSize(contentWidth, contentHeight);
    const int entryComponentWidth = contentWidth - (2 * margin);
    int currentY = margin;

    for (int i = 0; i < playlist_entries.size(); ++i) {

        playlist_entries[i]->setBounds(margin,
            currentY,
            entryComponentWidth,
            buttonHeight);

        currentY += buttonHeight + buttonMargin;
    }
}

void PlaylistViewComp::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1c1c1c));
}


void PlaylistViewComp::buttonClicked(juce::Button* button) {
    juce::ignoreUnused(button);
}

std::string PlaylistViewComp::get_playlist_path(int index)
{
    return playlist_paths[index];
}