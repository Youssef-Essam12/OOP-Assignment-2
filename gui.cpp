#pragma once
#include "gui.h"

PlayerGUI::PlayerGUI() {

    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

}
PlayerGUI::~PlayerGUI() {

}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    player.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    player.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources() {
    player.releaseResources();
}

void PlayerGUI::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}
void PlayerGUI::resized() {
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
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
    if (slider == &volumeSlider)
        player.setGain((float)slider->getValue());
}