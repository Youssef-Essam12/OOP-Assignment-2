#include "PlayPauseLook.h"

void PlayPauseLook::drawButtonBackground(juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool isMouseOver,
    bool isButtonDown)
{
    // Get the button's drawing area, reduced slightly for a margin
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);


    // Hover effect
    auto baseColour = (isMouseOver || isButtonDown) ? 
          backgroundColour.brighter(0.2f)
        : backgroundColour;
    g.setColour(baseColour);
    g.fillEllipse(bounds);

    // --- 2. Check the button's toggle state and draw the icon ---

    g.setColour(juce::Colours::white); // Set the icon colour

    if (button.getToggleState())
    {
        // --- DRAW PAUSE ICON (Two Bars) ---
        // The button is "On" (Playing) -> Show "Pause"

        auto barWidth = bounds.getWidth() * 0.2f;    // 20% of the width
        auto barHeight = bounds.getHeight() * 0.5f;  // 50% of the height
        auto barY = bounds.getCentreY() - barHeight / 2.0f; // Center vertically

        // Draw left bar
        g.fillRect(bounds.getCentreX() - barWidth * 1.5f, barY, barWidth, barHeight);

        // Draw right bar
        g.fillRect(bounds.getCentreX() + barWidth * 0.5f, barY, barWidth, barHeight);
    }
    else
    {
        // --- DRAW PLAY ICON (Triangle) ---
        // The button is "Off" (Paused) -> Show "Play"

        juce::Path playTriangle;
        auto x = bounds.getCentreX();
        auto y = bounds.getCentreY();
        auto h_half = bounds.getHeight() * 0.3f; // Half-height of triangle
        auto w_left = bounds.getWidth() * 0.2f;  // x-offset for left points
        auto w_right = bounds.getWidth() * 0.25f; // x-offset for right point

        playTriangle.addTriangle(x - w_left, y - h_half,   // Top-left point
            x - w_left, y + h_half,   // Bottom-left point
            x + w_right, y);         // Right point

        g.fillPath(playTriangle);
    }
}