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
class ReelsAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ReelsAudioProcessorEditor (ReelsAudioProcessor&);
    ~ReelsAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Slider grain_size_slider_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        grain_size_slider_attachment_;
 
    ReelsAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ReelsAudioProcessorEditor)
};
