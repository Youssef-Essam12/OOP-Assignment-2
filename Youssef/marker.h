#pragma once
#include <JuceHeader.h>
class marker : public juce::Component {
public:
	marker(double slider_x, double slider_y, double of, double p, double l) : pos(p), offset(of), length(l) {
		this->setBounds(slider_x + of, slider_y, 10, 10);
	}
	void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        float size = juce::jmin(bounds.getWidth(), bounds.getHeight());
        float cornerRadius = size / 2.0f;
        g.setColour(juce::Colours::red);
        g.fillRoundedRectangle(bounds, cornerRadius);
	}
	void mouseDown(const juce::MouseEvent& event) override {
		onClick(pos, length);
	}
	std::function<void(double, double)> onClick;
	void set_pos_length(double p, double l) {
		pos = p;
		length = l;
	}
private:
	double offset, pos, length;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(marker);
};
