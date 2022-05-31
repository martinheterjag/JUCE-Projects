#pragma once

#include <JuceHeader.h>

// Use the info button class by
// 1) Create an InfoButton object
// 2) Make the button visible by  using the function addToEditor(this) from PluginEditor class
// 3) In resized() function set boundry of the public data members button and info_text
class InfoButton : public juce::Button::Listener
{
public:
    explicit InfoButton (juce::Colour colour);
    ~InfoButton();
    void addToEditor (juce::AudioProcessorEditor* editor);
    void buttonStateChanged (juce::Button* b) override;
    void buttonClicked (juce::Button* button) override;
    juce::TextButton button;
    juce::Label info_text;
};
