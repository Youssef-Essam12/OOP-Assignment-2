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

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};