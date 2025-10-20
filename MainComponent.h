#pragma once
#include <JuceHeader.h>
#include <vector>
#include "gui.h"

class MainComponent : public juce::AudioAppComponent {
public:
    MainComponent();
    ~MainComponent();

    // Audio
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void start();
    void stop();
    void setPosition(double pos);
    void setGain(float gain);
    void play();
    bool load(const juce::File& file);
    double getPosition() const;
    double getLength() const;
    void mute();

    // GUI
    void paint(juce::Graphics& g);
    void resized();

private:
    PlayerAudio player;
    PlayerGUI gui;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent);
};
