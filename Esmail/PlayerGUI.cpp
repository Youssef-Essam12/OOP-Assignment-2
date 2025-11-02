#pragma once
#include "PlayerGUI.h"
#include "MainComponent.h"

#include "GUI Components/TopBarComp.h"
#include "GUI Components/NormalViewComp.h"
#include "GUI Components/LeftNavComp.h"
#include "GUI Components/BottomControlComp.h"
#include "GUI Components/PlaylistViewComp.h"
#include "GUI Components/MinimizedViewComp.h"
#include "GUI Components/MixerViewComp.h"
#include "Extra Functionalities/Marker.h"
#include "Extra Functionalities/FloatingVolumeSlider.h"



PlayerGUI::PlayerGUI(PlayerAudio& audio_player, PlayerAudio& mixer_player_1, PlayerAudio& mixer_player_2)
    : audio_player(audio_player),
    mixer_player_1(mixer_player_1),
    mixer_player_2(mixer_player_2)
{

    topBar = std::make_unique<TopBarComp>(audio_player);
    addAndMakeVisible(topBar.get());

    topBar->onFileLoaded = [&](const juce::File& file) {
        playlistView->add_playlist_entry(file);
    };

    miniView = std::make_unique<MinimizedViewComp>(audio_player);
    addAndMakeVisible(miniView.get());

    normalView = std::make_unique<NormalViewComp>(audio_player);
    addAndMakeVisible(normalView.get());

    playlistView = std::make_unique<PlaylistViewComp>(audio_player);
    addAndMakeVisible(playlistView.get());

    playlistView->onAudioSelected= [&](int index) {
        audio_player.playFile(index);
        normalView->update(audio_player.getPlaylistFile(index));
    };

    navBar = std::make_unique<LeftNavComp>();
    addAndMakeVisible(navBar.get());

    navBar->set_view = [&](int view_index) {
        this->setView(viewArr[view_index]);
    };

    controlBar = std::make_unique<BottomControlComp>(audio_player);
    addAndMakeVisible(controlBar.get());

    mixerView = std::make_unique<MixerViewComp>(mixer_player_1, mixer_player_2);
    addAndMakeVisible(mixerView.get());

    volume_slider = std::make_unique<FloatingVolumeSlider>(audio_player);
    addAndMakeVisible(volume_slider.get());
    volume_slider->toFront(true);

    if (auto* topLevel = getTopLevelComponent())
    {
        topLevel->addComponentListener(this);
    }

    normalView->setVisible(false);
    playlistView->setVisible(false);
    // editorView->setVisible(false);
    miniView->setVisible(false);

    setView(View::Normal);

    startTimerHz(30);
}

PlayerGUI::~PlayerGUI()
{
    stopTimer();

    if (auto* topLevel = getTopLevelComponent())
    {
        topLevel->removeComponentListener(this);
    }
}

double PlayerGUI::getMixPercentage() {
    return mixerView->getPercentage();
}

void PlayerGUI::handleMinimisedStateChange(bool isMinimised) {
    if (isMinimised)
    {
        setView(View::Minimized);
    }
    else
    {
        setView(View::Normal);
    }
}


void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}



void PlayerGUI::resized()
{
    auto bounds = getLocalBounds();

    int topBarHeight = 0.05 * getWidth();
    int leftBarWidth = 0.1 * getWidth();
    int bottomBarHeight = 0.05 * getWidth();

    int sliderWidth = 30;
    int sliderHeight = 500; 

    int xPos = bounds.getRight() - 40; 
    int yPos = bounds.getCentreY() - (sliderHeight / 2); 

    volume_slider->setBounds(xPos, yPos, sliderWidth, sliderHeight);

    if (currentView == View::Minimized)
    {
        miniView->setBounds(bounds);
    }
    else if (currentView == View::Mixer)
    {
        navBar->setBounds(bounds.removeFromLeft(leftBarWidth));
        mixerView->setBounds(bounds); 
    }
    else 
    {
        navBar->setBounds(bounds.removeFromLeft(leftBarWidth));
        topBar->setBounds(bounds.removeFromTop(topBarHeight));
        controlBar->setBounds(bounds.removeFromBottom(bottomBarHeight));

        switch (currentView)
        {
        case View::Normal:
            normalView->setBounds(bounds);
            break;
        case View::Playlist:
            playlistView->setBounds(bounds);
            break;
        case View::Editor:
            // editorView->setBounds(bounds);
            break;
        }
    }
}

void PlayerGUI::setView(View newView)
{

    auto* mainComp = findParentComponentOfClass<MainComponent>();
    if (mainComp == nullptr)
    {
        return;
    }

    normalView->setVisible(false);
    playlistView->setVisible(false);
    miniView->setVisible(false);
    mixerView->setVisible(false);
    // editorView->setVisible(false);

    topBar->setVisible(false);
    navBar->setVisible(false);
    controlBar->setVisible(false);

    volume_slider->setVisible(true);

    if (currentView == View::Mixer && newView != View::Mixer) // leaving mixer
    {
        mixer_player_1.stop();
        mixer_player_2.stop();
        mainComp->setAudioMode(MainComponent::AudioMode::Normal);
    }

    currentView = newView;

    if (currentView == View::Minimized)
    {
        miniView->setVisible(true);
    }
    else if (currentView == View::Mixer)
    {
        navBar->setVisible(true);
        mixerView->setVisible(true);
        audio_player.stop();
        volume_slider->setVisible(false);
        mainComp->setAudioMode(MainComponent::AudioMode::Mixer);
    }
    else 
    {
        topBar->setVisible(true);
        navBar->setVisible(true);
        controlBar->setVisible(true);

        switch (currentView)
        {
        case View::Normal:
            normalView->setVisible(true);
            break;
        case View::Playlist:
            playlistView->setVisible(true);
            break;
        case View::Editor:
            // editorView->setVisible(true);
            break;
        }
    }

    resized();
    repaint();
}

void PlayerGUI::timerCallback()
{
    // This timer now just updates the transport bar
    if (audio_player.isWokring())
    {
        controlBar->update();
        normalView->update(audio_player.getPlaylistFile(audio_player.getIndex()));
    }
}

double PlayerAudio::getGain() const {
    return transportSource.getGain();
}
