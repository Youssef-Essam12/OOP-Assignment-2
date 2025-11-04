#include <JuceHeader.h>
#include "MainComponent.h"

// Our application class
class SimpleAudioPlayer : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return "Vibe"; }
    const juce::String getApplicationVersion() override { return "1.0"; }

    void initialise(const juce::String&) override
    {
        // Create and show the main window
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr; // Clean up
    }

private:
    // The main window of the app
    class MainWindow : public juce::DocumentWindow,
        public juce::ComponentListener // Added inheritance from finalApp
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                juce::Colours::lightgrey,
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);

            mainComponentPtr = new MainComponent(); // Initialize pointer

            setContentOwned(mainComponentPtr, true); // MainComponent = our UI + logic
            centreWithSize(400, 200);
            setVisible(true);
            addComponentListener(this); // Start listening to component events (from finalApp)
        }

        ~MainWindow() override
        {
            removeComponentListener(this); // Stop listening (from finalApp)
        }

        // Implementation of juce::ComponentListener's virtual functions (from finalApp)
        void componentVisibilityChanged(juce::Component& component) override
        {
            if (&component == this)
            {
                // Note: !isVisible() correctly checks for the minimized state
                bool isMinimised = !isVisible();

                if (mainComponentPtr != nullptr)
                {
                    // Access the PlayerGUI and notify it of the minimization state change
                    if (auto* gui = mainComponentPtr->getGUI())
                    {
                        gui->handleMinimisedStateChange(isMinimised);
                    }
                }
            }
        }

        void componentMovedOrResized(juce::Component& component, bool wasMoved, bool wasResized) override
        {
            // Implementation for move/resize logic if needed, but empty here.
            juce::ignoreUnused(component, wasMoved, wasResized);
        }

        void closeButtonPressed() override
        {
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        MainComponent* mainComponentPtr = nullptr; // Pointer to MainComponent
    };

    std::unique_ptr<MainWindow> mainWindow;
};

// This macro starts the app
START_JUCE_APPLICATION(SimpleAudioPlayer)