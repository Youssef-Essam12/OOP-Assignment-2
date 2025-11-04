#include <JuceHeader.h>
#include "NormalViewComp.h"


NormalViewComp::NormalViewComp(PlayerAudio& player) :
    audio_player(player),
    thumbnailCache(5),
    thumbnail(512, formatManager, thumbnailCache) {

    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);

    // Add labels      
    for (auto* label : { &titleLabel, &artistLabel }) {
        addAndMakeVisible(label);
    }

    titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);

    artistLabel.setFont(juce::Font(14.0f));
    artistLabel.setJustificationType(juce::Justification::centred);
}

NormalViewComp::~NormalViewComp() {

}

void NormalViewComp::resized() {

    auto bounds = getLocalBounds();

    int titleHeight = 50;
    int artistHeight = 20;
    int spacing = 10;

    titleLabel.setBounds(bounds.removeFromTop(titleHeight));
    artistLabel.setBounds(bounds.removeFromTop(artistHeight));

    bounds.removeFromTop(spacing);
}

void NormalViewComp::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1c1c1c));

    auto waveform = juce::Rectangle<int>(20, 170, getWidth() - 40, 300);

    g.setColour(juce::Colours::lightgrey);
    g.drawRect(waveform);

    if (audio_player.getIndex() == -1) {
        g.setColour(juce::Colours::red);
        g.drawFittedText("No Audio Loaded", waveform, juce::Justification::centred, 1);
    }
    else {
        g.setColour(juce::Colours::hotpink);
        thumbnail.drawChannels(g, waveform, 0.0, thumbnail.getTotalLength(), 1.0f);

        double pos = audio_player.getPosition();
        double len = audio_player.getLength();

        if (len > 0) {
            float current_pos = (float)(waveform.getX() + (pos / len) * waveform.getWidth());
            g.setColour(juce::Colours::red);
            g.drawLine(current_pos, (float)waveform.getY(), current_pos, (float)waveform.getBottom(), 2.0f);
        }
    }
}



void NormalViewComp::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &thumbnail) {
        repaint();
    }
}

void NormalViewComp::update(const juce::File& file)
{
    if (audio_player.getOriginalIndex() != audio_player.getIndex()) {
        int current_index = audio_player.getIndex();
        titleLabel.setText((audio_player.isWokring() ? audio_player.getTitle(current_index) : ""), juce::dontSendNotification);
        artistLabel.setText((audio_player.isWokring() ? audio_player.getArtist(current_index) : ""), juce::dontSendNotification);
        thumbnail.setSource(new juce::FileInputSource(file));
        updateTrackInfo();
        audio_player.setOriginalIndex(audio_player.getIndex());
    }
    repaint();
}

void NormalViewComp::updateTrackInfo()
{
    int current_index = audio_player.getIndex();
    titleLabel.setText((audio_player.isWokring() ? audio_player.getTitle(current_index) : ""), juce::dontSendNotification);
    artistLabel.setText((audio_player.isWokring() ? audio_player.getArtist(current_index) : ""), juce::dontSendNotification);
}