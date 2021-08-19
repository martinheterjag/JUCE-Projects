/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EasyverbAudioProcessorEditor::EasyverbAudioProcessorEditor (EasyverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), info_button_(juce::Colours::darkgrey)
{
    constexpr int TEXT_BOX_SIZE = 25;

    reverb_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    reverb_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    reverb_slider_.addListener(this);
    addAndMakeVisible(reverb_slider_);

    reverb_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "REVERB", reverb_slider_);

    mix_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mix_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    mix_slider_.addListener(this);
    addAndMakeVisible(mix_slider_);

    mix_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "MIX", mix_slider_);

    info_button_.addToEditor(this);
    setSize(400, 300);
}

EasyverbAudioProcessorEditor::~EasyverbAudioProcessorEditor()
{
}

//==============================================================================
void EasyverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour(0xff123456));
    g.setColour(juce::Colours::azure);
    g.fillRect(top_section_);

    
    g.setColour (juce::Colour(0xff123456));
    g.setFont(40.0f);
    g.drawFittedText("EASYVERB", getLocalBounds(), juce::Justification::centredTop, 1);

    g.setColour(juce::Colours::azure);
    SetupSections();
    g.setFont(18.0f);
    g.drawFittedText("REVERB", reverb_text_section_, juce::Justification::left, 1);
    g.drawFittedText("DRY/WET", mix_text_section_, juce::Justification::left, 1);
}

void EasyverbAudioProcessorEditor::resized()
{
    info_button_.button.setBounds(getWidth() - 35, 10, 20, 20);
    info_button_.info_text.setBounds(30, 50, getWidth() - 60, getHeight() - 100);
    SetupSections();
    reverb_slider_.setBounds(reverb_section_);
    mix_slider_.setBounds(mix_section_);
}

void EasyverbAudioProcessorEditor::SetupSections()
{
    juce::Rectangle<int> r = getLocalBounds();
    top_section_ = r.removeFromTop(50);
    r.removeFromTop(50);
    r.removeFromBottom(60);
    r.removeFromLeft(100);
    r.removeFromRight(100);

    constexpr int text_section_width = 70;

    juce::Rectangle<int> interface_section = r;
    int section_height = interface_section.getHeight() / 2;

    reverb_section_ = interface_section.removeFromTop(section_height);
    reverb_text_section_ = reverb_section_.removeFromLeft(text_section_width);
    
    mix_section_ = interface_section;
    mix_text_section_ = mix_section_.removeFromLeft(text_section_width);
}

void EasyverbAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    repaint();
}