/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

constexpr float LINE_THICKNESS = 4.0f;

//==============================================================================
GramophonyAudioProcessorEditor::GramophonyAudioProcessorEditor (GramophonyAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), info_button_ (juce::Colours::darkgrey)
{
    constexpr int TEXT_BOX_SIZE = 25;

    compress_slider_.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    compress_slider_.setTextBoxStyle (juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    compress_slider_.addListener (this);
    addAndMakeVisible (compress_slider_);

    compress_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "COMPRESS", compress_slider_);

    tone_slider_.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    tone_slider_.setTextBoxStyle (juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    tone_slider_.addListener (this);
    addAndMakeVisible (tone_slider_);

    tone_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "TONE", tone_slider_);

    vibrato_slider_.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    vibrato_slider_.setTextBoxStyle (juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    vibrato_slider_.addListener (this);
    addAndMakeVisible (vibrato_slider_);

    vibrato_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "VIBRATO", vibrato_slider_);

    vibrato_rate_slider_.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    vibrato_rate_slider_.setTextBoxStyle (juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    vibrato_rate_slider_.addListener (this);
    addAndMakeVisible (vibrato_rate_slider_);

    vibrato_rate_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "VIBRATO_RATE", vibrato_rate_slider_);

    mix_slider_.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mix_slider_.setTextBoxStyle (juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    mix_slider_.addListener (this);
    addAndMakeVisible (mix_slider_);

    mix_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "MIX", mix_slider_);

    info_button_.addToEditor (this);
    setSize (500, 300);
}

GramophonyAudioProcessorEditor::~GramophonyAudioProcessorEditor()
{
}

//==============================================================================
void GramophonyAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::beige);

    g.setColour (juce::Colour (0xff123456));
    g.setFont (40.0f);
    g.drawFittedText ("GRAMOPHONY", getLocalBounds(), juce::Justification::centredTop, 1);

    // Gramophone
    juce::Path funnel;
    funnel.startNewSubPath (8.0f, 102.0f);
    funnel.lineTo (43.0f, 48.0f);
    funnel.lineTo (113.0f, 38.0f);
    funnel.lineTo (194.0f, 55.0);
    funnel.lineTo (258.0f, 109.0f);
    funnel.lineTo (279.0f, 171.0f);
    funnel.lineTo (252.0f, 225.0f);
    funnel.lineTo (174.0f, 245.0f);
    funnel.lineTo (92.0f, 229.0f);
    funnel.lineTo (25.0f, 166.0f);
    funnel.closeSubPath();

    juce::Path base;
    base.startNewSubPath (74.0f, 210.0f);
    base.lineTo (54.0f, 254.0f);
    base.lineTo (53.0f, 275.0f);
    base.lineTo (0.0F, 300.0f);
    base.lineTo (32.0F, 300.0f);
    base.lineTo (73.0f, 285.0f);
    base.lineTo (79.0f, 260.0f);
    base.lineTo (104.0F, 230.0f);
    base.closeSubPath();

    g.setColour (juce::Colour (0xff123456));
    g.fillPath (base);

    // Same as background to hide parts of base when funnel is painted with alpha
    g.setColour (juce::Colours::beige);
    g.fillPath (funnel);

    g.setColour (juce::Colours::orange.withAlpha (0.2f + sliderToAplhaValue (tone_slider_) / 2.0f));
    g.fillPath (funnel);

    g.setColour (juce::Colours::yellow.withAlpha (sliderToAplhaValue (compress_slider_) / 2.0f));
    g.fillPath (funnel);

    g.setColour (juce::Colours::blueviolet.withAlpha (sliderToAplhaValue (vibrato_slider_) / 4.0f));
    g.fillPath (funnel);

    g.setColour (juce::Colours::rebeccapurple.withAlpha (sliderToAplhaValue (vibrato_rate_slider_) / 3.0f));
    g.fillPath (funnel);

    g.setColour (juce::Colours::orangered.withAlpha (sliderToAplhaValue (mix_slider_) / 3.0f));
    g.fillPath (funnel);

    // Draw contours
    g.setColour (juce::Colour (0xff123456));
    DrawThreePointLine (g, 73.0f, 179.0f, 62.0f, 144.0f, 8.0f, 102.0f);
    DrawThreePointLine (g, 85.0f, 189.0f, 83.0f, 133.0f, 43.0f, 48.0f);
    DrawThreePointLine (g, 92.0f, 193.0f, 105.0f, 129.0f, 113.0f, 38.0f);
    DrawThreePointLine (g, 101.0f, 201.0f, 144.0f, 126.0f, 194.0f, 55.0f);
    DrawThreePointLine (g, 108.0f, 203.0f, 176.0f, 145.0f, 258.0f, 109.0f);
    DrawThreePointLine (g, 122.0f, 209.0f, 188.0f, 179.0f, 279.0f, 171.0f);
    DrawThreePointLine (g, 136.0f, 211.0f, 174.0f, 202.0f, 252.0f, 225.0f);
    DrawThreePointLine (g, 108.0f, 203.0f, 144.0f, 215.0f, 174.0f, 245.0f);
    DrawThreePointLine (g, 108.0f, 203.0f, 73.0f, 179.0f, 25.0f, 166.0f);
    g.drawLine (juce::Line<float> (108.0f, 203.0f, 92.0f, 229.0f), LINE_THICKNESS);

    SetupSections();
    g.setFont (18.0f);
    g.drawFittedText ("COMP", comp_text_section_, juce::Justification::left, 1);
    g.drawFittedText ("TONE", tone_text_section_, juce::Justification::left, 1);
    g.drawFittedText ("VIBE", vibrato_text_section_, juce::Justification::left, 1);
    g.drawFittedText ("DRY", mix_text_section_, juce::Justification::left, 1);
}

float GramophonyAudioProcessorEditor::sliderToAplhaValue (juce::Slider& slider)
{
    double range = (slider.getMaximum() - slider.getMinimum());
    return static_cast<float>((slider.getValue() - slider.getMinimum()) / range);
}

void GramophonyAudioProcessorEditor::DrawThreePointLine (juce::Graphics& g,
                                                         float x1,
                                                         float y1,
                                                         float x2,
                                                         float y2,
                                                         float x3,
                                                         float y3)
{
    g.drawLine (juce::Line<float> (x1, y1, x2, y2), LINE_THICKNESS);
    g.drawLine (juce::Line<float> (x2, y2, x3, y3), LINE_THICKNESS);
}

void GramophonyAudioProcessorEditor::resized()
{
    info_button_.button.setBounds (getWidth() - 35, 10, 20, 20);
    info_button_.info_text.setBounds (30, 50, getWidth() - 60, getHeight() - 100);
    SetupSections();
    compress_slider_.setBounds (comp_section_);
    tone_slider_.setBounds (tone_section_);
    vibrato_slider_.setBounds (vibrato_section_);
    vibrato_rate_slider_.setBounds (vibrato_rate_section_);
    mix_slider_.setBounds (mix_section_);
}

void GramophonyAudioProcessorEditor::SetupSections()
{
    juce::Rectangle<int> r = getLocalBounds();
    top_section_ = r.removeFromTop (50);
    picture_section_ = r.removeFromLeft (310);

    juce::Rectangle<int> interface_section = r;
    int section_height = interface_section.getHeight() / 4;
    constexpr int text_section_width = 40;

    comp_section_ = interface_section.removeFromTop (section_height);
    comp_text_section_ = comp_section_.removeFromLeft (text_section_width);

    tone_section_ = interface_section.removeFromTop (section_height);
    tone_text_section_ = tone_section_.removeFromLeft (text_section_width);

    vibrato_section_ = interface_section.removeFromTop (section_height);
    vibrato_text_section_ = vibrato_section_.removeFromLeft (text_section_width);
    vibrato_rate_section_ = vibrato_section_.removeFromRight (vibrato_section_.getWidth() / 2);

    mix_section_ = interface_section;
    mix_text_section_ = mix_section_.removeFromLeft (text_section_width);
}

void GramophonyAudioProcessorEditor::sliderValueChanged (juce::Slider* /*slider*/)
{
    repaint();
}
