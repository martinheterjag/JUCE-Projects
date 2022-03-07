/*
  ==============================================================================

    AnimatedTriangle.h
    Created: 7 Mar 2022 8:21:13pm
    Author:  Martin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const int FRAME_PERIOD_MS = 16;

class AnimatedTriangle : public juce::Component, public juce::Timer
{
public:
    AnimatedTriangle(const juce::Point<float> a,
        const juce::Point<float> b,
        const juce::Point<float> c);
    AnimatedTriangle(const AnimatedTriangle&&) {}
    AnimatedTriangle(const AnimatedTriangle&) = default;
    ~AnimatedTriangle();
    void paint(juce::Graphics&) override;

    void mouseMove(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    void timerCallback() override;

private:
    // Data members
    juce::Colour triangle_colour_;
    juce::Colour animation_colour_;
    int current_frame_{ 0 };
    int max_frame_{ 500 };
    juce::Path shape_;
    juce::Point<int> mouse_pos_;
    bool mouse_over_shape_ = false;
};