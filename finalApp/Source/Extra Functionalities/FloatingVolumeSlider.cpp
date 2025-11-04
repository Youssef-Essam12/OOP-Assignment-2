#include "FloatingVolumeSlider.h"

FloatingVolumeSlider::FloatingVolumeSlider(PlayerAudio& player) : audio_player(player)
{
    volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(audio_player.isWokring() ? audio_player.getGain() : 0.25);
    volumeSlider.addListener(this);
    addChildComponent(volumeSlider); 
    volumeSlider.setVisible(false);
    addChildComponent(volumeSlider);

    volumeSlider.setInterceptsMouseClicks(true, false);

    setInterceptsMouseClicks(true, true); 
}

FloatingVolumeSlider::~FloatingVolumeSlider()
{
    stopTimer();
}

void FloatingVolumeSlider::resized()
{
    auto bounds = getLocalBounds();

    volumeSlider.setBounds(bounds);
}

void FloatingVolumeSlider::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::transparentBlack);
    g.fillRect(getLocalBounds());
    if (isSliderVisible)
    {
        g.setColour(juce::Colours::black.withAlpha(0.7f));
        g.fillRoundedRectangle(volumeSlider.getBounds().toFloat(), 5.0f);
    }
}


void FloatingVolumeSlider::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        audio_player.setGain(slider->getValue());
    }
}

void FloatingVolumeSlider::mouseEnter(const juce::MouseEvent& event)
{
    stopTimer();
    showSlider(true);
}

void FloatingVolumeSlider::mouseExit(const juce::MouseEvent& event)
{
    startTimer(500);
}

void FloatingVolumeSlider::timerCallback()
{
    stopTimer(); 
    if (!getLocalBounds().contains(getMouseXYRelative()))
    {
        showSlider(false);
    }
}

void FloatingVolumeSlider::showSlider(bool shouldBeVisible)
{
    if (shouldBeVisible == isSliderVisible) return; // Already in this state
    isSliderVisible = shouldBeVisible;

    juce::ComponentAnimator& animator = juce::Desktop::getInstance().getAnimator();

    auto bounds = getLocalBounds();
    auto sliderOffBounds = bounds.withX(bounds.getRight());

    int animationTimeMs = 200; // Store the time

    if (shouldBeVisible)
    {
        // --- Show Slider ---
        volumeSlider.setVisible(true);
        volumeSlider.setBounds(sliderOffBounds); // Start off-screen

        animator.animateComponent(&volumeSlider, bounds, 1.0f, animationTimeMs,
            false, 0.0, 0.0);
    }
    else
    {
        // --- Hide Slider ---
        animator.animateComponent(&volumeSlider, sliderOffBounds, 0.0f, animationTimeMs,
            false, 0.0, 0.0);

        juce::Timer::callAfterDelay(animationTimeMs + 10, [this]()
            {
                if (!isSliderVisible)
                    volumeSlider.setVisible(false);
            });
    }

    repaint();
}