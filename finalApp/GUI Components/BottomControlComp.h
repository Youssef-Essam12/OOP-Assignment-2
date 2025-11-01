#pragma once
#include <JuceHeader.h>
#include "../PlayerAudio.h"
#include "../Extra Functionalities/PlayPauseLook.h"

class BottomControlComp : public juce::Component,
                          public juce::Button::Listener,
                          public juce::Slider::Listener
{
    /*
        This class containts control units:
            - Position slider
            - Play/Pause button
            - Mute/Segment checkboxes
            - Segment slider (when toggled)
            - Shuffle button
            - Loop button
    */ 
public:
    BottomControlComp(PlayerAudio& audio_player);
    void resized() override;
    void paint(juce::Graphics& g) override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    void update(); // Called by PlayerGUI's timer



    

private:
    
    PlayerAudio& audio_player;

    // Sliders
    juce::Slider positionSlider;

    // Text Buttons
    juce::TextButton backwardButton{ "-10s" };
    juce::TextButton forwardButton{ "+10s" };


    // Check Boxes
    juce::ToggleButton muteToggle{ "Mute" };
    juce::ToggleButton repeatToggle{ "Repeat" };
    juce::ToggleButton shuffleToggle{ "Shuffle" };
    juce::ToggleButton abSegmentToggle{ "Loop Segment" };
    
    // Circular play button
    PlayPauseLook pp_customlook;
    juce::TextButton playPauseButton{ "" };
};


