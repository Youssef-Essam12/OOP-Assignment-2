#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"


class NormalViewComp : public juce::Component,
                       public juce::ChangeListener
{
public:
    NormalViewComp(PlayerAudio& player);
    ~NormalViewComp();

    void paint(juce::Graphics& g) override;
    void resized() override;


    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void update(const juce::File& file);

    void updateTrackInfo();


private:
    PlayerAudio& audio_player;

    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;


    juce::Label titleLabel;
    juce::Label artistLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NormalViewComp);

};