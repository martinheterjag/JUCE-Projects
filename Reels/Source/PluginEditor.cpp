/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReelsAudioProcessorEditor::ReelsAudioProcessorEditor (ReelsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    constexpr int TEXT_BOX_SIZE = 25;
    grain_size_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    grain_size_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    addAndMakeVisible(grain_size_slider_);

    grain_size_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "GRAIN_SIZE", grain_size_slider_);

    setSize (400, 300);
}

ReelsAudioProcessorEditor::~ReelsAudioProcessorEditor()
{
}

//==============================================================================
void ReelsAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void ReelsAudioProcessorEditor::resized()
{
    grain_size_slider_.setBounds(getLocalBounds().reduced(20));
}
