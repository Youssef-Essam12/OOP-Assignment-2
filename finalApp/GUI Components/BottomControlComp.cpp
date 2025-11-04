#include <JuceHeader.h>
#include "BottomControlComp.h"
#include <algorithm>
#include <random>
#include <numeric> // Added headers for shuffle functionality
#include "BinaryData.h"

bool BottomControlComp::markers_visible = 0;

BottomControlComp::BottomControlComp(PlayerAudio& audio_player) : audio_player(audio_player) {

    // Position Slider setup
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

    // Segment Slider setup
    segmentSlider.setRange(0.0, 1.0, 1.0);
    segmentSlider.addListener(this);
    segmentSlider.setSliderStyle(juce::Slider::TwoValueHorizontal); //the two arrows esmail wanted
    segmentSlider.setMinAndMaxValues(0.0, audio_player.getLength());
    segmentSlider.setVisible(false); // hidden 
    segmentSlider.setTextBoxStyle(juce::Slider::TextBoxRight,
        false, // read-only
        60,    // same width
        segmentSlider.getTextBoxHeight());

    segmentSlider.textFromValueFunction = [&](double value) {
        int minutes = static_cast<int>(value) / 60;
        int seconds = static_cast<int>(value) - (minutes * 60);

        std::string seconds_string = std::to_string(seconds);
        if (seconds < 10) seconds_string = "0" + seconds_string;

        std::string minutes_string = std::to_string(minutes);
        if (minutes < 10) minutes_string = "0" + minutes_string;

        return minutes_string + ":" + seconds_string;
        };

    segmentSlider.setNumDecimalPlacesToDisplay(2);
    segmentSlider.setTextValueSuffix(" s");

    addAndMakeVisible(segmentSlider);
    segmentSlider.setVisible(0);

    addAndMakeVisible(forwardButton);
    addAndMakeVisible(backwardButton);

    addAndMakeVisible(muteToggle);
    addAndMakeVisible(repeatToggle);
    addAndMakeVisible(shuffleToggle);
    addAndMakeVisible(abSegmentToggle);
    addAndMakeVisible(playPauseButton);
    addAndMakeVisible(markerToggle);

    forwardButton.addListener(this);
    backwardButton.addListener(this);

    muteToggle.addListener(this);
    repeatToggle.addListener(this);
    shuffleToggle.addListener(this);
    abSegmentToggle.addListener(this);
    playPauseButton.addListener(this);
    markerToggle.addListener(this);

    playPauseButton.setLookAndFeel(&pp_customlook);

    positionSlider.textFromValueFunction = [&](double value) {
        std::string result = "";

        int minutes = static_cast<int>(value) / 60;        
        int seconds = static_cast<int>(value) - (minutes * 60);

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
    bool visible = markerToggle.getToggleState();

    //std::string marker_text = "Marker ";
    //marker_text += std::to_string(Marker::get_Marker_cnt() - 1);

    //Marker* m = new Marker(positionSlider.getX() + trackBounds.getX() - 4, positionSlider.getY() + 10, markerOffset, pos, audio_player.getLength());
    //markers.emplace_back(m);
    //addAndMakeVisible(m);
    //m->setVisible(visible);
    //m->onClick = [this](double p) {
    //    double ratio = p / audio_player.getOriginalLength();
    //    double new_pos = ratio * audio_player.getLength();
    //    audio_player.setPosition(new_pos);
    //};
    const auto* data = BinaryData::marker_png;
    const int dataSize = BinaryData::marker_pngSize;
    auto marker_icon = juce::ImageFileFormat::loadFrom(data, dataSize);

    juce::ImageButton* marker_button = new juce::ImageButton;
    marker_button->setImages(false, true, true,
        marker_icon, 1.0f, juce::Colours::transparentWhite,
        marker_icon, 1.0f, juce::Colours::transparentWhite,
        marker_icon, 1.0f, juce::Colours::transparentWhite);
    //label->setText(marker_text, juce::dontSendNotification);
    //marker_button->setJustificationType(juce::Justification::centred);
    /*label->setFont(juce::Font(10.0f, juce::Font::bold));
    label->setColour(juce::Label::textColourId, juce::Colours::white);
    label->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);*/

    int labelWidth = 50;
    int labelHeight = 15;
    int markerX = positionSlider.getX() + trackBounds.getX() - 4 + markerOffset;
    int markerY = positionSlider.getY() + 10;
    marker_button->setBounds(markerX - (labelWidth / 2) + 5, markerY - labelHeight - 2, labelWidth, labelHeight);
    addAndMakeVisible(marker_button);
    marker_button->addListener(this);
    marker_button->setVisible(visible);
    markersImageButtons.emplace_back(marker_button);
    marker_pos.push_back(pos);
    resized();
}

void BottomControlComp::clear_markers() {
    //for (auto* m : markers) {
    //    removeChildComponent(m);
    //    delete m;
    //}
    //markers.clear();

    for (auto* btn : markersImageButtons) {
        removeChildComponent(btn);
        delete btn;
    }
    markersImageButtons.clear();
}

void BottomControlComp::delete_marker(int index)
{
    removeChildComponent(markersImageButtons[index]);
    delete markersImageButtons[index];
    markersImageButtons.erase(markersImageButtons.begin() + index);

    marker_pos.erase(marker_pos.begin() + index);
    resized();
    repaint();
}

bool BottomControlComp::getMarkersVisible()
{
    return markers_visible;
}

void BottomControlComp::setMarkersVisible(bool visible)
{
    markers_visible = visible;
}


void BottomControlComp::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.3f));
}

void BottomControlComp::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    segmentSlider.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(10);

    positionSlider.setBounds(bounds.removeFromTop(30));
    bounds.removeFromTop(10);
    auto controlsRow = bounds;
    int rowHeight = controlsRow.getHeight();

    int buttonSize = 50;
    int sideButtonWidth = 60;

    playPauseButton.setBounds(
        (controlsRow.getWidth() - buttonSize) / 2,
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
    int markerToggleWidth = 100;

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

    markerToggle.setBounds(
        shuffleToggle.getX() - 5 - markerToggleWidth, // 10px spacing
        controlsRow.getY(),
        markerToggleWidth,
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
        if (shuffleOn) {
            generateShuffleOrder();
        }
    }
    else if (button == &abSegmentToggle)
    {
        bool toggleState = abSegmentToggle.getToggleState();
        segmentSlider.setVisible(toggleState);

        if (toggleState)
        {
            double len = audio_player.getLength();
            segmentSlider.setRange(0.0, len, 1.0);
            segmentSlider.setMinAndMaxValues(0.0, len, juce::dontSendNotification);
        }
        else
        {
            audio_player.clearSegmentLoop();
            segmentA = segmentB = -1;
            segmentASet = false;
        }

        resized();
        repaint();
    }

    else if (button == &markerToggle) {
        if (!added_markers) {
            add_loaded_markers();
            added_markers = 1;
        }
        bool visible = markerToggle.getToggleState();
        BottomControlComp::setMarkersVisible(visible);
        for (int i = 0; i < (int)markersImageButtons.size(); i++) {
            markersImageButtons[i]->setVisible(visible);
        }
    }
    else { // markerImageButtons
        for (int i = 0; i < (int)markersImageButtons.size(); i++) {
            if (button == markersImageButtons[i]) {
                double ratio = marker_pos[i] / audio_player.getOriginalLength();
                double new_pos = ratio * audio_player.getLength();
                if (audio_player.isSegmentActive()) {
                    auto segment_bounds = audio_player.getSegmentBounds();
                    if (!(new_pos < segment_bounds.first || new_pos > segment_bounds.second)) audio_player.setPosition(new_pos);
                }
                else audio_player.setPosition(new_pos);
                break;
            }
        }
    }
}


// Shuffle Helper Methods (from finalApp)
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

    // Get the shuffle-order index of the current song
    int current_shuffle_index = inverse_shuffleOrder[song_index];

    // Calculate the next shuffle-order index, wrapping around
    int next_shuffle_index = (current_shuffle_index + 1) % playlist_size;

    // Return the playlist index corresponding to the next shuffle-order index
    return shuffleOrder[next_shuffle_index];
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
                // FIX: Correct calculation to map from original length (slider value) 
                // to current length (player position) (from finalApp)
                double ratio = current_value / audio_player.getOriginalLength();
                double new_position = ratio * audio_player.getLength();

                audio_player.setPosition(new_position);
            }

        }
    }
    else if (slider == &segmentSlider)
    {
        double start = segmentSlider.getMinValue();
        double end = segmentSlider.getMaxValue();

        if (end > start)
        {
            segmentA = start;
            segmentB = end;
            audio_player.enableSegmentLoop(segmentA, segmentB);
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
            // FIX: Correct calculation for position slider value (from finalApp)
            double current_pos = audio_player.getPosition();
            double current_len = audio_player.getLength();

            double ratio = current_pos / current_len;
            double result = ratio * truelengthInSeconds;

            positionSlider.setRange(0.0, truelengthInSeconds, juce::dontSendNotification);
            positionSlider.setValue(result, juce::dontSendNotification);

            // Shuffle Autoplay Logic (from finalApp)
            if (shuffleOn && ratio >= .999) {

                if (shuffleOrder.size() != audio_player.getPlaylistSize()) generateShuffleOrder();

                audio_player.playFile(get_next_song_index(audio_player.getIndex()));
            }
        }
        else {
            positionSlider.setValue(0.0, juce::dontSendNotification);
        }
    }
    if (segmentBarVisible)
    {
        segmentSlider.setRange(0.0, truelengthInSeconds, juce::dontSendNotification);
    }
    repaint();
}