#pragma once
#include <JuceHeader.h>
#include "MixerViewComp.h"

MixerViewComp::MixerViewComp(PlayerAudio& audio_player_1, PlayerAudio& audio_player_2) : 
    audio_player_1(audio_player_1), audio_player_2(audio_player_2) 
{

    audio_1_slider.addListener(this);
    audio_2_slider.addListener(this);
    position_1_slider.addListener(this);
    position_2_slider.addListener(this);
    percentage_slider.addListener(this);
    audio_player_1_load_button.addListener(this);
    audio_player_2_load_button.addListener(this);
    audio_player_1_play_button.addListener(this);
    audio_player_2_play_button.addListener(this);

    audio_1_slider.setSliderStyle(juce::Slider::LinearVertical);
    audio_1_slider.setRange(0.0, 1.0, 0.01);
    audio_1_slider.setValue(1.0); 

    audio_2_slider.setSliderStyle(juce::Slider::LinearVertical);
    audio_2_slider.setRange(0.0, 1.0, 0.01);
    audio_2_slider.setValue(1.0); 



    position_1_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    position_2_slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    percentage_slider.setSliderStyle(juce::Slider::LinearHorizontal);
    percentage_slider.setRange(0.0, 1.0, 0.01); // 0.0 = p1, 1.0 = p2
    percentage_slider.setValue(0.5); 

    addAndMakeVisible(audio_1_slider);
    addAndMakeVisible(audio_2_slider);
    addAndMakeVisible(position_1_slider);
    addAndMakeVisible(position_2_slider);
    addAndMakeVisible(percentage_slider);
    addAndMakeVisible(audio_player_1_load_button);
    addAndMakeVisible(audio_player_2_load_button);
    addAndMakeVisible(audio_player_1_play_button);
    addAndMakeVisible(audio_player_2_play_button);

}


MixerViewComp::~MixerViewComp() {

}

void MixerViewComp::resized() {
    auto bounds = getLocalBounds();

    auto mixerBounds = bounds.removeFromBottom(100).reduced(10); // 10px padding

    auto p1bounds = bounds.removeFromLeft(bounds.getWidth() / 2).reduced(10);
    auto p2bounds = bounds.reduced(10); // The rest of the top area

    audio_player_1_load_button.setBounds(p1bounds.removeFromTop(30));
    position_1_slider.setBounds(p1bounds.removeFromTop(20));
    audio_player_1_play_button.setBounds(p1bounds.removeFromTop(40).reduced(30, 0)); // Added Play button

    audio_player_2_load_button.setBounds(p2bounds.removeFromTop(30));
    position_2_slider.setBounds(p2bounds.removeFromTop(20));
    audio_player_2_play_button.setBounds(p2bounds.removeFromTop(40).reduced(30, 0)); // Added Play button

    audio_1_slider.setBounds(mixerBounds.removeFromLeft(80));
    audio_2_slider.setBounds(mixerBounds.removeFromRight(80));

    percentage_slider.setBounds(mixerBounds.reduced(10, 0));
}

void MixerViewComp::updateGains() {
    double vol1 = double(audio_1_slider.getValue());
    double vol2 = double(audio_2_slider.getValue());
    double percent = double(percentage_slider.getValue());

    double angle = percent * juce::MathConstants<double>::halfPi; // 90 deg

    double crossfade1 = cos(angle);
    double crossfade2 = sin(angle);

    audio_player_1.setGain(vol1 * crossfade1);
    audio_player_2.setGain(vol2 * crossfade2);
}

void MixerViewComp::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff111c1c));
}

void MixerViewComp::load(bool player) {

    PlayerAudio& audio_player = player ? audio_player_2 : audio_player_1;

    juce::FileChooser chooser("Select audio files...",
        juce::File{},
        "*.wav;*.mp3");

    fileChooser = std::make_unique<juce::FileChooser>(
        "Select an audio file...",
        juce::File{},
        "*.wav;*.mp3");

    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [&](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file.existsAsFile())
            {
                while (audio_player.getAudioCount() > 0)
                {
                    audio_player.delete_button(0);
                }
                
                audio_player.remove_source(); 
                audio_player.load(file);
                audio_player.playFile(0);
                audio_player.stop();

                if (player == 0) // Deck A
                {
                    position_1_slider.setRange(0.0, audio_player.getOriginalLength(), juce::dontSendNotification);
                }
                else // Deck B
                {
                    position_2_slider.setRange(0.0, audio_player.getOriginalLength(), juce::dontSendNotification);
                }
            }
        });
}

void MixerViewComp::buttonClicked(juce::Button* button) {
    if (button == &audio_player_1_load_button) {
        this->load(0);
    }
    if (button == &audio_player_2_load_button) {
        this->load(1);
    }
    if (button == &audio_player_1_play_button) {
        audio_player_1.play_pause();
    }
    if (button == &audio_player_2_play_button) {
        audio_player_2.play_pause();
    }
}

void MixerViewComp::sliderValueChanged(juce::Slider* slider) {
    if (slider == &audio_1_slider) {
        updateGains();
    }
    if (slider == &audio_2_slider) {
        updateGains();
    }
    if (slider == &position_1_slider) {
        if (slider->isMouseButtonDown())
        {
            audio_player_1.setPosition(slider->getValue());
        }
    }
    if (slider == &position_2_slider) {
        if (slider->isMouseButtonDown())
        {
            audio_player_2.setPosition(slider->getValue());
        }
    }
    if (slider == &percentage_slider) {
        updateGains();
    }
}

void MixerViewComp::setPercentage(double new_percentage) {
    this->audio_player_1_percentage = new_percentage;
}

double MixerViewComp::getPercentage() {
    return this->audio_player_1_percentage;
}