    #pragma once
    #include "PlayerGUI.h"


    int marker::marker_cnt = 1;

    PlayerGUI::PlayerGUI(PlayerAudio& audio_player) : player(audio_player),
    thumbnailCache(5),
    thumbnail(512, formatManager, thumbnailCache) {

        formatManager.registerBasicFormats();
        thumbnail.addChangeListener(this);


        playlist_component = new juce::Component;
        playlistViewport.setViewedComponent(playlist_component, true);
        playlistViewport.setAlwaysOnTop(true);
        playlistViewport.setScrollBarThickness(10);
        addAndMakeVisible(playlistViewport);

        marker_list_component = new juce::Component;
        markerlistViewport.setViewedComponent(marker_list_component, true);
        markerlistViewport.setAlwaysOnTop(true);
        markerlistViewport.setScrollBarThickness(10);
        addAndMakeVisible(markerlistViewport);

        // Add buttons
        for (auto* btn : { &loadButton, &restartButton  , &stopButton,
                           &muteButton, &playPauseButton, &toEnd,
                           &toStart   , &backward       , &forward,
                           &loopButton, &playlist_menu, &addMarker, &displayMarkers, &clearMarkers,& setAButton,& setBButton })
        {
            btn->addListener(this);
            addAndMakeVisible(btn);
        }

   
        // Add labels
        std::vector<juce::Label*> labels = {
            &volumeLabel,
            &speedLabel,
            &titleLabel,
            &artistLabel,
            &lowLabel,
            &midLabel,
            &highLabel,
            &reverbLabel,
        };
        for (auto* label : labels) {
            addAndMakeVisible(label);
        }

        volumeLabel.setText("Volume", juce::dontSendNotification);
        speedLabel.setText("Speed", juce::dontSendNotification);
        lowLabel.setText("Low", juce::dontSendNotification);
        midLabel.setText("Mid", juce::dontSendNotification);
        highLabel.setText("High", juce::dontSendNotification);
        reverbLabel.setText("Reverb", juce::dontSendNotification);

        titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
        titleLabel.setJustificationType(juce::Justification::centred);

        artistLabel.setFont(juce::Font(14.0f));
        artistLabel.setJustificationType(juce::Justification::centred);

        // Add sliders
        std::vector<juce::Slider*> sliders = {
            &volumeSlider,
            &speedSlider,
            &positionSlider,
            &low_eq,
            &mid_eq,
            &high_eq,
            &reverbSlider
        };

        for (auto* slider : sliders) {
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

        low_eq.setRange(-20.0, 20.0, 0.1);
        low_eq.setValue(0.0);
        low_eq.setTextValueSuffix(" dB");

        mid_eq.setRange(-20.0, 20.0, 0.1);
        mid_eq.setValue(0.0);
        mid_eq.setTextValueSuffix(" dB");

        high_eq.setRange(-20.0, 20.0, 0.1);
        high_eq.setValue(0.0);
        high_eq.setTextValueSuffix(" dB");

        // In constructor, after setting up the sliders:
        low_eq_value = 0.0f;
        mid_eq_value = 0.0f;
        high_eq_value = 0.0f;

        positionSlider.textFromValueFunction = [&](double value) {
            std::string result = "";

            int minutes = static_cast<int>(value) / 60;
            int seconds = value - (minutes * 60);

            std::string seconds_string = std::to_string(seconds);
            if (seconds < 10) seconds_string = "0" + seconds_string;

            std::string minutes_string = std::to_string(minutes);
            if (minutes < 10) minutes_string = "0" + minutes_string;

            return minutes_string + ":" + seconds_string;
            };

        speedSlider.setRange(30, 200, 1);
        speedSlider.setValue(100);

        positionSlider.setNumDecimalPlacesToDisplay(2);
        positionSlider.setTextValueSuffix(" s");
        resized();
        startTimerHz(30);

        // Load Session
        juce::File session_file("data.json");
        juce::var session;
        if (session_file.existsAsFile()) {
            juce::String json_data = session_file.loadFileAsString();
            session = juce::JSON::parse(json_data);
        }
        else return;

        if (session.isVoid()) return;
        if (
            session["playlist"].isVoid() ||
            session["last_played_index"].isVoid() ||
            session["timestamp"].isVoid())
            return;

        juce::Array<juce::var> *paths_from_json_file = session["playlist"].getArray();
        if (paths_from_json_file == nullptr) return;
        std::string last_played_path = session["last_audio_path"].toString().toStdString();

        for (auto &path : *paths_from_json_file) {
            juce::File f = juce::File(path.toString());
            if (player.load(f)) add_playlist_entry(f);
        }
        int last_played_index = (int)session["last_played_index"];
        juce::File file = juce::File((juce::String)last_played_path);
        if (file.existsAsFile()) {
            if (player.playFile(last_played_index)) {
                player.setIndex(last_played_index);
                updateTrackInfo();
            }
            thumbnail.setSource(new juce::FileInputSource(file));
            player.setPosition((double)session["timestamp"]);
            player.play_pause();
        }


        juce::Array<juce::var>* markers_from_json_file = session["markers"].getArray();
        if (markers_from_json_file == nullptr) return;
        for (auto &m : *markers_from_json_file) {
            markers_pos.push_back((double)m);
        }
    }

    PlayerGUI::~PlayerGUI() {
        stopTimer();

        for (auto* btn : playlist_buttons) {
            delete btn;
        }

        for (auto* btn : playlist_delete_buttons) {
            delete btn;
        }

        playlist_buttons.clear();
        playlist_delete_buttons.clear();

        juce::DynamicObject* session = new juce::DynamicObject();

        int current_audio_playing = player.getIndex();
        if (~current_audio_playing) session->setProperty("last_audio_path", juce::String(playlist_paths[current_audio_playing]));
        session->setProperty("last_played_index", current_audio_playing);
        session->setProperty("timestamp", player.getPosition());
        juce::Array<juce::var> arr;
        for (auto& path : playlist_paths) arr.add(juce::String(path));
        session->setProperty("playlist", arr);

        arr.clear();
        for (auto& m : markers) arr.add(m->get_pos());
        session->setProperty("markers", arr);

        juce::var session_json(session);
        juce::String jsonOutput = juce::JSON::toString(session_json, false);
        juce::File sessionFile("data.json");
        sessionFile.replaceWithText(jsonOutput);

        for (auto* m : markers) {
            delete m;
        }
        markers.clear();

        for (auto* l : markers_labels) {
            delete l;
        }
        markers_labels.clear();
    }

    void PlayerGUI::paint(juce::Graphics& g) {
        g.fillAll(juce::Colours::darkgrey);
        auto waveform = juce::Rectangle<int>(20, 170, getWidth() - 40, 300);

        g.setColour(juce::Colours::lightgrey);
        g.drawRect(waveform);

        if (player.getIndex() == -1) {
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
            &playlistViewport,  // Component to animate
            targetBounds,         // Target position and size
            1.0f,                 // Target opacity (1.0 = fully visible)
            300,                  // Time in milliseconds
            false,                // Use an ease-out curve
            0.0,                  // Start velocity
            0.0                   // End velocity
        );
    }

    void PlayerGUI::display_markers_menu() {
        markers_list_visible = !markers_list_visible;

        juce::ComponentAnimator& animator = juce::Desktop::getInstance().getAnimator();
        juce::Rectangle<int> targetBounds;

        if (markers_list_visible)
        {
            // Target is ON-screen (on the left side)
            targetBounds = juce::Rectangle<int>(0, 0, panelWidth, panelHeight);
        }
        else
        {
            // Target is OFF-screen (to the left)
            targetBounds = juce::Rectangle<int>(-panelWidth, 0, panelWidth, panelHeight);
        }

        animator.animateComponent(
            &markerlistViewport,
            targetBounds,
            1.0f,
            300,
            false,
            0.0,
            0.0
        );
    }

    void PlayerGUI::add_playlist_entry(const juce::File& file) {
        juce::String fileName = file.getFileNameWithoutExtension();

        auto* newEntry = new juce::TextButton(fileName);
        newEntry->setButtonText(fileName);
        newEntry->addListener(this);

        auto* deleteButton = new juce::TextButton();
        deleteButton->setButtonText("X");
        deleteButton->addListener(this);

        playlist_component->addAndMakeVisible(newEntry);
        playlist_component->addAndMakeVisible(deleteButton);

        playlist_buttons.push_back(newEntry);
        playlist_delete_buttons.push_back(deleteButton);

        playlist_paths.push_back(file.getFullPathName().toStdString());

        resized();
    }

    void PlayerGUI::add_markers_list_entry(std::string text) {
        auto* newEntry = new juce::TextButton();
        newEntry->setButtonText(text);
        newEntry->addListener(this);
    
        auto* deleteButton = new juce::TextButton();
        deleteButton->setButtonText("X");
        deleteButton->addListener(this);

        marker_list_component->addAndMakeVisible(newEntry);
        marker_list_component->addAndMakeVisible(deleteButton);

        marker_buttons.push_back(newEntry);
        marker_delete_buttons.push_back(deleteButton);
        resized();
    }

    void PlayerGUI::delete_marker(int index)
    {
        removeChildComponent(markers[index]);
        delete markers[index];
        markers.erase(markers.begin() + index);

        removeChildComponent(marker_buttons[index]);
        delete marker_buttons[index];
        marker_buttons.erase(marker_buttons.begin() + index);

        removeChildComponent(marker_delete_buttons[index]);
        delete marker_delete_buttons[index];
        marker_delete_buttons.erase(marker_delete_buttons.begin() + index);

        removeChildComponent(markers_labels[index]);
        delete markers_labels[index];
        markers_labels.erase(markers_labels.begin() + index);
    }

    void PlayerGUI::delete_button(int index)
    {
        // remove from playlist_buttons, playlist_delete_buttons, playlist_paths
        if (index == player.getIndex()) {
            player.remove_source();
            clear_markers();
            player.setIndex(-1);
        }
        if (index < player.getIndex()) {
            player.setIndex(player.getIndex()-1);
        }
        playlist_component->removeChildComponent(playlist_buttons[index]);
        playlist_component->removeChildComponent(playlist_delete_buttons[index]);
        delete playlist_buttons[index];
        delete playlist_delete_buttons[index];
        playlist_buttons.erase(playlist_buttons.begin()+index);
        playlist_delete_buttons.erase(playlist_delete_buttons.begin() + index);
        playlist_paths.erase(playlist_paths.begin() + index);
    }

    void PlayerGUI::display_markers() {
        if (!markers_added) {
            for (auto p : markers_pos) {
                add_marker(p, 0);
            }
            markers_pos.clear();
            markers_added = 1;
        }
        for (int i = 0; i < (int)markers.size(); i++) {
            if (markers[i]) {
                markers[i]->setVisible(markers_visible);
            }
            if (markers_labels[i]) {
                markers_labels[i]->setVisible(markers_visible);
            }
        }
    }

    void PlayerGUI::clear_markers()
    {
        marker::set_marker_cnt(1);
        for (auto* m : markers) {
            removeChildComponent(m);
            delete m;
        }
        for (auto* l : markers_labels) {
            removeChildComponent(l);
            delete l;
        }

        for (auto *btn : marker_buttons) {
            removeChildComponent(btn);
            delete btn;
        }

        for (auto* btn : marker_delete_buttons) {
            removeChildComponent(btn);
            delete btn;
        }
        markers.clear();
        markers_labels.clear();
        marker_buttons.clear();
        marker_delete_buttons.clear();
        player.clearSegmentLoop();
        aIndex = bIndex = -1;
    }

    void PlayerGUI::add_marker(double pos, bool f)
    {
        if (f && !markers_visible) return;
        if (player.getIndex() == -1) return;
        auto& lf = positionSlider.getLookAndFeel();
        auto layout = lf.getSliderLayout(positionSlider);
        auto trackBounds = layout.sliderBounds;

        double markerOffset = (pos / player.getOriginalLength()) * trackBounds.getWidth();
        std::string marker_text = "Marker";
        marker_text += std::to_string(marker::get_marker_cnt());
        auto m = new marker(positionSlider.getX() + trackBounds.getX() - 4, positionSlider.getY() + 10, markerOffset, pos, player.getLength());
        markers.push_back(m);
        addAndMakeVisible(m);
        m->onClick = [this, m](double p, double length) {
            double ratio = p/player.getOriginalLength();
            double new_pos = ratio*player.getLength();
            player.setPosition(new_pos);
        };
        juce::Label* label = new juce::Label();
        label->setText(marker_text, juce::dontSendNotification);
        label->setJustificationType(juce::Justification::centred);
        label->setFont(juce::Font(10.0f, juce::Font::bold));
        label->setColour(juce::Label::textColourId, juce::Colours::white);
        label->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);

        int labelWidth = 50;
        int labelHeight = 15;
        int markerX = positionSlider.getX() + trackBounds.getX() - 4 + markerOffset;
        int markerY = positionSlider.getY() + 10;
        label->setBounds(markerX - (labelWidth / 2) + 5, markerY - labelHeight - 2, labelWidth, labelHeight);
        addAndMakeVisible(label);
        markers_labels.push_back(label);
        add_markers_list_entry(marker_text);
        //resized();
    }

    void PlayerGUI::resized() {

        int buttons_w = 70;
        int buttons_h = 50;

        int button_margin = buttons_w * 0.40;
        int buttons_x = (getWidth() - buttons_w) >> 1;
        int buttons_y = getHeight() - 1.5 * button_margin;

        int distance_factor = 3;

        // Place buttons
        {
            int i = 1;
            for (auto* btn : { &playPauseButton,
                               &forward, &backward,
                               &toEnd, &toStart,
                               &muteButton,&restartButton,
                               &stopButton, &loopButton,
                               &playlist_menu,&loadButton,&addMarker, &displayMarkers, &clearMarkers, & setAButton,& setBButton })
            {
                int factor = (i % 2 == 0 ? 1 : -1);
                btn->setBounds(buttons_x + factor * (i / 2) * distance_factor * button_margin, buttons_y, buttons_w, buttons_h);
                i++;
            }
        }

        int slider_w = getWidth() * 0.75;
        int slider_h = 30;
        int slider_x = (getWidth() - slider_w) >> 1;
        int slider_y = playPauseButton.getY() - button_margin - slider_h;

        volumeSlider.setBounds(slider_x, slider_y, slider_w, slider_h);
        speedSlider.setBounds(slider_x, slider_y - 1 * slider_h, slider_w, slider_h);
        positionSlider.setBounds(slider_x, slider_y - 3 * slider_h, slider_w, slider_h);
        low_eq.setBounds(slider_x, slider_y - 4 * slider_h, slider_w, slider_h);
        mid_eq.setBounds(slider_x, slider_y - 5 * slider_h, slider_w, slider_h);
        high_eq.setBounds(slider_x, slider_y - 6 * slider_h, slider_w, slider_h);
        reverbSlider.setBounds(slider_x, slider_y - 7 * slider_h, slider_w, slider_h);

        int label_w = 70;
        int label_h = 50;
        int label_margin = 20;
        volumeLabel.setBounds(volumeSlider.getX(), volumeSlider.getY(), label_w, label_h);
        speedLabel.setBounds(speedSlider.getX(), speedSlider.getY(), label_w, label_h);
        lowLabel.setBounds(low_eq.getX(), low_eq.getY(), label_w, label_h);
        midLabel.setBounds(mid_eq.getX(), mid_eq.getY(), label_w, label_h);
        highLabel.setBounds(high_eq.getX(), high_eq.getY(), label_w, label_h);
        reverbLabel.setBounds(reverbSlider.getX(), reverbSlider.getY(), label_w, label_h);

        titleLabel.setBounds(0, 50, getWidth(), label_h);
        artistLabel.setBounds(0, 100, getWidth(), 20);

        int toggleButtonW = 100;
        int toggleButtonH = 40;

        panelWidth = 0.3*getWidth();
        panelHeight = 0.15*getHeight();

        int buttonMargin = 10;
        int buttonHeight = 30;
        int startY = 10;

        if (playlist_componenet_visible)
        {
            playlistViewport.setBounds(getWidth() - panelWidth, 0, panelWidth, panelHeight);
            playlistViewport.setScrollBarsShown(true, false);
        }

        // Make sure the viewed component size is updated
        int height = std::max(panelHeight, (int)playlist_buttons.size() * (buttonHeight + buttonMargin));
        playlist_component->setSize(panelWidth, height);


        const int deleteButtonWidth = 30;

        const int entryButtonWidth = panelWidth - (3 * buttonMargin) - deleteButtonWidth;
        for (int i = 0; i < playlist_buttons.size(); ++i) {
            playlist_buttons[i]->setBounds(buttonMargin,
                startY + i * (buttonHeight + buttonMargin),
                panelWidth - 2 * buttonMargin,
                buttonHeight);

            int yPos = startY + i * (buttonHeight + buttonMargin);
            playlist_delete_buttons[i]->setBounds(buttonMargin + entryButtonWidth + buttonMargin, // X position: Main Button End + Margin
                yPos,
                deleteButtonWidth,
                buttonHeight);
        }


        if (markers_list_visible)
        {
            markerlistViewport.setBounds(0, 0, panelWidth, panelHeight);
            markerlistViewport.setScrollBarsShown(true, false);
        }

        height = std::max(panelHeight, (int)marker_buttons.size() * (buttonHeight + buttonMargin));
        marker_list_component->setSize(panelWidth, height);

        for (int i = 0; i < marker_buttons.size(); ++i) {
            marker_buttons[i]->setBounds(buttonMargin,
                startY + i * (buttonHeight + buttonMargin),
                panelWidth - 2 * buttonMargin,
                buttonHeight);

            int yPos = startY + i * (buttonHeight + buttonMargin);
            marker_delete_buttons[i]->setBounds(buttonMargin + entryButtonWidth + buttonMargin, // X position: Main Button End + Margin
                yPos,
                deleteButtonWidth,
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

        else if (button == &stopButton)
        {
            player.stop();
        }
        else if (button == &muteButton) {
            player.mute();
        }
        else if (button == &playPauseButton)
        {
            player.play_pause();
        }
        else if (button == &toStart)
        {
            player.setPosition(0.0);
            player.start();
        }
        else if (button == &toEnd)
        {
            player.setPosition(player.getLength());
        }
        else if (button == &forward) {
            player.move_by(10);
        }
        else if (button == &backward) {
            player.move_by(-10);
        }
        else if (button == &loopButton)
        {
            player.loop();
        }
        else if (button == &setAButton)
        {
            double pos = player.getPosition();
            add_marker(pos, 1);
            aIndex = markers.size() - 1; // remember where marker is
            
        }
        else if (button == &setBButton)
        {
            double pos = player.getPosition();
            add_marker(pos, 1);
            bIndex = markers.size() - 1; 

            // activate segment looping:
            if (aIndex != -1 && bIndex != -1 &&
                markers[aIndex]->get_pos() < markers[bIndex]->get_pos())
            {
                player.enableSegmentLoop(markers[aIndex]->get_pos(), markers[bIndex]->get_pos());
   
            }
            else
            {
                player.clearSegmentLoop();
              
            }
        }

        else if (button == &playlist_menu)
        {
            display_playlist_menu();
        }
        else if (button == &addMarker) {
            add_marker(positionSlider.getValue(), 1);
        }
        else if (button == &displayMarkers) {
            markers_visible ^= 1;
            display_markers();
            display_markers_menu();
        }
        else if (button == &clearMarkers) {
            clear_markers();
            resized();
            repaint();
        }
        else // play list buttons or markers buttons
        {
            // Check if the clicked button is one of the playlist entries
            for (size_t i = 0; i < playlist_buttons.size(); ++i) {
                if (button == playlist_buttons[i]) {
                    if (i == player.getIndex()) break;
                    const juce::File& file = player.getPlaylistFile(i);
                    if (file.existsAsFile()) {
                        if (player.playFile(i)) {
                            clear_markers();
                            updateTrackInfo();
                        }
                        thumbnail.setSource(new juce::FileInputSource(file));
                    }
                    break;
                }
                else if (button == playlist_delete_buttons[i]) {
                    delete_button(i);
                    player.delete_button(i);
                    updateTrackInfo();
                    resized();
                    repaint();
                    break;
                }
            }
            for (int i = 0; i < markers.size(); i++) {
                if (button == marker_buttons[i]) {
                    double ratio = markers[i]->get_pos()/player.getOriginalLength();
                    double new_pos = ratio*player.getLength();
                    player.setPosition(new_pos);
                    break;
                }
                else if (button == marker_delete_buttons[i]) {
                    delete_marker(i);
                    resized();
                    repaint();
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
                auto event = juce::Desktop::getInstance().getMainMouseSource();
                if (event.getCurrentModifiers().isLeftButtonDown())
                {
                    double new_position = current_value / player.getOriginalLength() * player.getLength();
                    player.setPosition(new_position);
                }
                if (event.getCurrentModifiers().isRightButtonDown())
                {
                    add_marker(current_value, 1);
                }
            }
        }
        else if (slider == &low_eq) {
            float low = low_eq.getValue();
            low_eq_value = low;
            player.updateFilters(low_eq_value, mid_eq_value, high_eq_value);
        }
        else if (slider == &mid_eq) {
            float mid = mid_eq.getValue();
            mid_eq_value = mid;
            player.updateFilters(low_eq_value, mid_eq_value, high_eq_value);
        }
        else if (slider == &high_eq) {
            float high = high_eq.getValue();
            high_eq_value = high;
            player.updateFilters(low_eq_value, mid_eq_value, high_eq_value);
        }
        else if (slider == &reverbSlider) {
            if (!current_value) {
                player.updateReverb(0.0, 0.0, 0.0, 1.0, 0.0);
            }
            else {
                float roomSizeValue = current_value / 100.0f;
                float wetLevel = roomSizeValue * 0.33f;
                player.updateReverb(roomSizeValue, 0.5f, wetLevel, 0.8f, 1.0f);
            }
        }
    }

    void PlayerGUI::updateTrackInfo()
    {
        titleLabel.setText((player.isWokring() ? player.getTitle() : ""), juce::dontSendNotification);
        artistLabel.setText((player.isWokring() ? player.getArtist() : ""), juce::dontSendNotification);
    }

    void PlayerGUI::changeListenerCallback(juce::ChangeBroadcaster* source) {
        if (source == &thumbnail) {
            repaint();
        }
    }

    void PlayerGUI::timerCallback()
    {
        panelWidth = 0.3 * getWidth();
        panelHeight = 0.15 * getHeight();

        if (player.getIndex() == -1) {
            positionSlider.setRange(0.0, 0.0, juce::dontSendNotification);
            positionSlider.setValue(0.0, juce::dontSendNotification);
            return;
        }
        double truelengthInSeconds = player.getOriginalLength();

        if (!positionSlider.isMouseButtonDown())
        {
            if (truelengthInSeconds > 0)
            {
                double ratio = truelengthInSeconds / player.getLength();

                positionSlider.setRange(0.0, truelengthInSeconds, juce::dontSendNotification);
                positionSlider.setValue(ratio * player.getPosition(), juce::dontSendNotification);
            }
            else {
                positionSlider.setValue(0.0, juce::dontSendNotification);
            }
        }
        repaint();
    }