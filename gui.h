#pragma once
#include <JuceHeader.h>
#include "audio_player.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener {
public:
    PlayerGUI();
    ~PlayerGUI();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void paint(juce::Graphics& g);
    void resized();
    void buttonClicked(juce::Button* button);
    void sliderValueChanged(juce::Slider* slider);

private:

    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::Slider volumeSlider;
    std::unique_ptr<juce::FileChooser> fileChooser;

    PlayerAudio player;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI);
};