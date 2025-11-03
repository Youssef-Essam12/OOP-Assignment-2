#include "Marker.h"


Marker::Marker(double slider_x, double slider_y, double of, double p, double l) : pos(p), offset(of), length(l) {
	this->setBounds(slider_x + of, slider_y, 10, 10);
}

Marker::~Marker() {
}

void Marker::paint(juce::Graphics& g) {
	auto bounds = getLocalBounds().toFloat();
	float size = juce::jmin(bounds.getWidth(), bounds.getHeight());
	float cornerRadius = size / 2.0f;
	g.setColour(juce::Colours::red);
	g.fillRoundedRectangle(bounds, cornerRadius);
}

void Marker::mouseDown(const juce::MouseEvent& event) {
	onClick(pos);
}

double Marker::get_pos() {
	return this->pos;
}
double Marker::get_length() {
	return this->length;
}
