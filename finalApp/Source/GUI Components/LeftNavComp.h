#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"
#include <functional>


class LeftNavComp : public juce::Component,
    public juce::ChangeListener, juce::Button::Listener
{
public:
    LeftNavComp();
    ~LeftNavComp();

    void paint(juce::Graphics& g) override;
    void resized() override;


    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void update(const juce::File& file);

    void buttonClicked(juce::Button* button);

    std::function<void(int view_index)> set_view;

private:
    std::vector<juce::TextButton*> viewButtons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LeftNavComp);

};