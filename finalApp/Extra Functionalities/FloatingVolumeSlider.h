#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h" 

class FloatingVolumeSlider : public juce::Component,
    // public juce::Button::Listener, 
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    FloatingVolumeSlider(PlayerAudio& player);
    ~FloatingVolumeSlider() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void timerCallback() override;

private:
    void showSlider(bool shouldBeVisible);

    PlayerAudio& audio_player;

    juce::Slider volumeSlider;
    // juce::DrawableButton volumeIcon;
    // juce::DrawablePath iconPath; 

    bool isSliderVisible = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FloatingVolumeSlider);
};