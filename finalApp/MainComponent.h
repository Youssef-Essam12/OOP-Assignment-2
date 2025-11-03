#pragma once
#include <JuceHeader.h>
#include <vector>
#include "PlayerGUI.h"
#include "PlayerAudio.h"

class MainComponent : public juce::AudioAppComponent {
public:

    // New enum for audio routing mode
    enum class AudioMode { Normal, Mixer };

    MainComponent();
    ~MainComponent() override; // Added override specifier

    // Audio
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void start();
    void stop();
    void setPosition(double pos);
    void setGain(float gain);
    void play_pause();
    bool load(const juce::File& file);
    double getPosition() const;
    double getLength() const;
    void mute();
    void loop();

    // GUI
    void paint(juce::Graphics& g);
    void resized();

    // New public methods for mixer functionality and GUI access
    void setAudioMode(AudioMode newMode);
    PlayerAudio* getPlayer(int index);

    PlayerGUI* getGUI();

private:

    // New private members for mixer functionality
    AudioMode current_audio_mode = AudioMode::Normal;

    PlayerAudio player;

    PlayerAudio mixer_player_1;
    PlayerAudio mixer_player_2;

    PlayerGUI gui;

    juce::AudioBuffer<float> tempBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent);
};