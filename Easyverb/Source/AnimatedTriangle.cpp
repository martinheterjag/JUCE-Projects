/*
  ==============================================================================

    AnimatedTriangle.cpp
    Created: 7 Mar 2022 8:20:55pm
    Author:  Martin

  ==============================================================================
*/

#include "AnimatedTriangle.h"


AnimatedTriangle::AnimatedTriangle(juce::Point<float> a,
    juce::Point<float> b,
    juce::Point<float> c)
{
    backgroundColor_ = juce::Colours::darkorange;
    shape_.addTriangle(a, b, c);
    juce::Desktop::getInstance().addGlobalMouseListener(this);
}

AnimatedTriangle::~AnimatedTriangle()
{
    juce::Desktop::getInstance().removeGlobalMouseListener(this);
}

void AnimatedTriangle::paint(juce::Graphics& g)
{
    g.setColour(backgroundColor_);
    g.fillPath(shape_);
}

void AnimatedTriangle::mouseMove(const juce::MouseEvent& e)
{
    // Get mouse coordinates on the entire screen
    mouse_pos_ = juce::Point<int>(e.getScreenX(), e.getScreenY());
    // Move "origo" to component corner instead of screen corner
    mouse_pos_ = getLocalPoint(nullptr, mouse_pos_);


    if (shape_.contains(mouse_pos_.getX(), mouse_pos_.getY()) == true && mouse_over_shape_ == false) {
        mouse_over_shape_ = true;
        startTimer(FRAME_PERIOD_MS);
    }
    else if (shape_.contains(mouse_pos_.getX(), mouse_pos_.getY()) == false && mouse_over_shape_ == true) {
        mouse_over_shape_ = false;
        startTimer(FRAME_PERIOD_MS);
    }
}

void AnimatedTriangle::mouseEnter(const juce::MouseEvent& e)
{
}

void AnimatedTriangle::mouseExit(const juce::MouseEvent& e)
{
}

void AnimatedTriangle::timerCallback()
{
    // This callback triggers a change and repaints the component
    if (mouse_over_shape_) {
        if (current_frame_ < max_frame_) {
            // TODO: fix some cool algorithm for
            current_frame_ += 15;
            current_frame_ *= 1.8;
            // current_frame_ = 500 + (current_frame_ * (0 - 1));
        }
    }
    else {
        if (current_frame_ > 0) {
            current_frame_ /= 1.012;
        }
    }

    backgroundColor_ = juce::Colours::darkorange.interpolatedWith(juce::Colours::peachpuff,
        float(current_frame_) / float(max_frame_));
    repaint();
    // next frame!
    if (current_frame_ != max_frame_) {
        startTimer(FRAME_PERIOD_MS);
    }
}