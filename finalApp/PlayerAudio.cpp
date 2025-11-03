#pragma once
#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() {
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio() {
    releaseResources();
}

int PlayerAudio::getPlaylistSize() const {
    return (int)this->audioFiles.size();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    this->sample_rate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.sampleRate = sampleRate;
    spec.numChannels = 2;

    leftChain.prepare(spec);
    rightChain.prepare(spec);
    updateFilters();

    reverb.prepare(spec);
    updateReverb();

    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::updateReverb(float roomSize, float damping, float wetLevel, float dryLevel, float width) {
    juce::dsp::Reverb::Parameters params;

    params.roomSize = roomSize;    // 0.0 to 1.0 (size of the space)
    params.damping = damping;      // 0.0 to 1.0 (high frequency absorption)
    params.wetLevel = wetLevel;    // 0.0 to 1.0 (reverb volume)
    params.dryLevel = dryLevel;    // 0.0 to 1.0 (original signal volume)
    params.width = width;          // 0.0 to 1.0 (stereo width)
    params.freezeMode = 0.0f;      // 0.0 = normal, 1.0 = infinite reverb

    reverb.setParameters(params);
}

void PlayerAudio::updateFilters(float lowGainDB, float midGainDB, float highGainDB)
{
    float sampleRate = this->sample_rate;

    float lowGain = juce::Decibels::decibelsToGain(lowGainDB);
    float midGain = juce::Decibels::decibelsToGain(midGainDB);
    float highGain = juce::Decibels::decibelsToGain(highGainDB);

    auto lowShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate,
        100.0f,      // cutoff frequency
        0.707f,      // Q factor (resonance)
        lowGain    // gain in decibels
    );

    auto peakCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate,
        1000.0f,     // center frequency
        1.0f,        // Q factor (bandwidth)
        midGain
    );

    auto highShelfCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate,
        8000.0f,
        0.707f,
        highGain
    );

    *leftChain.get<0>().coefficients = *lowShelfCoeffs;
    *leftChain.get<1>().coefficients = *peakCoeffs;
    *leftChain.get<2>().coefficients = *highShelfCoeffs;

    *rightChain.get<0>().coefficients = *lowShelfCoeffs;
    *rightChain.get<1>().coefficients = *peakCoeffs;
    *rightChain.get<2>().coefficients = *highShelfCoeffs;
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    transportSource.getNextAudioBlock(bufferToFill);

    juce::dsp::AudioBlock<float> block(*bufferToFill.buffer, bufferToFill.startSample);
    auto subBlock = block.getSubBlock(0, bufferToFill.numSamples);

    // FIX: Add channel check before attempting to process stereo blocks (from finalApp)
    if (bufferToFill.buffer->getNumChannels() >= 2)
    {
        auto leftBlock = subBlock.getSingleChannelBlock(0);
        auto rightBlock = subBlock.getSingleChannelBlock(1);

        juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
        juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

        leftChain.process(leftContext);
        rightChain.process(rightContext);
    }

    juce::dsp::ProcessContextReplacing<float> reverbContext(subBlock);
    reverb.process(reverbContext);

    if (is_looping && transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds()) {
        transportSource.setPosition(0.0);
        transportSource.start();
    }

    // SEGMENT LOOPING    
    if (segmentLoopActive && loopStart != -1 && loopEnd > loopStart)
    {
        double pos = transportSource.getCurrentPosition();
        if (pos >= loopEnd)
        {
            transportSource.setPosition(loopStart);
        }
    }
}

void PlayerAudio::setIndex(int index) {
    this->currently_loaded_audioFile_index = index;
}

void PlayerAudio::setOriginalIndex(int index)
{
    this->original_loaded_audioFile_index = index;
}

int PlayerAudio::getIndex() const {
    return currently_loaded_audioFile_index;
}

std::pair<double, double> PlayerAudio::getSegmentBounds()
{
    return { loopStart, loopEnd };
}

bool PlayerAudio::isSegmentActive()
{
    return segmentLoopActive;
}

int PlayerAudio::getOriginalIndex() const
{
    return this->original_loaded_audioFile_index;
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
    // Only save the current_gain if we are NOT muted, otherwise the saved value will be 0.
    if (!this->is_muted) this->current_gain = gain;
    transportSource.setGain(gain);
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

            // FIX: Ensure correct double casting of lengthInSamples (from finalApp)
            this->original_audio_length_in_seconds.emplace_back((double)reader->lengthInSamples / reader->sampleRate);
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

        return true;
    }
    return false;
}

void PlayerAudio::delete_button(int index)
{
    // remove from audioFiles, audioFileMetadata, audioReaders in player
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
    // FIX: Add index validity check (from finalApp)
    if (currently_loaded_audioFile_index >= 0 && currently_loaded_audioFile_index < audioFileMetadata.size())
        return audioFileMetadata[currently_loaded_audioFile_index].title;
    return {}; // Return empty String for safety
}
juce::String PlayerAudio::getArtist() const {
    // FIX: Add index validity check (from finalApp)
    if (currently_loaded_audioFile_index >= 0 && currently_loaded_audioFile_index < audioFileMetadata.size())
        return audioFileMetadata[currently_loaded_audioFile_index].artist;
    return {}; // Return empty String for safety
}

double PlayerAudio::getOriginalLength() const {
    // FIX: Corrected index check and bounds checking (from finalApp)
    if (isWokring() && currently_loaded_audioFile_index >= 0 && currently_loaded_audioFile_index < original_audio_length_in_seconds.size())
        return this->original_audio_length_in_seconds[currently_loaded_audioFile_index];
    return 1.0;
}

void PlayerAudio::mute() {
    is_muted = !is_muted;
    if (this->is_muted)
    {
        // Save the current gain before muting (from finalApp)
        this->current_gain = transportSource.getGain();
        this->setGain(0.0f);
    }
    else
    {
        // Restore the saved gain (from finalApp)
        this->setGain(current_gain);
    }
}

void PlayerAudio::setPosition(double pos) {

    pos = std::min(pos, this->getLength());
    pos = std::max(0.0, pos);

    transportSource.setPosition(pos);
}

void PlayerAudio::move_by(double displacement) {

    // FIX: Using simplified logic (from finalApp) to move the position directly 
    // based on the current transport source state.
    double current_position = this->getPosition();
    double new_position = current_position + displacement;

    new_position = std::min(new_position, this->getLength());
    new_position = std::max(0.0, new_position);

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

void PlayerAudio::enableSegmentLoop(double start, double end)
{
    loopStart = start;
    loopEnd = end;
    if (this->getPosition() < loopStart) this->setPosition(loopStart);
    segmentLoopActive = (start != -1 && end > start);
}

void PlayerAudio::clearSegmentLoop()
{
    segmentLoopActive = false;
    loopStart = loopEnd = -1;
}


const juce::File& PlayerAudio::getPlaylistFile(int index) const {
    return this->audioFiles[index];

}

bool PlayerAudio::isWokring() const {
    return currently_loaded_audioFile_index != -1;
}

int PlayerAudio::getAudioCount() const
{
    return this->audioFiles.size();
}

bool PlayerAudio::isFileAlreadyLoaded(const juce::File& file) {
    for (const auto& f : audioFiles) {
        if (f == file) {
            return true;
        }
    }
    return false;
}

void PlayerAudio::set_last_played_audio_path(juce::String path)
{
    last_played_audio_path = path;
}

double PlayerAudio::getGain() const {
    // New function merged from finalApp
    return transportSource.getGain();
}