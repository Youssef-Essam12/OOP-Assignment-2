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

    // GUI
    void paint(juce::Graphics& g);
    void resized();

private:
    PlayerGUI gui;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent);
};
