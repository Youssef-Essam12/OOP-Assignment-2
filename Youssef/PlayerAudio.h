#pragma once
#include <JuceHeader.h>

struct MetaDataWraper {
    juce::String title;
    juce::String artist;
	MetaDataWraper(){
        this->title  = "";
        this->artist = "";
    }
};

class PlayerAudio {
public:
    PlayerAudio();
    ~PlayerAudio();


    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    bool load(const juce::File& file);

    void start();
    void stop();
    void play_pause();
    void mute();
    void loop();
    void move_by(double displacement);
    bool playFile(int index);
    void delete_button(int index);
    void remove_source();

    // setter methods
    void setSpeed(float speed);
    void setPosition(double pos);
    void setGain(float gain);
    void setIndex(int index);

    // getter methods
    double getPosition() const;
    double getLength() const;
	double getOriginalLength() const;
    int    getIndex() const;
    bool   isWokring() const;
    
    juce::String getTitle() const;
    juce::String getArtist() const;
    const juce::File& getPlaylistFile(int index) const;

    // Warnings
    bool isFileAlreadyLoaded(const juce::File& file);

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    int currently_loaded_audioFile_index = -1;
    bool is_muted = 0;
    float current_gain = 0;

    int max_file_channels = 0;
    float sample_rate = 0.0;

    bool is_looping = 0;

    std::vector<double> original_audio_length_in_seconds;

    std::vector<juce::File> audioFiles;
    std::vector<MetaDataWraper> audioFileMetadata;
    std::vector<std::unique_ptr<juce::AudioFormatReader>> audioReaders;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};