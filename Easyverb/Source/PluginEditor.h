/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <InfoButton.h>

// How many milliseconds frames per animation frame (8 ms gives 62.5 FPS)
const int FRAME_PERIOD_MS = 16;

static const std::vector<juce::Point<int>> CAVE_FG_POINTS = {
    {0, 0},
    {20, 0},
    {62, 0},
    {91, 0},
    {141, 0},
    {0, 27},
    {11, 15},
    {10, 28},
    {29, 35},
    {54, 24},
    {70, 6},
    {64, 26},
    {79, 10},
    {137, 33},
    {116, 55},
    {75, 29},
    {74, 55},
    {64, 41},
    {131, 65},
    {117, 91},
    {75, 73},
    {75, 88},
    {75, 102},
    {126, 107},
    {129, 122},
    {71, 122},
    {82, 155},
    {58, 145},
    {129, 143},
    {129, 162},
    {132, 174},
    {133, 206},
    {47, 186},
    {158, 218},
    {191, 250},
    {147, 250},
    {27, 224},
    {20, 238},
    {25, 197},
    {27, 174},
    {22, 159},
    {43, 163},
    {39, 150},
    {28, 135},
    {12, 82},
    {13, 66},
    {0, 53},
    {436, 69},
    {0, 69},
    {0, 81},
    {0, 118},
    {0, 150},
    {0, 150},
    {0, 249},
    {223, 242},
    {240, 249},
    {277, 236},
    {337, 249},
    {399, 249},
    {327, 206},
    {295, 242},
    {399, 176},
    {399, 158},
    {399, 120},
    {389, 98},
    {399, 89},
    {384, 85},
    {379, 70},
    {399, 74},
    {375, 46},
    {375, 25},
    {399, 0},
    {375, 0},
    {303, 28},
    {253, 0},
    {272, 10},
    {284, 30},
    {346, 13},
    {195, 0},
    {206, 9},
    {219, 0},
    {217, 14},
    {224, 21}
};


class AnimatedComponent : public juce::Component, public juce::Timer
{
public:
    AnimatedComponent(const juce::Point<float> a,
                      const juce::Point<float> b,
                      const juce::Point<float> c);
    AnimatedComponent(const AnimatedComponent&&) {}
    AnimatedComponent(const AnimatedComponent&) = default;
    ~AnimatedComponent();
    void paint(juce::Graphics&) override;

    void mouseMove(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    void timerCallback() override;

private:
    // Data members
    juce::Colour backgroundColor_{0xFFFF0000};
    int current_frame_{ 0 };
    int max_frame_{ 25 };
    juce::Path shape_;
    juce::Point<int> mouse_pos_;
    bool mouse_over_shape_ = false;
};
 
//==============================================================================
/**
*/
class EasyverbAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                      public juce::Slider::Listener

{
public:
    EasyverbAudioProcessorEditor (EasyverbAudioProcessor&);
    ~EasyverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void SetupSections();
    void sliderValueChanged(juce::Slider* slider) override;
    InfoButton info_button_;

    //AnimatedComponent comp;
    std::vector<std::shared_ptr<AnimatedComponent>> cave_foreground_;

    typedef std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> SliderAttatchmentPtr;
    juce::Slider reverb_slider_;
    SliderAttatchmentPtr reverb_slider_attachment_;

    juce::Rectangle<int> top_section_;
    juce::Slider mix_slider_;
    SliderAttatchmentPtr mix_slider_attachment_;
    
    juce::Rectangle<int> reverb_section_;
    juce::Rectangle<int> mix_section_;
    juce::Rectangle<int> reverb_text_section_;
    juce::Rectangle<int> mix_text_section_;


    EasyverbAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EasyverbAudioProcessorEditor)
};
