#pragma once
#include "gui.h"

PlayerGUI::PlayerGUI(PlayerAudio& audio_player) :
    player(audio_player),
        thumbnailCache(5),
        thumbnail(512, formatManager, thumbnailCache) {

    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
    startTimerHz(30);

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
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);

    speedSlider.setRange(0.1, 5.0, 0.01);
    speedSlider.setValue(1.0);

    for (auto* slider : sliders) {
        slider->addListener(this);
        addAndMakeVisible(*slider);
    }
}
PlayerGUI::~PlayerGUI() {
    stopTimer();
}

void PlayerGUI::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);

    auto waveform = juce::Rectangle<int>(20, 200, getWidth()-40, 300);

    g.setColour(juce::Colours::lightgrey);
    g.drawRect(waveform);

    if (thumbnail.getNumChannels() == 0) {
        g.setColour(juce::Colours::white);
        g.drawFittedText("No Audio Loaded", waveform, juce::Justification::centred, 1);
    }
    else {
        g.setColour(juce::Colours::skyblue);
        thumbnail.drawChannels(g, waveform, 0.0, thumbnail.getTotalLength(), 1.0f);

        double pos = player.getPosition();
        double len = player.getLength();

        if (len > 0) {
            float current_pos = (float)(waveform.getX() + (pos/len) * waveform.getWidth());
            g.setColour(juce::Colours::red);
            g.drawLine(current_pos, (float)waveform.getY(), current_pos, (float)waveform.getBottom(), 2.0f);
        }
    }
}
void PlayerGUI::resized() {
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    muteButton.setBounds(340, y, 80, 40);
    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/

    volumeSlider.setBounds(20, y+40, getWidth() - 40, 30);
    speedSlider.setBounds(20, y+80, getWidth() - 40, 30);
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
                    thumbnail.setSource(new juce::FileInputSource(file));
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
        if (button == &muteButton) {
            player.mute();
        }
}
void PlayerGUI::sliderValueChanged(juce::Slider* slider) {
    if (slider == &volumeSlider) player.setGain((float)slider->getValue());
    else if (slider == &speedSlider) {
        player.change_playback_speed((float)slider->getValue());
    }
}

void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &thumbnail) {
        repaint();
    }
}

void PlayerGUI::timerCallback()
{
    repaint();
}
