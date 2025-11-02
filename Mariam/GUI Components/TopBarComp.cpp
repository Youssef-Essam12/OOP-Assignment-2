#pragma once
#include "../PlayerAudio.h"
#include "TopBarComp.h"


TopBarComp::TopBarComp(PlayerAudio& audio_player) : audio_player(audio_player) {
	
	addAndMakeVisible(loadButton);
	loadButton.addListener(this);

}



void TopBarComp::buttonClicked(juce::Button* button) {

    if (button == &loadButton)
    {

        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (audio_player.isFileAlreadyLoaded(file))
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,     // Icon type
                        "File Already Loaded",              // Window title
                        "The file '" + file.getFileName()   // Message
                        + "' is already in the playlist.",
                        "OK"                                // Button text
                    );
                }
                else
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
