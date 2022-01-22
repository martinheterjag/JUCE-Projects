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
const int TOP_SECTION_HEIGHT = 50;

static const std::vector<juce::Point<float>> CAVE_FG_POINTS = {
    {0, 0 + TOP_SECTION_HEIGHT},
    {20, 0 + TOP_SECTION_HEIGHT},
    {62, 0 + TOP_SECTION_HEIGHT},
    {91, 0 + TOP_SECTION_HEIGHT},
    {141, 0 + TOP_SECTION_HEIGHT},
    {0, 27 + TOP_SECTION_HEIGHT},
    {11, 15 + TOP_SECTION_HEIGHT},
    {10, 28 + TOP_SECTION_HEIGHT},
    {29, 35 + TOP_SECTION_HEIGHT},
    {54, 24 + TOP_SECTION_HEIGHT},
    {70, 6 + TOP_SECTION_HEIGHT},
    {64, 26 + TOP_SECTION_HEIGHT},
    {79, 10 + TOP_SECTION_HEIGHT},
    {137, 33 + TOP_SECTION_HEIGHT},
    {116, 55 + TOP_SECTION_HEIGHT},
    {75, 29 + TOP_SECTION_HEIGHT},
    {74, 55 + TOP_SECTION_HEIGHT},
    {64, 41 + TOP_SECTION_HEIGHT},
    {131, 65 + TOP_SECTION_HEIGHT},
    {117, 91 + TOP_SECTION_HEIGHT},
    {75, 73 + TOP_SECTION_HEIGHT},
    {75, 88 + TOP_SECTION_HEIGHT},
    {75, 102 + TOP_SECTION_HEIGHT},
    {126, 107 + TOP_SECTION_HEIGHT},
    {129, 122 + TOP_SECTION_HEIGHT},
    {71, 122 + TOP_SECTION_HEIGHT},
    {82, 155 + TOP_SECTION_HEIGHT},
    {58, 145 + TOP_SECTION_HEIGHT},
    {129, 143 + TOP_SECTION_HEIGHT},
    {129, 162 + TOP_SECTION_HEIGHT},
    {132, 174 + TOP_SECTION_HEIGHT},
    {133, 206 + TOP_SECTION_HEIGHT},
    {47, 186 + TOP_SECTION_HEIGHT},
    {158, 218 + TOP_SECTION_HEIGHT},
    {191, 250 + TOP_SECTION_HEIGHT},
    {147, 250 + TOP_SECTION_HEIGHT},
    {27, 224 + TOP_SECTION_HEIGHT},
    {20, 238 + TOP_SECTION_HEIGHT},
    {25, 197 + TOP_SECTION_HEIGHT},
    {27, 174 + TOP_SECTION_HEIGHT},
    {22, 159 + TOP_SECTION_HEIGHT},
    {43, 163 + TOP_SECTION_HEIGHT},
    {39, 150 + TOP_SECTION_HEIGHT},
    {28, 135 + TOP_SECTION_HEIGHT},
    {12, 82 + TOP_SECTION_HEIGHT},
    {13, 66 + TOP_SECTION_HEIGHT},
    {0, 53 + TOP_SECTION_HEIGHT},
    {436, 69 + TOP_SECTION_HEIGHT},
    {0, 69 + TOP_SECTION_HEIGHT},
    {0, 81 + TOP_SECTION_HEIGHT},
    {0, 118 + TOP_SECTION_HEIGHT},
    {0, 150 + TOP_SECTION_HEIGHT},
    {0, 150 + TOP_SECTION_HEIGHT},
    {0, 249 + TOP_SECTION_HEIGHT},
    {223, 242 + TOP_SECTION_HEIGHT},
    {240, 249 + TOP_SECTION_HEIGHT},
    {277, 236 + TOP_SECTION_HEIGHT},
    {337, 249 + TOP_SECTION_HEIGHT},
    {399, 249 + TOP_SECTION_HEIGHT},
    {327, 206 + TOP_SECTION_HEIGHT},
    {295, 242 + TOP_SECTION_HEIGHT},
    {399, 176 + TOP_SECTION_HEIGHT},
    {399, 158 + TOP_SECTION_HEIGHT},
    {399, 120 + TOP_SECTION_HEIGHT},
    {389, 98 + TOP_SECTION_HEIGHT},
    {399, 89 + TOP_SECTION_HEIGHT},
    {384, 85 + TOP_SECTION_HEIGHT},
    {379, 70 + TOP_SECTION_HEIGHT},
    {399, 74 + TOP_SECTION_HEIGHT},
    {375, 46 + TOP_SECTION_HEIGHT},
    {375, 25 + TOP_SECTION_HEIGHT},
    {399, 0 + TOP_SECTION_HEIGHT},
    {375, 0 + TOP_SECTION_HEIGHT},
    {303, 28 + TOP_SECTION_HEIGHT},
    {253, 0 + TOP_SECTION_HEIGHT},
    {272, 10 + TOP_SECTION_HEIGHT},
    {284, 30 + TOP_SECTION_HEIGHT},
    {346, 13 + TOP_SECTION_HEIGHT},
    {195, 0 + TOP_SECTION_HEIGHT},
    {206, 9 + TOP_SECTION_HEIGHT},
    {219, 0 + TOP_SECTION_HEIGHT},
    {217, 14 + TOP_SECTION_HEIGHT},
    {224, 21 + TOP_SECTION_HEIGHT}
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
    void SetupCaveForeground();
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
