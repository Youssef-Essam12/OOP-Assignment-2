#pragma once
#include <JuceHeader.h>
#include <functional>

class PlaylistEntry : public juce::Component, juce::Button::Listener {
public:
	PlaylistEntry(const juce::String& t, const juce::String& a, int i);
	~PlaylistEntry() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	int getIndex();
	void setIndex(int i);

	void buttonClicked(juce::Button* button);
	void mouseDown(const juce::MouseEvent& event) override;

	std::function<void(int)> onClick;
	std::function<void(int)> onDeleteClick;
private:
	int index;
	std::unique_ptr<juce::Label> titleLabel;
	std::unique_ptr<juce::Label> authorLabel;
	juce::TextButton deleteButton;

	juce::String title, author;

	juce::Colour backgroundColour = juce::Colour(0xff303337); // Dark background for the overall view
	juce::Colour entryColour = juce::Colour(0xff44474b); // Slightly lighter background for the entry
	juce::Colour textColour = juce::Colours::white;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistEntry);
};
