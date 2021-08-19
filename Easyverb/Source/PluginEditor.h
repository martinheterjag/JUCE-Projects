/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <InfoButton.h>

 
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
