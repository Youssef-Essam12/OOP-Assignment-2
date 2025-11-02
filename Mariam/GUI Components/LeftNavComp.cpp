#pragma once
#include <JuceHeader.h>
#include "LeftNavComp.h"


LeftNavComp::LeftNavComp() {

    viewButtons.emplace_back(new juce::TextButton("Normal"));
    viewButtons.emplace_back(new juce::TextButton("Playlist"));
    viewButtons.emplace_back(new juce::TextButton("Editor"));
   
    for (auto* btn : viewButtons) {
        addAndMakeVisible(btn);
        btn->addListener(this);
        btn->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    }
}

LeftNavComp::~LeftNavComp() {

    for (auto* btn : viewButtons) {
        btn->removeListener(this);
    }
}

void LeftNavComp::resized() {
    auto area = getLocalBounds();
    int padding = 10; // Space around and between buttons
    int buttonHeight = 40; // Fixed height for buttons

    auto buttonArea = area.reduced(padding);
    for (auto* btn : viewButtons) {
        btn->setBounds(buttonArea.removeFromTop(buttonHeight));
        buttonArea.removeFromTop(padding);
    }
}

void LeftNavComp::paint(juce::Graphics& g) {
    // Draw the background rectangle for the left navigation area
    g.setColour(juce::Colours::lightgrey.darker(0.8f)); // Dark background
    g.fillRect(getLocalBounds());

    // Optional: Draw a separator line on the right edge
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds().toFloat(), 1.0f);
}


void LeftNavComp::buttonClicked(juce::Button* button) {
    for (int i = 0; i < (int)viewButtons.size(); i++) {
        if (button == viewButtons[i]) {
            set_view(i);
        }
    }
}

void LeftNavComp::changeListenerCallback(juce::ChangeBroadcaster* source) {

}

void LeftNavComp::update(const juce::File& file)
{

}


