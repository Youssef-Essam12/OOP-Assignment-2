#pragma once
#include "PlayerGUI.h"

PlayerGUI::PlayerGUI(PlayerAudio& audio_player) : player(audio_player),
thumbnailCache(5),
thumbnail(512, formatManager, thumbnailCache) {


    addAndMakeVisible(playlist_component);
    formatManager.registerBasicFormats();
    thumbnail.addChangeListener(this);
    playlist_component.setAlwaysOnTop(true);
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton  , &stopButton, 
                       &muteButton, &playPauseButton, &toEnd, 
                       &toStart   , &backward       , &forward, 
                       &loopButton, &playlist_menu })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Add sliders
    std::vector<juce::Slider*> sliders = {
        &volumeSlider,
        &speedSlider,
        &positionSlider
    };
    for (auto *slider : sliders) {
        slider->setRange(0.0, 100, 1);
        slider->setValue(20);
        slider->addListener(this);
        addAndMakeVisible(slider);
        slider->setTextBoxStyle(juce::Slider::TextBoxRight,
            false, // read-only
            60,    // Box width
            slider->getTextBoxHeight());

        slider->setTextValueSuffix(" %");
    }

    positionSlider.textFromValueFunction = [&](double value) {
        std::string result = "";

		int minutes = static_cast<int>(value) / 60;
		int seconds = value - (minutes * 60);
		
        std::string seconds_string = std::to_string(seconds);
		if (seconds < 10) seconds_string = "0" + seconds_string;

        std::string minutes_string = std::to_string(minutes);
        if (minutes < 10) minutes_string = "0" + minutes_string;
        
        return minutes_string + ":" + seconds_string ;
	};

    speedSlider.setRange(30, 200, 1);
    speedSlider.setValue(100);

    positionSlider.setNumDecimalPlacesToDisplay(2);
    positionSlider.setTextValueSuffix(" s");

    startTimerHz(30);
    // Add labels
    std::vector<juce::Label*> labels = {
        &volumeLabel,
        &speedLabel,
		&titleLabel,
		&artistLabel
    };
    for (auto* label : labels) {
        addAndMakeVisible(label);
    }
	
    volumeLabel.setText("Volume", juce::dontSendNotification);
    speedLabel.setText("Speed", juce::dontSendNotification);
    
    titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);

    artistLabel.setFont(juce::Font(14.0f));
    artistLabel.setJustificationType(juce::Justification::centred);
    
    
}

PlayerGUI::~PlayerGUI() {
    stopTimer();

    // Clean up dynamically created playlist entry buttons
    for (auto* btn : playlist_buttons) {
        delete btn;
    }
    playlist_buttons.clear();
}

void PlayerGUI::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);

    //player.setSpeed(150);
    auto waveform = juce::Rectangle<int>(20, 200, getWidth() - 40, 300);

    g.setColour(juce::Colours::lightgrey);
    g.drawRect(waveform);

    if (thumbnail.getNumChannels() == 0) {
        g.setColour(juce::Colours::white);
        g.drawFittedText("No Audio Loaded", waveform, juce::Justification::centred, 1);
    }
    else {
        g.setColour(juce::Colours::hotpink);
        thumbnail.drawChannels(g, waveform, 0.0, thumbnail.getTotalLength(), 1.0f);

        double pos = player.getPosition();
        double len = player.getLength();

        if (len > 0) {
            float current_pos = (float)(waveform.getX() + (pos / len) * waveform.getWidth());
            g.setColour(juce::Colours::red);
            g.drawLine(current_pos, (float)waveform.getY(), current_pos, (float)waveform.getBottom(), 2.0f);
        }
    }
}

void PlayerGUI::display_playlist_menu() {

    playlist_componenet_visible = !playlist_componenet_visible;

    juce::ComponentAnimator& animator = juce::Desktop::getInstance().getAnimator();
    

    juce::Rectangle<int> targetBounds;

    if (playlist_componenet_visible)
    {
        // Target is ON-screen
        targetBounds = juce::Rectangle<int>(getWidth() - panelWidth, 0, panelWidth, panelHeight);
    }
    else
    {
        // Target is OFF-screen (to the right)
        targetBounds = juce::Rectangle<int>(getWidth(), 0, panelWidth, panelHeight);
    }

    // Animate the component to the target bounds over 300ms
    animator.animateComponent(
        &playlist_component,  // Component to animate
        targetBounds,         // Target position and size
        1.0f,                 // Target opacity (1.0 = fully visible)
        300,                  // Time in milliseconds
        false,                // Use an ease-out curve
        0.0,                  // Start velocity
        0.0                   // End velocity
    );
}

void PlayerGUI::add_playlist_entry(const juce::File& file) {
    juce::String fileName = file.getFileNameWithoutExtension();

    auto* newEntry = new juce::TextButton(fileName);

    newEntry->setButtonText(fileName);
    newEntry->addListener(this); 

    playlist_component.addAndMakeVisible(newEntry);
    playlist_buttons.push_back(newEntry);

    resized();
}

void PlayerGUI::resized() {
 
    int buttons_w = 100;
    int buttons_h = 50;

    int button_margin = buttons_w * 0.40;
    int buttons_x = (getWidth() - buttons_w) >> 1;
    int buttons_y = getHeight() - 1.5 * button_margin;

    int distance_factor = 3;

    playPauseButton.setBounds(buttons_x                                      , buttons_y, buttons_w, buttons_h);
    loadButton.setBounds     (buttons_x - 1 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    restartButton.setBounds  (buttons_x + 1 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    stopButton.setBounds     (buttons_x - 2 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    muteButton.setBounds     (buttons_x + 2 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    toStart.setBounds        (buttons_x - 3 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    toEnd.setBounds          (buttons_x + 3 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    loopButton.setBounds     (buttons_x - 4 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    forward.setBounds        (buttons_x + 4 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
    backward.setBounds       (buttons_x - 5 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
	playlist_menu.setBounds  (buttons_x + 5 * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);

    int slider_w = getWidth() * 0.75;
	int slider_h = 30;  
    int slider_x = (getWidth() - slider_w) >> 1;
	int slider_y = playPauseButton.getY() - button_margin - slider_h;

    volumeSlider.setBounds  (slider_x, slider_y               , slider_w, slider_h);
    speedSlider.setBounds   (slider_x, slider_y - 1 * slider_h, slider_w, slider_h);
    positionSlider.setBounds(slider_x, slider_y - 3 * slider_h, slider_w, slider_h);

    int label_w = 70;
    int label_h = 50;
    int label_margin = 20;
    volumeLabel.setBounds(volumeSlider.getX(), volumeSlider.getY(), label_w, label_h);
    speedLabel.setBounds(speedSlider.getX(), speedSlider.getY(), label_w, label_h);

    titleLabel.setBounds(0, 50, getWidth(), label_h);
    artistLabel.setBounds(0, 100, getWidth(), 20);

    int toggleButtonW = 100;
    int toggleButtonH = 40;

    panelWidth = getWidth() / 3;
    panelHeight = getHeight() / 3;

    if (playlist_componenet_visible)
    {
        playlist_component.setBounds(getWidth() - panelWidth, 0, panelWidth, panelHeight);
    }
    else
    {
        playlist_component.setBounds(getWidth(), 0, panelWidth, panelHeight);
    }

    int buttonMargin = 10;
    int buttonHeight = 30;
    int startY = 10; // Start at the top of the panel (or below the toggle button if it's inside the panel)

    // Layout the dynamically created buttons inside the 'playlist_component'
    for (int i = 0; i < playlist_buttons.size(); ++i) {
        playlist_buttons[i]->setBounds(buttonMargin,
            startY + i * (buttonHeight + buttonMargin),
            panelWidth - 2 * buttonMargin,
            buttonHeight);
    }
}
void PlayerGUI::buttonClicked(juce::Button* button) {

    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (player.isFileAlreadyLoaded(file))
                {
                    juce::AlertWindow::showMessageBoxAsync(
                        juce::AlertWindow::WarningIcon,     // Icon type
                        "File Already Loaded",              // Window title
                        "The file '" + file.getFileName()   // Message
                        + "' is already in the playlist.",
                        "OK"                                // Button text
                    );
                }
                else
                {
                    if (player.load(file)) {
                        add_playlist_entry(file);
                    }
                }
            });
    }

    else if (button == &restartButton)
    {
        player.start();
    }

    else if (button == &stopButton)
    {
        player.stop();
    }
    else if (button == &muteButton) {
        player.mute();
    }
    else if(button == &playPauseButton)
    {
        player.play_pause();
	}
    else if(button == &toStart)
    {
        player.setPosition(0.0);
        player.start();
    }
    else if (button == &toEnd)
    {
        player.setPosition(player.getLength());
	}
    else if (button == &forward) {
        player.forward_backward(1);
    }
    else if (button == &backward) {
        player.forward_backward(0);
    }
    else if (button == &loopButton)
    {
        player.loop();
    }
    else if(button == & playlist_menu)
    {
        display_playlist_menu();
    }
    else // play list buttons
    {
        // Check if the clicked button is one of the playlist entries
        for (size_t i = 0; i < playlist_buttons.size(); ++i) {
            if (button == playlist_buttons[i]) {
                const juce::File& file = player.getPlaylistFile(i);
                if (file.existsAsFile()) {
                    if (player.playFile(i)) {
                        updateTrackInfo();
                    }
                    thumbnail.setSource(new juce::FileInputSource(file));
                }
                break;
            }
        }
	}
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider) {

    float current_value = (float)slider->getValue();

    if (slider == &volumeSlider) {
        player.setGain(current_value / 100);
    }
    else if (slider == &speedSlider) {
        player.setSpeed(current_value / 100);
    }
    else if (slider == &positionSlider)
    {
        if (slider->isMouseButtonDown())
        {
            player.setPosition(current_value);
        }
    }

}

void PlayerGUI::updateTrackInfo()
{
    if (player.isWokring()) {
        titleLabel.setText(player.getTitle(), juce::dontSendNotification);
        artistLabel.setText(player.getArtist(), juce::dontSendNotification);
    }
}

void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &thumbnail) {
        repaint();
    }
}

void PlayerGUI::timerCallback()
{
    double truelengthInSeconds = player.getOriginalLength();
         
    if (truelengthInSeconds > 0)
    {
	    double ratio = truelengthInSeconds / player.getLength();

        positionSlider.setRange(0.0, truelengthInSeconds, juce::dontSendNotification);
        positionSlider.setValue(ratio * player.getPosition(), juce::dontSendNotification);
    }
    repaint();
}