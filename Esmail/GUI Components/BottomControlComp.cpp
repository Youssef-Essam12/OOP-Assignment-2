#pragma once
#include <algorithm>
#include <JuceHeader.h>
#include "BottomControlComp.h"
#include <random>
#include <numeric>


BottomControlComp::BottomControlComp(PlayerAudio& audio_player) : audio_player(audio_player) {
	

    for (auto* slider : { &positionSlider }) {
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

	addAndMakeVisible(forwardButton);
	addAndMakeVisible(backwardButton);

	addAndMakeVisible(muteToggle);
	addAndMakeVisible(repeatToggle);
	addAndMakeVisible(shuffleToggle);
	addAndMakeVisible(abSegmentToggle);
	addAndMakeVisible(playPauseButton);
	//addAndMakeVisible(positionSlider);


	forwardButton.addListener(this);
	backwardButton.addListener(this);

	muteToggle.addListener(this);
	repeatToggle.addListener(this);
	shuffleToggle.addListener(this);
	abSegmentToggle.addListener(this);
	playPauseButton.addListener(this);
	//positionSlider.addListener(this);


	playPauseButton.setLookAndFeel(&pp_customlook);
	//playPauseButton.setColour(juce::Button::id, juce::Colours::steelblue);

    positionSlider.textFromValueFunction = [&](double value) {
        std::string result = "";

        int minutes = static_cast<int>(value) / 60;
        int seconds = value - (minutes * 60);

        std::string seconds_string = std::to_string(seconds);
        if (seconds < 10) seconds_string = "0" + seconds_string;

        std::string minutes_string = std::to_string(minutes);
        if (minutes < 10) minutes_string = "0" + minutes_string;

        return minutes_string + ":" + seconds_string;
    };

    positionSlider.setNumDecimalPlacesToDisplay(2);
    positionSlider.setTextValueSuffix(" s");
}



void BottomControlComp::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::black.withAlpha(0.3f));
}

void BottomControlComp::resized()
{
    auto bounds = getLocalBounds().reduced(10); 

    positionSlider.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(10); 
    auto controlsRow = bounds;
    int rowHeight = controlsRow.getHeight();

    int buttonSize = 50;
    int sideButtonWidth = 60;

    playPauseButton.setBounds(
        (controlsRow.getWidth() - buttonSize)/2,
        controlsRow.getY(),
        buttonSize,
        rowHeight
    );

    backwardButton.setBounds(
        playPauseButton.getX() - 10 - sideButtonWidth, // 10px spacing
        controlsRow.getY(),
        sideButtonWidth,
        rowHeight
    );

    forwardButton.setBounds(
        playPauseButton.getX() + playPauseButton.getWidth() + 10, // 10px spacing
        controlsRow.getY(),
        sideButtonWidth,
        rowHeight
    );

    int toggleWidth = 70; 
    muteToggle.setBounds(
        controlsRow.getX(), 
        controlsRow.getY(),
        toggleWidth,
        rowHeight
    );

    repeatToggle.setBounds(
        muteToggle.getRight() + 5, 
        controlsRow.getY(),
        toggleWidth,
        rowHeight
    );

    int loopToggleWidth = 100; 
    int shuffleToggleWidth = 80;

    abSegmentToggle.setBounds(
        controlsRow.getRight() - loopToggleWidth, // Align to far right
        controlsRow.getY(),
        loopToggleWidth,
        rowHeight
    );

    shuffleToggle.setBounds(
        abSegmentToggle.getX() - 5 - shuffleToggleWidth, // 5px spacing
        controlsRow.getY(),
        shuffleToggleWidth,
        rowHeight
    );
}

void BottomControlComp::buttonClicked(juce::Button* button)
{
    if (button == &playPauseButton)
    {
        audio_player.play_pause();
    }
    else if (button == &forwardButton)
    {
        audio_player.move_by(10);
    }
    else if (button == &backwardButton)
    {
        audio_player.move_by(-10);
    }
    else if (button == &muteToggle)
    {
        audio_player.mute();
    }
    else if (button == &repeatToggle)
    {
        audio_player.loop();
    }
    else if (button == &shuffleToggle)
    {
        shuffleOn ^= 1;
    }
    else if (button == &abSegmentToggle)
    {
        // TODO: Implement A-B segment
    }

}

void BottomControlComp::generateShuffleOrder() {
    
    shuffleOrder.clear();
    shuffleOrder.shrink_to_fit();

    inverse_shuffleOrder.clear();
    inverse_shuffleOrder.shrink_to_fit();

    int playlist_size = audio_player.getPlaylistSize();

    shuffleOrder.resize(playlist_size);
    inverse_shuffleOrder.resize(playlist_size);

    std::iota(shuffleOrder.begin(), shuffleOrder.end(), 0);
    
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(shuffleOrder.begin(), shuffleOrder.end(), g);

    for (int i = 0; i < playlist_size; i++) {
        inverse_shuffleOrder[shuffleOrder[i]] = i;
    }
}

int BottomControlComp::get_next_song_index(int song_index) {
    
    int playlist_size = audio_player.getPlaylistSize();
    
    if (playlist_size == 0) return -1;

    int next_song_index = inverse_shuffleOrder[song_index];

    return shuffleOrder[(next_song_index + 1) % playlist_size];
}


void BottomControlComp::sliderValueChanged(juce::Slider* slider)
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

void BottomControlComp::update() {
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
            double ratio = audio_player.getPosition() / audio_player.getLength();
            double result = ratio * truelengthInSeconds;
            positionSlider.setRange(0.0, truelengthInSeconds, juce::dontSendNotification);
            positionSlider.setValue(result, juce::dontSendNotification);

            if (shuffleOn && ratio >= .999) {
                
                if (shuffleOrder.size() != audio_player.getPlaylistSize()) generateShuffleOrder();

                audio_player.playFile(get_next_song_index(audio_player.getIndex()));
            }
        }
        else {
            positionSlider.setValue(0.0, juce::dontSendNotification);
        }
    }
    repaint();
}