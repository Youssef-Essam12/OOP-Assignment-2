#pragma once
#include <JuceHeader.h>
#include <functional> // Ensure functional is included for std::function

class Marker : public juce::Component {
public:
	// Constructor and Destructor
	Marker(double slider_x, double slider_y, double of, double p, double l);
	~Marker() override; // Added override for destructor

	// Virtual JUCE Component methods
	void paint(juce::Graphics& g) override;
	void mouseDown(const juce::MouseEvent& event) override;

	// Public API methods
	double get_pos();
	double get_length();

	// Click handler (Function object)
	std::function<void(double)> onClick;

private:

	// Marker data
	double offset, pos, length;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Marker);
};