#pragma once
#include <JuceHeader.h>
#include "BottomControlComp.h"


BottomControlComp::BottomControlComp(PlayerAudio& audio_player) : audio_player(audio_player){
	

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

void BottomControlComp::add_marker(double pos)
{
    if (audio_player.getIndex() == -1) return;
    auto& lf = positionSlider.getLookAndFeel();
    auto layout = lf.getSliderLayout(positionSlider);
    auto trackBounds = layout.sliderBounds;

    double markerOffset = (pos / audio_player.getOriginalLength()) * trackBounds.getWidth();
    std::string marker_text = "Marker ";
    marker_text += std::to_string(MarkerEntry::get_marker_cnt());
    Marker *m = new Marker(positionSlider.getX() + trackBounds.getX() - 4, positionSlider.getY() + 10, markerOffset, pos, audio_player.getLength());
    markers.emplace_back(m);
    addAndMakeVisible(m);
    m->onClick = [this, m](double p, double length) {
        double ratio = p / audio_player.getOriginalLength();
        double new_pos = ratio * audio_player.getLength();
        audio_player.setPosition(new_pos);
    };
    juce::Label* label = new juce::Label();
    label->setText(marker_text, juce::dontSendNotification);
    label->setJustificationType(juce::Justification::centred);
    label->setFont(juce::Font(10.0f, juce::Font::bold));
    label->setColour(juce::Label::textColourId, juce::Colours::white);
    label->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);

    int labelWidth = 50;
    int labelHeight = 15;
    int markerX = positionSlider.getX() + trackBounds.getX() - 4 + markerOffset;
    int markerY = positionSlider.getY() + 10;
    label->setBounds(markerX - (labelWidth / 2) + 5, markerY - labelHeight - 2, labelWidth, labelHeight);
    addAndMakeVisible(label);
    resized();
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
        // TODO: Implement shuffle
    }
    else if (button == &abSegmentToggle)
    {
        // TODO: Implement A-B segment
    }

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