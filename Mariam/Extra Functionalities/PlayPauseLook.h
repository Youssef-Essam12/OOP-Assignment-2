#pragma once
#include <JuceHeader.h>


class PlayPauseLook : public juce::LookAndFeel_V4
{
public:
    // Overrides the default button look
    void drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, 
                              bool isMouseOver, bool isButtonDown) override;
};