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

void PlayerAudio::setIndex(int index) {
    this->currently_loaded_audioFile_index = index;
}

int PlayerAudio::getIndex() const {
    return currently_loaded_audioFile_index;
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
    if (transportSource.isPlaying()) {
        transportSource.stop();
    }
    else {
        transportSource.start();
    }
}


void PlayerAudio::setGain(float gain) {
    if (!this->is_muted) this->current_gain = gain;
    transportSource.setGain(gain);
}

bool displayAllMetadata(juce::AudioFormatReader* reader)
{
    if (reader == nullptr)
        return false;

    // 1. Get a reference to the metadata StringPairArray
    const juce::StringPairArray& metadata = reader->metadataValues;

    // 2. Get a list of all keys found in the file
    juce::StringArray keys = metadata.getAllKeys();

    if (keys.isEmpty())
    {
        juce::Logger::writeToLog("No metadata found in the file.");
        return false;
    }

    juce::Logger::writeToLog("--- Metadata for File ---");

    // 3. Iterate through all found keys
    for (int i = 0; i < keys.size(); ++i)
    {
        const juce::String& key = keys[i];

        const juce::String value = metadata.getValue(key, juce::String());

        // Use DBG or Logger::writeToLog to display the result
        juce::Logger::writeToLog(key + ": " + value);

        // If you wanted to ONLY display non-empty values, you would use:
        // if (value.isNotEmpty()) { juce::Logger::writeToLog(key + ": " + value); }
    }
    juce::Logger::writeToLog("-------------------------");

    return true;
}

bool PlayerAudio::load(const juce::File& file) {
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            audioFiles.push_back(file);
            audioFileMetadata.emplace_back(MetaDataWraper());
            auto& [title, artist] = audioFileMetadata.back();

            const auto& metadata = reader->metadataValues;
            title = metadata.getValue("TITLE", file.getFileNameWithoutExtension());
            artist = metadata.getValue("TPE1", metadata.getValue("ARTIST", "Unknown Artist"));

            audioReaders.push_back(std::unique_ptr<juce::AudioFormatReader>(reader));

            displayAllMetadata(reader);

;            //this->original_audio_length_in_seconds.emplace_back(this->getLength());
            this->original_audio_length_in_seconds.emplace_back(reader->lengthInSamples / reader->sampleRate);
            return true;
        }
    }
    return false;
}

bool PlayerAudio::playFile(int index) {

    if (index >= 0 && index < audioReaders.size()) {

        juce::AudioFormatReader* reader = audioReaders[index].get();

        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset(); 

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, false);

        this->sample_rate = reader->sampleRate;
        this->max_file_channels = (int)reader->numChannels;

        transportSource.setSource(readerSource.get(),
            0,
            nullptr,
            reader->sampleRate); 

        currently_loaded_audioFile_index = index;
        transportSource.start();

		displayAllMetadata(reader);
        return true;
    }
    return false;
}

void PlayerAudio::delete_button(int index)
{
    // remove from audioFiles, audioFileMetadata, audioReaders in player
    if (index <= currently_loaded_audioFile_index) currently_loaded_audioFile_index = -1;
    audioFiles.erase(audioFiles.begin() + index);
    audioFileMetadata.erase(audioFileMetadata.begin() + index);
    audioReaders.erase(audioReaders.begin() + index);
    original_audio_length_in_seconds.erase(original_audio_length_in_seconds.begin() + index);
}

void PlayerAudio::remove_source()
{
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();
}

double PlayerAudio::getPosition() const {
	return transportSource.getCurrentPosition();
}
double PlayerAudio::getLength() const {
	return transportSource.getLengthInSeconds();
}

juce::String PlayerAudio::getTitle() const {
    return audioFileMetadata[currently_loaded_audioFile_index].title;
}
juce::String PlayerAudio::getArtist() const {
    return audioFileMetadata[currently_loaded_audioFile_index].artist;
}

double PlayerAudio::getOriginalLength() const {
    if(this->isWokring() && ~currently_loaded_audioFile_index)
        return this->original_audio_length_in_seconds[currently_loaded_audioFile_index];
    return 1.0;
}

void PlayerAudio::mute() {
    is_muted = !is_muted;
    if (this->is_muted) this->setGain(0);
    else this->setGain(current_gain);
}

void PlayerAudio::setPosition(double pos) {
    
    pos = std::min(pos, this->getLength());
    pos = std::max(0.0, pos);

    transportSource.setPosition(pos);
}

void PlayerAudio::move_by(double displacement) {
    
    double org_position = this->getPosition() / this->getLength() * this->getOriginalLength();

    org_position += displacement;
    org_position = std::min(org_position, this->getLength());
    org_position = std::max(0.0, org_position);
	
    double new_position = org_position / this->getOriginalLength() * this->getLength();

    this->transportSource.setPosition(new_position);
}

void PlayerAudio::setSpeed(float speed) {
    bool isplaying = transportSource.isPlaying();
    float new_sample_rate = this->sample_rate * speed;

    double position_ratio = this->transportSource.getCurrentPosition() / this->getLength();
    
    this->transportSource.setSource(readerSource.get(),
        0,
        nullptr,
        new_sample_rate,
        max_file_channels);
    this->transportSource.setPosition(position_ratio * this->getLength());

    if (isplaying) this->transportSource.start();
}


void PlayerAudio::loop() {
    is_looping = !is_looping;
}

const juce::File& PlayerAudio::getPlaylistFile(int index) const {
    return this->audioFiles[index];

}

bool PlayerAudio::isWokring() const {
    return currently_loaded_audioFile_index != -1;
}

bool PlayerAudio::isFileAlreadyLoaded(const juce::File& file) {
    for (const auto& f : audioFiles) {
        if (f == file) {
            return true;
        }
    }
	return false;
}