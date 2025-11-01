#pragma once
#include <JuceHeader.h>
#include <functional>

class MarkerEntry : public juce::Component, juce::Button::Listener {
public:
	MarkerEntry(const juce::String& t, const juce::String& a, int i, double marker_time);
	~MarkerEntry() override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	int getIndex();
	void setIndex(int i);

	double get_pos();

	void buttonClicked(juce::Button* button);
	void mouseDown(const juce::MouseEvent& event) override;

	static int get_marker_cnt();

	std::function<void(int)> onClick;
	std::function<void(int)> onDeleteClick;
private:
	int index;
	std::unique_ptr<juce::Label> titleLabel;
	std::unique_ptr<juce::Label> timeLabel;
	juce::TextButton deleteButton;

	juce::String title, timeText;
	double time;

	juce::Colour backgroundColour = juce::Colour(0xff303337); // Dark background for the overall view
	juce::Colour entryColour = juce::Colour(0xff44474b); // Slightly lighter background for the entry
	juce::Colour textColour = juce::Colours::white;

	static int cnt;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerEntry);
};
