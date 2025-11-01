#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"


class EditorComp : public juce::Component, juce::Slider::Listener
{
public:
    EditorComp(PlayerAudio& player);
    ~EditorComp();

    void paint(juce::Graphics& g) override;
    void resized() override;


    void sliderValueChanged(juce::Slider* slider);
    

    void update(const juce::File& file);


private:
    PlayerAudio& audio_player;

    juce::Slider low_eq, mid_eq, high_eq;
    juce::Slider reverbSlider;
    juce::Slider speedSlider;

    juce::Label lowLabel, midLabel, highLabel, reverbLabel, speedLabel;
    float low_eq_value, mid_eq_value, high_eq_value;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EditorComp);

};