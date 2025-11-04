#include "../PlayerAudio.h"
#include "TopBarComp.h"


TopBarComp::TopBarComp(PlayerAudio& audio_player) : audio_player(audio_player) {

    addAndMakeVisible(loadButton);
    loadButton.addListener(this);

}


void TopBarComp::buttonClicked(juce::Button* button) {

    if (button == &loadButton)
    {
        // Removed redundant juce::FileChooser chooser definition

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();

                // First check: did the user cancel or is the file invalid?
                if (!file.existsAsFile()) return;

                // Second check: is the file a duplicate?
                if (audio_player.isFileAlreadyLoaded(file))
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,       // Icon type
                        "File Already Loaded",                // Window title
                        "The file '" + file.getFileName()     // Message
                        + "' is already in the playlist.",
                        "OK"                                  // Button text
                    );
                }
                else // If the file exists and is not a duplicate (using the cleaner 'else')
                {
                    audio_player.load(file);
                    onFileLoaded(file);
                }
            });
    }
}


void TopBarComp::resized() {
    loadButton.setBounds(getLocalBounds().reduced(10));
}