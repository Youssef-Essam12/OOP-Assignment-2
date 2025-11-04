#pragma once
#include <JuceHeader.h>
#include "LeftNavComp.h"


LeftNavComp::LeftNavComp() {

    viewButtons.emplace_back(new juce::TextButton("Normal"));
    viewButtons.emplace_back(new juce::TextButton("Playlist"));
    viewButtons.emplace_back(new juce::TextButton("Editor"));
    viewButtons.emplace_back(new juce::TextButton("Markers"));
    viewButtons.emplace_back(new juce::TextButton("Mixer")); // Added Mixer button


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

void LeftNavComp::paint(juce::Graphics& g)
{
    const juce::Colour surfaceColour = juce::Colour::fromString("FF1F222A");
    const juce::Colour separatorColour = juce::Colour::fromString("FF33363D"); // A subtle, dark grey separator

    g.fillAll(surfaceColour);
    juce::Rectangle<int> bounds = getLocalBounds();
    g.setColour(separatorColour);

    g.fillRect(bounds.getRight() - 1, bounds.getY(), 1, bounds.getHeight());
}


void LeftNavComp::buttonClicked(juce::Button* button) {
    for (int i = 0; i < (int)viewButtons.size(); i++) {
        if (button == viewButtons[i]) {
            set_view(i);
        }
    }
}

void LeftNavComp::changeListenerCallback(juce::ChangeBroadcaster* source) {
    juce::ignoreUnused(source);
}

void LeftNavComp::update(const juce::File& file)
{
    juce::ignoreUnused(file);
}