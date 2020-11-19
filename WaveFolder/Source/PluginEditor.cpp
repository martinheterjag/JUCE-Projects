/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveFolderAudioProcessorEditor::WaveFolderAudioProcessorEditor (WaveFolderAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    constexpr int TEXT_BOX_SIZE = 25;

    gain_slider_.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    gain_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    addAndMakeVisible(gain_slider_);

    gain_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "GAIN", gain_slider_);

    threshold_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    threshold_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    addAndMakeVisible(threshold_slider_);

    threshold_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "THRESHOLD", threshold_slider_);

    bias_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    bias_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    addAndMakeVisible(bias_slider_);

    bias_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "BIAS", bias_slider_);

    volume_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    volume_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    addAndMakeVisible(volume_slider_);

    volume_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "VOLUME", volume_slider_);

    thr_lfo_rate_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    thr_lfo_rate_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    addAndMakeVisible(thr_lfo_rate_slider_);

    thr_lfo_rate_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "THR_LFO_RATE", thr_lfo_rate_slider_);

    thr_lfo_depth_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    thr_lfo_depth_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    addAndMakeVisible(thr_lfo_depth_slider_);

    thr_lfo_depth_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "THR_LFO_DEPTH", thr_lfo_depth_slider_);

    setSize (600, 200);
    DivideSections();
}

WaveFolderAudioProcessorEditor::~WaveFolderAudioProcessorEditor()
{
}

//==============================================================================
void WaveFolderAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    g.setColour(juce::Colours::powderblue);
    g.fillRect(gain_section_);

    g.setColour(juce::Colours::lightgreen);
    g.fillRect(threshold_section_);

    g.setColour(juce::Colours::lemonchiffon);
    g.fillRect(bias_section_);

    g.setColour(juce::Colours::peachpuff);
    g.fillRect(volume_section_);

    g.setColour (juce::Colours::white);
    g.setFont (40.0f);
    g.drawFittedText("WAVE FOLDER TEST", top_section_, juce::Justification::centred, 1);

    g.setColour(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setFont(20.0f);
    g.drawFittedText("GAIN", gain_section_, juce::Justification::centredTop, 1);
    g.drawFittedText("THRESHOLD", threshold_section_, juce::Justification::centredTop, 1);
    g.drawFittedText("BIAS", bias_section_, juce::Justification::centredTop, 1);
    g.drawFittedText("VOLUME", volume_section_, juce::Justification::centredTop, 1);
}

void WaveFolderAudioProcessorEditor::resized()
{
    DivideSections();
    constexpr int KNOB_SECTION_REDUCTION = 30;
    gain_slider_.setBounds(gain_section_.reduced(KNOB_SECTION_REDUCTION));
    threshold_slider_.setBounds(threshold_section_.reduced(KNOB_SECTION_REDUCTION));
    bias_slider_.setBounds(bias_section_.reduced(KNOB_SECTION_REDUCTION));
    volume_slider_.setBounds(volume_section_.reduced(KNOB_SECTION_REDUCTION));
    thr_lfo_rate_slider_.setBounds(thr_lfo_rate_section_);
    thr_lfo_depth_slider_.setBounds(thr_lfo_depth_section_);
}

void WaveFolderAudioProcessorEditor::DivideSections() {
    juce::Rectangle<int> r = getLocalBounds();

    top_section_ = r.removeFromTop(50);
    gain_section_ = r.removeFromLeft(150);
    threshold_section_ = r.removeFromLeft(150);
    bias_section_ = r.removeFromLeft(150);
    volume_section_ = r.removeFromLeft(150);

    // Cut bottom of a copy from threshold section to make room for lfo knobs
    r = threshold_section_;
    r.removeFromTop(100);
    thr_lfo_rate_section_ = r.removeFromLeft(75);
    thr_lfo_depth_section_ = r.removeFromLeft(75);
}
