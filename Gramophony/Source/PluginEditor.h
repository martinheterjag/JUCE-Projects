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
class GramophonyAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GramophonyAudioProcessorEditor (GramophonyAudioProcessor&);
    ~GramophonyAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void DrawThreePointLine(juce::Graphics& , float x1, float y1, 
                            float x2, float y2, float x3, float y3);
    void resized() override;

private:
    void SetupSections();

    typedef std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> SliderAttatchmentPtr;

    juce::Slider compress_slider_;
    SliderAttatchmentPtr compress_slider_attachment_;
    
    juce::Slider vibrato_slider_;
    SliderAttatchmentPtr vibrato_slider_attachment_;

    juce::Slider mix_slider_;
    SliderAttatchmentPtr mix_slider_attachment_;

    juce::ToggleButton mix_button_;

    juce::Rectangle<int> top_section_;
    juce::Rectangle<int> picture_section_;
    juce::Rectangle<int> interface_section_;
    juce::Rectangle<int> comp_section_;
    juce::Rectangle<int> filter_section_;
    juce::Rectangle<int> vibrato_section_;
    juce::Rectangle<int> mix_section_;

    GramophonyAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GramophonyAudioProcessorEditor)
};
