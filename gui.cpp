#pragma once
#include "gui.h"

PlayerGUI::PlayerGUI(PlayerAudio& audio_player) : player(audio_player) {
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton, &muteButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Add sliders
    std::vector<juce::Slider*> sliders = {
        &volumeSlider,
        &speedSlider
    };
    for (auto *slider : sliders) {
        slider->setRange(0.0, 1.0, 0.01);
        slider->setValue(0.5);
        slider->addListener(this);
        addAndMakeVisible(slider);
    }

}
PlayerGUI::~PlayerGUI() {

}

void PlayerGUI::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}
void PlayerGUI::resized() {
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    muteButton.setBounds(20, y+50, 80, 40);
    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
}
void PlayerGUI::buttonClicked(juce::Button* button) {
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
                if (file.existsAsFile()) {
                    player.load(file);
                }
            });
    }

    if (button == &restartButton)
    {
        player.start();
    }

    if (button == &stopButton)
    {
        player.stop();
    }
}
void PlayerGUI::sliderValueChanged(juce::Slider* slider) {
    if (slider == &volumeSlider) player.setGain((float)slider->getValue());
    //else if (slider == &speedSlider) 
}