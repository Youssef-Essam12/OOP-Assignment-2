#include "MarkerEntry.h"

MarkerEntry::MarkerEntry(const juce::String& t, const juce::String& a, int i, double marker_time)
    : title(t), timeText(a), index(i), time(marker_time)
{
    titleLabel.reset(new juce::Label("Title", title));
    titleLabel->setFont(juce::Font(16.0f, juce::Font::bold));
    titleLabel->setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel->setJustificationType(juce::Justification::centredLeft);

    // Allow double-click editing and intercept clicks
    titleLabel->setEditable(false, true, false);
    titleLabel->setInterceptsMouseClicks(true, true);
    titleLabel->addListener(this);
    addAndMakeVisible(*titleLabel);

    timeLabel.reset(new juce::Label("Time", timeText));
    timeLabel->setFont(juce::Font(14.0f));
    timeLabel->setColour(juce::Label::textColourId, juce::Colour(0xffaaaaaa)); // Slightly brighter muted text
    timeLabel->setJustificationType(juce::Justification::centredLeft);
    timeLabel->setInterceptsMouseClicks(false, false);
    titleLabel->setAlwaysOnTop(1);
    addAndMakeVisible(*timeLabel);

    deleteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffc0392b)); // Modern Red (Alizarin Crimson)
    deleteButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    deleteButton.setButtonText("X");
    deleteButton.addListener(this);
    addAndMakeVisible(&deleteButton);

    setOpaque(true);
}

MarkerEntry::~MarkerEntry()
{
    deleteButton.removeListener(this);
}

void MarkerEntry::paint(juce::Graphics& g)
{
    // Changed to modern dark gray entry background (replaces entryColour)
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(getLocalBounds());

    // Changed to a subtle border (replaces backgroundColour)
    g.setColour(juce::Colour(0xff1c1c1c).withAlpha(0.8f));
    g.drawRect(getLocalBounds(), 1);
}

void MarkerEntry::resized()
{
    auto bounds = getLocalBounds();
    const int margin = 10;
    const int padding = 5;
    const int deleteButtonWidth = 40;

    const int contentHeight = bounds.getHeight() - 2 * padding;

    deleteButton.setBounds(
        bounds.getWidth() - deleteButtonWidth - margin,
        padding,
        deleteButtonWidth,
        contentHeight
    );

    auto contentArea = bounds.withRight(deleteButton.getX()).reduced(margin, padding);

    int combinedContentWidth = contentArea.getWidth();

    int titleWidth = (combinedContentWidth * 4) / 10;
    int authorWidth = combinedContentWidth - titleWidth;

    titleLabel->setBounds(
        contentArea.getX(),
        contentArea.getY(),
        titleWidth,
        contentArea.getHeight()
    );

    timeLabel->setBounds(
        titleLabel->getRight(),
        contentArea.getY(),
        authorWidth,
        contentArea.getHeight()
    );
}

int MarkerEntry::getIndex()
{
    return index;
}

void MarkerEntry::setIndex(int i)
{
    index = i;
}

double MarkerEntry::get_pos()
{
    return time;
}

juce::String MarkerEntry::get_title()
{
    return title;
}

void MarkerEntry::buttonClicked(juce::Button* button)
{
    if (button == &deleteButton) {
        onDeleteClick(index);
    }
}

void MarkerEntry::labelTextChanged(juce::Label* label)
{
    if (label == titleLabel.get()) {
        title = label->getText();
    }
}


void MarkerEntry::mouseDown(const juce::MouseEvent& event)
{
    //onClick(index);
    if (!titleLabel->getBounds().contains(event.getPosition()))
    {
        onClick(index);
    }
}
