#pragma once
#include <JuceHeader.h>

class PlayerAudio {
public:
    PlayerAudio();
    ~PlayerAudio();


    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void start();
    void stop();
    void setPosition(double pos);
    void setGain(float gain);
    void play();
    bool load(const juce::File& file);
    double getPosition() const;
    double getLength() const;
    void mute();
    void change_playback_speed(float speed);

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    bool is_muted = 0;
    float current_gain = 0;

    int max_file_channels;
    float sample_rate = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};