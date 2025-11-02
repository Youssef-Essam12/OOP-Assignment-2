#pragma once
#include <JuceHeader.h>
#include "EditorComp.h"


EditorComp::EditorComp(PlayerAudio& player) : audio_player(player) {
    std::vector<juce::Label*> labels = {
    &lowLabel,
    &midLabel,
    &highLabel,
    &reverbLabel,
    &speedLabel
    };
    for (auto* label : labels) {
        addAndMakeVisible(label);
    }

    lowLabel.setText("Low", juce::dontSendNotification);
    midLabel.setText("Mid", juce::dontSendNotification);
    highLabel.setText("High", juce::dontSendNotification);
    reverbLabel.setText("Reverb", juce::dontSendNotification);
    speedLabel.setText("Speed", juce::dontSendNotification);

    std::vector<juce::Slider*> sliders = {
    &low_eq,
    &mid_eq,
    &high_eq,
    &reverbSlider,
    &speedSlider
    };


    for (auto* slider : sliders) {
        slider->setRange(0.0, 100, 1);
        slider->setValue(0);
        slider->addListener(this);
        addAndMakeVisible(slider);
        slider->setSliderStyle(juce::Slider::LinearVertical);
        slider->setTextBoxStyle(juce::Slider::TextBoxBelow,
            false, // read-only
            60,    // Box width
            20);   // Box height

        slider->setTextValueSuffix(" %");
    }

    low_eq.setRange(-20.0, 20.0, 0.1);
    low_eq.setValue(0.0);
    low_eq.setTextValueSuffix(" dB");

    mid_eq.setRange(-20.0, 20.0, 0.1);
    mid_eq.setValue(0.0);
    mid_eq.setTextValueSuffix(" dB");

    high_eq.setRange(-20.0, 20.0, 0.1);
    high_eq.setValue(0.0);
    high_eq.setTextValueSuffix(" dB");

    speedSlider.setRange(30, 200, 1);
    speedSlider.setValue(100);
    speedSlider.setTextValueSuffix(" %");

    low_eq_value = 0.0f;
    mid_eq_value = 0.0f;
    high_eq_value = 0.0f;
}
EditorComp::~EditorComp() {

}

void EditorComp::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1c1c1c));
}

void EditorComp::resized() {
    auto area = getLocalBounds();

    // Add margins
    const int margin = 20;
    area.reduce(margin, margin);

    // Calculate width for each slider column
    const int numSliders = 5;
    const int sliderWidth = area.getWidth() / numSliders;
    const int labelHeight = 25;
    const int spacing = 10;

    // Layout each slider with its label below
    auto layoutSliderWithLabel = [&](juce::Slider& slider, juce::Label& label, int index) {
        auto sliderArea = area.removeFromLeft(sliderWidth);

        // Position label at the bottom
        auto labelArea = sliderArea.removeFromBottom(labelHeight);
        label.setBounds(labelArea);
        label.setJustificationType(juce::Justification::centred);

        // Add spacing between slider and label
        sliderArea.removeFromBottom(spacing);

        // Position slider in remaining spaces
        slider.setBounds(sliderArea);
        };

    layoutSliderWithLabel(low_eq, lowLabel, 0);
    layoutSliderWithLabel(mid_eq, midLabel, 1);
    layoutSliderWithLabel(high_eq, highLabel, 2);
    layoutSliderWithLabel(reverbSlider, reverbLabel, 3);
    layoutSliderWithLabel(speedSlider, speedLabel, 4);
}

void EditorComp::sliderValueChanged(juce::Slider* slider)
{

    auto current_value = slider->getValue();
    
    if (slider == &speedSlider) {
            audio_player.setSpeed(current_value / 100);
    }
    else if (slider == &low_eq) {
        float low = low_eq.getValue();
        low_eq_value = low;
        audio_player.updateFilters(low_eq_value, mid_eq_value, high_eq_value);
    }
    else if (slider == &mid_eq) {
        float mid = mid_eq.getValue();
        mid_eq_value = mid;
        audio_player.updateFilters(low_eq_value, mid_eq_value, high_eq_value);
    }
    else if (slider == &high_eq) {
        float high = high_eq.getValue();
        high_eq_value = high;
        audio_player.updateFilters(low_eq_value, mid_eq_value, high_eq_value);
    }
    else if (slider == &reverbSlider) {
        if (!current_value) {
            audio_player.updateReverb(0.0, 0.0, 0.0, 1.0, 0.0);
        }
        else {
            float roomSizeValue = current_value / 100.0f;
            float wetLevel = roomSizeValue * 0.33f;
            audio_player.updateReverb(roomSizeValue, 0.5f, wetLevel, 0.8f, 1.0f);
        }
    }
}

void EditorComp::update(const juce::File& file)
{

}