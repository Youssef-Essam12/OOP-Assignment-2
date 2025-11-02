#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"
#include "NormalViewComp.h"
#include "PlaylistEntry.h"

#include <functional>

class MixerViewComp : public juce::Component, 
    public juce::Button::Listener,
    public juce::Slider::Listener {
public:

    MixerViewComp(PlayerAudio& audio_player_1, PlayerAudio& audio_player_2);
    ~MixerViewComp();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    
    void load(bool player);

    void setPercentage(double new_percentage);
    double getPercentage();

    void updateGains();

private:

    double audio_player_1_percentage;

    PlayerAudio& audio_player_1;
    PlayerAudio& audio_player_2;

    juce::Slider percentage_slider;

    juce::Slider position_1_slider;
    juce::Slider position_2_slider;

    juce::Slider audio_1_slider;
    juce::Slider audio_2_slider;

    juce::TextButton audio_player_1_load_button{"Load Button"};
    juce::TextButton audio_player_2_load_button{ "Load Button" };

    juce::TextButton audio_player_1_play_button{ "Play" };
    juce::TextButton audio_player_2_play_button{ "Play" };

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerViewComp);
};