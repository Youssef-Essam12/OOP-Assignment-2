#pragma once
#include "MainComponent.h"

MainComponent::MainComponent() : player(), gui(player) {
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
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    player.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    player.releaseResources();
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


