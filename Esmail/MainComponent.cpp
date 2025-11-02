#pragma once
#include "MainComponent.h"

MainComponent::MainComponent() : player(), gui(player, mixer_player_1, mixer_player_2) {
    addAndMakeVisible(gui);
    setSize(500, 1080);
    setAudioChannels(0, 2);

}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixer_player_1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixer_player_2.prepareToPlay(samplesPerBlockExpected, sampleRate);


    // prepare for mixing
    tempBuffer.setSize(2, samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    if (current_audio_mode == AudioMode::Normal)
    {
        player.getNextAudioBlock(bufferToFill);
    }
    else if (current_audio_mode == AudioMode::Mixer)
    {
        mixer_player_1.getNextAudioBlock(bufferToFill);

        tempBuffer.clear();
        juce::AudioSourceChannelInfo tempChannelInfo(&tempBuffer, 0, bufferToFill.numSamples);

        mixer_player_2.getNextAudioBlock(tempChannelInfo);

        // Add the temp buffer (player 2) to the main buffer (player 1)
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            bufferToFill.buffer->addFrom(channel, bufferToFill.startSample, // Destination
                tempBuffer,                        // Source
                channel,                           // Source Channel
                0,                                 // Source Start Sample
                bufferToFill.numSamples);          // Num Samples
        }
    }
}

void MainComponent::releaseResources()
{
    player.releaseResources();
    mixer_player_1.releaseResources();
    mixer_player_2.releaseResources();
}

void MainComponent::setAudioMode(AudioMode newMode)
{
    current_audio_mode = newMode;
}

PlayerAudio* MainComponent::getPlayer(int index)
{
    if (index == 1) return &mixer_player_1;
    if (index == 2) return &mixer_player_2;
    return &player; // Default to main player
}

void MainComponent::start() {
    player.start();
}

void MainComponent::stop() {
    player.stop();
}
void MainComponent::setPosition(double pos) {
    player.setPosition(pos);
}
void MainComponent::setGain(float gain) {
    player.setGain(gain);
}

bool MainComponent::load(const juce::File& file) {
    return player.load(file);
}
double MainComponent::getPosition() const {
    return player.getPosition();
}
double MainComponent::getLength() const {
    return player.getLength();
}
void MainComponent::mute() {
    player.mute();
}

void MainComponent::play_pause() {
    player.play_pause();
}

void MainComponent::paint(juce::Graphics& g)
{
    gui.paint(g);
}

void MainComponent::resized()
{
    gui.setBounds(0, 0, getWidth(), getHeight());
}

void MainComponent::loop() {
    player.loop();
}

PlayerGUI* MainComponent::getGUI() {
    return &this->gui;
}