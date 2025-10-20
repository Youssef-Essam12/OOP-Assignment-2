#pragma once
#include <JuceHeader.h>
#include "audio_player.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::ChangeListener,
    public juce::Timer {
public:
    PlayerGUI(PlayerAudio &audio_player);
    ~PlayerGUI();

    void paint(juce::Graphics& g);
    void resized();
    void buttonClicked(juce::Button* button);
    void sliderValueChanged(juce::Slider* slider);

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void timerCallback() override;

private:
    PlayerAudio& player;
        
    // -------Buttons-------------
    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton muteButton{ "Mute" };
    
    //----------Sliders-------------
    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::AudioFormatManager formatManager;

    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI);
};