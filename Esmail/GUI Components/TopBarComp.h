#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"

#include <functional>

class TopBarComp : public juce::Component,
                   public juce::Button::Listener 
{
public:
    
    TopBarComp(PlayerAudio& audio_player);

    void buttonClicked(juce::Button* button) override;
    void resized() override;

    std::function<void(const juce::File& file)> onFileLoaded;
private:

    PlayerAudio& audio_player;

    // Buttons
    juce::TextButton loadButton{ "Load" };


    std::unique_ptr<juce::FileChooser> fileChooser;
};