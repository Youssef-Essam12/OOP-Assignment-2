#pragma once
#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audio_player) : player(audio_player) {
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton, &muteButton, &playPauseButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Add sliders
    std::vector<juce::Slider*> sliders = {
        &volumeSlider,
        &speedSlider,
        &positionSlider
    };
    for (auto *slider : sliders) {
        slider->setRange(0.0, 100, 1);
        slider->setValue(20);
        slider->addListener(this);
        addAndMakeVisible(slider);
        slider->setTextBoxStyle(juce::Slider::TextBoxRight,
            false, // read-only
            60,    // Box width
            slider->getTextBoxHeight());

        slider->setTextValueSuffix(" %");
    }

    speedSlider.setRange(30, 200, 1);
    speedSlider.setValue(100);

    positionSlider.setNumDecimalPlacesToDisplay(2);
    positionSlider.setTextValueSuffix(" s");

    startTimerHz(30);
    // Add labels
    std::vector<juce::Label*> labels = {
        &volumeLabel,
        &speedLabel
    };
    for (auto* label : labels) {
        addAndMakeVisible(label);
    }
	volumeLabel.setText("Volume", juce::dontSendNotification);
    speedLabel.setText("Speed", juce::dontSendNotification);

}
PlayerGUI::~PlayerGUI() {

}

void PlayerGUI::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
}
void PlayerGUI::resized() {
 
    int buttons_w = 100;
    int buttons_h = 50;

    int button_margin = buttons_w >> 1;
    int buttons_x = (getWidth() - buttons_w) >> 1;
    int buttons_y = getHeight() - 1.5 * button_margin;

    int distance_factor = 3;

    playPauseButton.setBounds(buttons_x                                      , buttons_y, buttons_w, buttons_h);
    loadButton.setBounds     (buttons_x - 1 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    restartButton.setBounds  (buttons_x + 1 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    stopButton.setBounds     (buttons_x - 2 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    muteButton.setBounds     (buttons_x + 2 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
	
    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/

    int slider_w = getWidth() * 0.75;
	int slider_h = 30;  
    int slider_x = (getWidth() - slider_w) >> 1;
	int slider_y = playPauseButton.getY() - button_margin - slider_h;

    volumeSlider.setBounds  (slider_x, slider_y               , slider_w, slider_h);
    speedSlider.setBounds   (slider_x, slider_y - 1 * slider_h, slider_w, slider_h);
    positionSlider.setBounds(slider_x, slider_y - 3 * slider_h, slider_w, slider_h);

    int label_w = 70;
    int label_h = 50;
    int label_margin = 20;
    volumeLabel.setBounds(volumeSlider.getX(), volumeSlider.getY(), label_w, label_h);
    speedLabel.setBounds(speedSlider.getX(), speedSlider.getY(), label_w, label_h);
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

    else if (button == &restartButton)
    {
        player.start();
    }

    else if (button == &stopButton)
    {
        player.stop();
    }
    else if (button == &muteButton) {
        player.mute();
    }
    else if(button == &playPauseButton)
    {
        player.play_pause();
	}

}

void PlayerGUI::sliderValueChanged(juce::Slider* slider) {

    float current_value = (float)slider->getValue();

    if (slider == &volumeSlider) {
        player.setGain(current_value / 100);
    }
    else if (slider == &speedSlider) {
        player.setSpeed(current_value / 100);
    }
    else if (slider == &positionSlider)
    {
        if (slider->isMouseButtonDown())
        {
            player.setPosition(current_value);
        }
    }

}

void PlayerGUI::timerCallback()
{
    double lengthInSeconds = player.getLength();

    if (lengthInSeconds > 0)
    {
        double currentPosition = player.getPosition();

        positionSlider.setRange(0.0, lengthInSeconds, juce::dontSendNotification);

        positionSlider.setValue(currentPosition, juce::dontSendNotification);
    }
}