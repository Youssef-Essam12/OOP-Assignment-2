#pragma once
#include <JuceHeader.h>
#include "MinimizedViewComp.h"


MinimizedViewComp::MinimizedViewComp(PlayerAudio& player) : audio_player(player)
{

    titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);

    artistLabel.setFont(juce::Font(14.0f));
    artistLabel.setJustificationType(juce::Justification::centred);

    positionSlider.setSliderStyle(juce::Slider::LinearBar);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    playPauseButton.addListener(this);
    playPauseButton.setClickingTogglesState(true);
    playPauseButton.setLookAndFeel(&pp_customlook);

    for (auto* comp : { &titleLabel, &artistLabel })
    {
        addAndMakeVisible(comp);
    }

    addAndMakeVisible(&positionSlider);
    addAndMakeVisible(&playPauseButton);

}

MinimizedViewComp::~MinimizedViewComp()
{
}

void MinimizedViewComp::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &positionSlider)
    {
        float current_value = (float)slider->getValue();
        if (slider->isMouseButtonDown())
        {
            auto event = juce::Desktop::getInstance().getMainMouseSource();
            if (event.getCurrentModifiers().isLeftButtonDown())
            {
                double new_position = current_value / audio_player.getOriginalLength() * audio_player.getLength();
                audio_player.setPosition(new_position);
            }

        }
    }
}

void MinimizedViewComp::updateControlState() {
    if (audio_player.getIndex() == -1) {
        positionSlider.setRange(0.0, 0.0, juce::dontSendNotification);
        positionSlider.setValue(0.0, juce::dontSendNotification);
        return;
    }
    double truelengthInSeconds = audio_player.getOriginalLength();

    if (!positionSlider.isMouseButtonDown())
    {
        if (truelengthInSeconds > 0)
        {
            double ratio = truelengthInSeconds / audio_player.getLength();

            positionSlider.setRange(0.0, truelengthInSeconds, juce::dontSendNotification);
            positionSlider.setValue(ratio * audio_player.getPosition(), juce::dontSendNotification);
        }
        else {
            positionSlider.setValue(0.0, juce::dontSendNotification);
        }
    }
    repaint();
}

void MinimizedViewComp::resized() {
    
    auto bounds = getLocalBounds().reduced(5); 

    int labelHeight = 20;
    int sliderHeight = 15;
    int buttonSize = 40;

    titleLabel.setBounds(bounds.removeFromTop(labelHeight));
    artistLabel.setBounds(bounds.removeFromTop(labelHeight));
    
    positionSlider.setBounds(bounds.removeFromTop(sliderHeight).reduced(0, 2));

    bounds.removeFromTop(5);
    playPauseButton.setBounds(
        bounds.getCentreX() - (buttonSize / 2),
        bounds.getY(), // Starts at the top of the remaining bounds
        buttonSize,
        buttonSize // Fixed size
    );
}

void MinimizedViewComp::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey.darker(0.3f));
}




void MinimizedViewComp::buttonClicked(juce::Button* button)
{
    if (button == &playPauseButton)
    {
        audio_player.play_pause();
    }
}

void MinimizedViewComp::updateTrackInfo()
{
    titleLabel.setText((audio_player.isWokring() ? audio_player.getTitle() : ""), juce::dontSendNotification);
    artistLabel.setText((audio_player.isWokring() ? audio_player.getArtist() : ""), juce::dontSendNotification);
}