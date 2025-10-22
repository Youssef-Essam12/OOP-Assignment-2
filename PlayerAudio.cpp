#pragma once
#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() {
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio() {
    releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    transportSource.getNextAudioBlock(bufferToFill);
    if (is_looping && transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds()) {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

void PlayerAudio::releaseResources() {
	transportSource.releaseResources();
}

void PlayerAudio::start() {
	transportSource.start();
}

void PlayerAudio::stop() {
	transportSource.stop();
	transportSource.setPosition(0.0);
}

void PlayerAudio::play_pause() {
    if (!isPaused) {
        transportSource.stop();
    }
    else {
        transportSource.start();
    }
    isPaused = !isPaused;
}

void PlayerAudio::setPosition(double pos) {
	transportSource.setPosition(pos);
}

void PlayerAudio::setGain(float gain) {
    if (!this->is_muted) this->current_gain = gain;
    transportSource.setGain(gain);
}

bool PlayerAudio::load(const juce::File& file) {


    if (file.existsAsFile())
    {
        currentTitle = "";
        currentArtist = "";

        if (auto* reader = formatManager.createReaderFor(file))
        {
            // 🔑 Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            this->sample_rate = reader->sampleRate;
            this->max_file_channels = (int)reader->numChannels;

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
            
            const auto& metadata = reader->metadataValues;

            currentTitle = metadata.getValue("TITLE", file.getFileNameWithoutExtension());
            currentArtist = metadata.getValue("ARTIST", "Unknown Artist");

            transportSource.start();
            return true;
        }
    }
    return false;
}

double PlayerAudio::getPosition() const {
	return transportSource.getCurrentPosition();
}
double PlayerAudio::getLength() const {
	return transportSource.getLengthInSeconds();
}

void PlayerAudio::mute() {
    is_muted = !is_muted;
    if (this->is_muted) this->setGain(0);
    else this->setGain(current_gain);
}

void PlayerAudio::forward_backward(bool forward) {
    double new_position = this->getPosition();
    if (forward) {
        new_position = std::min(new_position + 10, this->getLength());
    }
    else {
        new_position = std::max(0.0, new_position - 10);
    }
    this->transportSource.setPosition(new_position);
}

void PlayerAudio::setSpeed(float speed) {
    float new_sample_rate = this->sample_rate * speed;
    double current_position = this->transportSource.getCurrentPosition();
    //bool was_playing = this->transportSource.isPlaying();
    this->transportSource.setSource(readerSource.get(),
        0,
        nullptr,
        new_sample_rate,
        max_file_channels);
    this->transportSource.setPosition(current_position);
    //if (was_playing) this->transportSource.start();
}

juce::String PlayerAudio::getTitle() const {
    return currentTitle;
}
juce::String PlayerAudio::getArtist() const {
    return currentArtist;
}

void PlayerAudio::loop() {
    is_looping = !is_looping;
}