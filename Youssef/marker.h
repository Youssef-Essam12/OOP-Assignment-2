#pragma once
#include <JuceHeader.h>
class marker : public juce::Component {
public:
	marker(double slider_x, double slider_y, double of, double p, double l) : pos(p), offset(of), length(l) {
		this->setBounds(slider_x + of, slider_y, 10, 10);
		++marker_cnt;
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
	double get_pos() {
		return this->pos;
	}
	double get_length() {
		return this->length;
	}
	static int get_marker_cnt() {
		return marker_cnt;
	}
	static void set_marker_cnt(int c) {
		marker_cnt = c;
	}
	std::function<void(double, double)> onClick;
private:
	static int marker_cnt;
	double offset, pos, length;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(marker);
};
