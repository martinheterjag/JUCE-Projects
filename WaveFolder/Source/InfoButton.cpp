#include "InfoButton.h"

InfoButton::InfoButton(juce::Colour colour)
{
    button.setButtonText("i");
    button.addListener(this);

    info_text.setColour(juce::Label::backgroundColourId, colour);
    info_text.setColour(juce::Label::outlineColourId, juce::Colours::white);
    info_text.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    info_text.setJustificationType(juce::Justification::centredLeft);
    info_text.setBorderSize(juce::BorderSize<int>(20, 70, 20, 70));

    std::string info_string = "";
    info_string += ProjectInfo::companyName + std::string(" ") + ProjectInfo::projectName + std::string(" version ") + ProjectInfo::versionString + std::string("\n\n");
    info_string += std::string("Copyright (C) Martin Eriksson, \nSource code licenced under MIT license\n\n");
    info_string += std::string("Check out my other projects at ") + JucePlugin_ManufacturerWebsite + std::string("\n");

    info_text.setText(info_string, juce::dontSendNotification);
}

InfoButton::~InfoButton() {}

void InfoButton::addToEditor(juce::AudioProcessorEditor* editor) {
    editor->addAndMakeVisible(info_text);
    info_text.setVisible(false);
    editor->addAndMakeVisible(button);
}

void InfoButton::buttonStateChanged(juce::Button* b)
{
    if (b == &button) {
        if (button.isOver()) {
            info_text.setVisible(true);
        }
        else {
            info_text.setVisible(false);
        }
    }
}

void InfoButton::buttonClicked(juce::Button* button) {
}

