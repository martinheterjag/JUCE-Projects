/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "AnimatedTriangle.h"
#include "PluginProcessor.h"

//==============================================================================
EasyverbAudioProcessorEditor::EasyverbAudioProcessorEditor (EasyverbAudioProcessor& p)
    : AudioProcessorEditor (&p), info_button_ (juce::Colours::darkgrey), audioProcessor (p)
{
    constexpr int TEXT_BOX_SIZE = 25;

    SetupTrianglePattern();

    for (const auto& component : triangle_pattern_)
    {
        addAndMakeVisible (*component);
    }

    reverb_slider_.setColour (juce::Slider::thumbColourId, juce::Colours::chocolate);
    reverb_slider_.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    reverb_slider_.setTextBoxStyle (juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    reverb_slider_.addListener (this);
    addAndMakeVisible (reverb_slider_);

    reverb_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "REVERB", reverb_slider_);

    mix_slider_.setColour (juce::Slider::thumbColourId, juce::Colours::chocolate);
    mix_slider_.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mix_slider_.setTextBoxStyle (juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    mix_slider_.addListener (this);
    addAndMakeVisible (mix_slider_);

    mix_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (audioProcessor.apvts, "MIX", mix_slider_);

    info_button_.addToEditor (this);
    setSize (400, 400 + TOP_SECTION_HEIGHT);
}

EasyverbAudioProcessorEditor::~EasyverbAudioProcessorEditor()
{
}

//==============================================================================
void EasyverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::lemonchiffon);
    g.setColour (juce::Colours::lemonchiffon);
    g.fillRect (top_section_);

    g.setColour (juce::Colour (juce::Colours::chocolate));
    g.setFont (juce::Font (40.0f, juce::Font::bold));
    g.drawFittedText ("EASYVERB", getLocalBounds(), juce::Justification::centredTop, 1);

    g.setColour (juce::Colours::darkorange);
    SetupSections();
    g.setFont (juce::Font (18.0f));
    g.drawFittedText ("REVERB", reverb_text_section_, juce::Justification::centred, 1);
    g.drawFittedText ("DRY/WET", mix_text_section_, juce::Justification::centred, 1);
}

void EasyverbAudioProcessorEditor::resized()
{
    info_button_.button.setBounds (getWidth() - 35, 10, 20, 20);
    info_button_.info_text.setBounds (30, 50, getWidth() - 60, getHeight() - 100);
    SetupSections();
    reverb_slider_.setBounds (reverb_section_);
    mix_slider_.setBounds (mix_section_);

    for (auto& component : triangle_pattern_)
    {
        component->setBounds (getLocalBounds().withSizeKeepingCentre (400, 400 + TOP_SECTION_HEIGHT)); // Should be same as whole screen
    }
}

void EasyverbAudioProcessorEditor::SetupTrianglePattern()
{
    const int component_width = 50;
    const int component_height = 50;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (i > 2 && i < 5 && j > 1 && j < 5)
            {
                // dont draw in middle of the window to make room for knobs
                continue;
            }
            // Logic in if statement is to reverse triangle orientation depending on where
            // in the main window it is, every second row is flipped horisontaly (j % 2) and on right
            // side of screen they are flipped vertically (i >= 4)...
            if (j % 2 == 0 && i < 4 || j % 2 == 1 && i >= 4)
            {
                // ... also every second triangle is flipped on each row
                if (i % 2 == 0)
                {
                    triangle_pattern_.push_back (std::make_unique<AnimatedTriangle> (
                        juce::Point<float> (static_cast<float> (i * component_width),
                                            static_cast<float> (j * component_height + TOP_SECTION_HEIGHT)),
                        juce::Point<float> (static_cast<float> ((i + 1) * component_width),
                                            static_cast<float> ((j + 1) * component_height + TOP_SECTION_HEIGHT)),
                        juce::Point<float> (static_cast<float> (i * component_width),
                                            static_cast<float> ((j + 1) * component_height + TOP_SECTION_HEIGHT))));
                }
                else
                {
                    triangle_pattern_.push_back (std::make_unique<AnimatedTriangle> (
                        juce::Point<float> (static_cast<float> (i * component_width),
                                            static_cast<float> (j * component_height + TOP_SECTION_HEIGHT)),
                        juce::Point<float> (static_cast<float> ((i + 1) * component_width),
                                            static_cast<float> (j * component_height + TOP_SECTION_HEIGHT)),
                        juce::Point<float> (static_cast<float> ((i + 1) * component_width),
                                            static_cast<float> ((j + 1) * component_height + TOP_SECTION_HEIGHT))));
                }
            }
            else
            {
                // ... also every second triangle is flipped on each row
                if (i % 2 == 0)
                {
                    triangle_pattern_.push_back (std::make_unique<AnimatedTriangle> (
                        juce::Point<float> (static_cast<float> (i * component_width),
                                            static_cast<float> (j * component_height + TOP_SECTION_HEIGHT)),
                        juce::Point<float> (static_cast<float> ((i + 1) * component_width),
                                            static_cast<float> (j * component_height + TOP_SECTION_HEIGHT)),
                        juce::Point<float> (static_cast<float> (i * component_width),
                                            static_cast<float> ((j + 1) * component_height + TOP_SECTION_HEIGHT))));
                }
                else
                {
                    triangle_pattern_.push_back (std::make_unique<AnimatedTriangle> (
                        juce::Point<float> (static_cast<float> ((i + 1) * component_width),
                                            static_cast<float> (j * component_height + TOP_SECTION_HEIGHT)),
                        juce::Point<float> (static_cast<float> ((i + 1) * component_width),
                                            static_cast<float> ((j + 1) * component_height + TOP_SECTION_HEIGHT)),
                        juce::Point<float> (static_cast<float> (i * component_width),
                                            static_cast<float> ((j + 1) * component_height + TOP_SECTION_HEIGHT))));
                }
            }
        }
    }
}

void EasyverbAudioProcessorEditor::SetupSections()
{
    juce::Rectangle<int> r = getLocalBounds();
    top_section_ = r.removeFromTop (TOP_SECTION_HEIGHT);
    r.removeFromTop (100);
    r.removeFromBottom (150);
    r.removeFromLeft (150);
    r.removeFromRight (150);

    constexpr int text_section_height = 20;

    juce::Rectangle<int> interface_section = r;
    int section_height = interface_section.getHeight() / 2;

    reverb_section_ = interface_section.removeFromTop (section_height);
    reverb_text_section_ = reverb_section_.removeFromTop (text_section_height);

    mix_section_ = interface_section;
    mix_text_section_ = mix_section_.removeFromTop (text_section_height);
}

void EasyverbAudioProcessorEditor::sliderValueChanged (juce::Slider* /*slider*/)
{
    repaint();
}