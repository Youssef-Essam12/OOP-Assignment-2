#include "PlaylistEntry.h"

PlaylistEntry::PlaylistEntry(const juce::String& t, const juce::String& a, int i)
    : title(t), author(a), index(i)
{
    titleLabel.reset(new juce::Label("Title", title));
    titleLabel->setFont(juce::Font(16.0f, juce::Font::bold));
    titleLabel->setColour(juce::Label::textColourId, juce::Colours::white); // Modern white text
    titleLabel->setJustificationType(juce::Justification::centredLeft);
    titleLabel->setInterceptsMouseClicks(false, false);
    addAndMakeVisible(*titleLabel);

    authorLabel.reset(new juce::Label("Author", author));
    authorLabel->setFont(juce::Font(14.0f));
    authorLabel->setColour(juce::Label::textColourId, juce::Colour(0xffaaaaaa)); // Slightly brighter muted text
    authorLabel->setJustificationType(juce::Justification::centredLeft);
    authorLabel->setInterceptsMouseClicks(false, false);
    addAndMakeVisible(*authorLabel);

    deleteButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffc0392b)); // Modern Red (Alizarin Crimson)
    deleteButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    deleteButton.setButtonText("X");
    deleteButton.addListener(this);
    addAndMakeVisible(&deleteButton);

    setOpaque(true);
}

PlaylistEntry::~PlaylistEntry()
{
    deleteButton.removeListener(this);
}

void PlaylistEntry::paint(juce::Graphics& g)
{
    // Changed to modern dark gray entry background (replaces entryColour)
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(getLocalBounds());

    // Changed to a subtle border (replaces backgroundColour)
    g.setColour(juce::Colour(0xff1c1c1c).withAlpha(0.8f));
    g.drawRect(getLocalBounds(), 1);
}

void PlaylistEntry::resized()
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

    authorLabel->setBounds(
        titleLabel->getRight(),
        contentArea.getY(),
        authorWidth,
        contentArea.getHeight()
    );
}

int PlaylistEntry::getIndex()
{
    return index;
}

void PlaylistEntry::setIndex(int i)
{
    index = i;
}

void PlaylistEntry::buttonClicked(juce::Button* button)
{
    if (button == &deleteButton) {
        onDeleteClick(index);
    }
}

void PlaylistEntry::mouseDown(const juce::MouseEvent& event)
{
    onClick(index);
}
