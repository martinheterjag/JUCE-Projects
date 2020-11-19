/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class WaveFolderAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WaveFolderAudioProcessorEditor (WaveFolderAudioProcessor&);
    ~WaveFolderAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void DivideSections();
    juce::Slider gain_slider_;
    juce::Slider threshold_slider_;
    juce::Slider bias_slider_;
    juce::Slider volume_slider_;
    juce::Slider thr_lfo_rate_slider_;
    juce::Slider thr_lfo_depth_slider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        gain_slider_attachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        threshold_slider_attachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        bias_slider_attachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        volume_slider_attachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        thr_lfo_rate_slider_attachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        thr_lfo_depth_slider_attachment_;
    juce::Rectangle<int> top_section_;
    juce::Rectangle<int> gain_section_;
    juce::Rectangle<int> threshold_section_;
    juce::Rectangle<int> bias_section_;
    juce::Rectangle<int> volume_section_;
    juce::Rectangle<int> thr_lfo_rate_section_;
    juce::Rectangle<int> thr_lfo_depth_section_;

    WaveFolderAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveFolderAudioProcessorEditor)
};
