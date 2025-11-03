#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"
#include "../Extra Functionalities/PlayPauseLook.h"

class MinimizedViewComp : public juce::Component,      
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    MinimizedViewComp(PlayerAudio& player);
    ~MinimizedViewComp() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void updateTrackInfo();
    void updateControlState();

private:
    PlayerAudio& audio_player;

    // --- Controls ---
    PlayPauseLook pp_customlook;
    juce::ToggleButton playPauseButton{ "" };
    juce::Slider positionSlider;

    // --- Labels ---
    juce::Label titleLabel;
    juce::Label artistLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MinimizedViewComp);
};