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

    AnimatedComponent comp;

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
