#pragma once
#include <JuceHeader.h>
class Marker : public juce::Component {
public:
	Marker(double slider_x, double slider_y, double of, double p, double l) : pos(p), offset(of), length(l) {
		this->setBounds(slider_x + of, slider_y, 10, 10);
		++Marker_cnt;
	}
	~Marker() {
		--Marker_cnt;
	}
	void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        float size = juce::jmin(bounds.getWidth(), bounds.getHeight());
        float cornerRadius = size / 2.0f;
        g.setColour(juce::Colours::red);
        g.fillRoundedRectangle(bounds, cornerRadius);
	}
	void mouseDown(const juce::MouseEvent& event) override {
		onClick(pos);
	}
	double get_pos() {
		return this->pos;
	}
	double get_length() {
		return this->length;
	}
	static int get_Marker_cnt() {
		return Marker_cnt;
	}
	static void set_Marker_cnt(int c) {
		Marker_cnt = c;
	}
	std::function<void(double)> onClick;
private:
	static int Marker_cnt;
	double offset, pos, length;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Marker);
};