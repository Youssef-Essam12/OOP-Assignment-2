#pragma once
#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(gui);
    setSize(500, 250);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    gui.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    gui.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    gui.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    gui.paint(g);
}

void MainComponent::resized()
{
    gui.setBounds(20, 20, getWidth() - 40, 120);
}


