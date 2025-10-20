#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener, 
    public juce::Timer {
public:
    PlayerGUI(PlayerAudio &audio_player);
    ~PlayerGUI();

    void paint(juce::Graphics& g);
    void resized();
    void buttonClicked(juce::Button* button);
    void sliderValueChanged(juce::Slider* slider);

    void timerCallback() override;
    void updateTrackInfo();

private:
    //-------Global variables-------
    PlayerAudio& player;
    
        
    // -------Buttons-------------
    juce::TextButton loadButton{ "Load Audio" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton playPauseButton{ "Play/Pause" };
    juce::TextButton toEnd{ "To End" };
    juce::TextButton toStart{ "To Start" };
    
    //----------Sliders-------------
    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Slider positionSlider;

    //----------Labels-------------
    juce::Label volumeLabel;
    juce::Label speedLabel;
    juce::Label titleLabel;
    juce::Label artistLabel;

    std::unique_ptr<juce::FileChooser> fileChooser;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI);
};