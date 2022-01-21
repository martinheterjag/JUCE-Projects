/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

AnimatedComponent::AnimatedComponent( juce::Point<float> a,
                                      juce::Point<float> b,
                                      juce::Point<float> c)
{
    backgroundColor_ = juce::Colours::black;
    shape_.addTriangle(a, b, c);
    juce::Desktop::getInstance().addGlobalMouseListener(this);
}

AnimatedComponent::~AnimatedComponent()
{
    juce::Desktop::getInstance().removeGlobalMouseListener(this);
}

void AnimatedComponent::paint(juce::Graphics& g)
{
    //g.fillAll(juce::Colours::beige);
    g.setColour(backgroundColor_);
    g.fillPath(shape_);
}

void AnimatedComponent::mouseMove(const juce::MouseEvent& e)
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
    std::string s = "mouse_over_shape_ == " + std::to_string(mouse_over_shape_) + 
                    " mouse pos: " + std::to_string(mouse_pos_.getX()) +  
                    " ," + std::to_string(mouse_pos_.getY());
    DBG(s);
}

void AnimatedComponent::mouseEnter(const juce::MouseEvent& e)
{
}

void AnimatedComponent::mouseExit(const juce::MouseEvent& e)
{
}

void AnimatedComponent::timerCallback()
{
    // This callback triggers a change and repaints the component
    if (mouse_over_shape_) {
        if (current_frame_ < max_frame_) {
            current_frame_++;
        }
    }
    else {
        if (current_frame_ > 0) {
            current_frame_--;
        }
    }

    backgroundColor_ = juce::Colours::black.interpolatedWith(juce::Colours::aquamarine,
        float(current_frame_) / float(max_frame_));
    repaint();
    // next frame!
    if (current_frame_ != max_frame_) {
        startTimer(FRAME_PERIOD_MS);
    }
}

//==============================================================================
EasyverbAudioProcessorEditor::EasyverbAudioProcessorEditor (EasyverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), info_button_(juce::Colours::darkgrey)
{
    constexpr int TEXT_BOX_SIZE = 25;

    SetupCaveForeground();

    // TODO: Move down below header.
    for (auto &component : cave_foreground_) {
        addAndMakeVisible(*component);
    }

    reverb_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    reverb_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    reverb_slider_.addListener(this);
    addAndMakeVisible(reverb_slider_);

    reverb_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "REVERB", reverb_slider_);

    mix_slider_.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mix_slider_.setTextBoxStyle(juce::Slider::NoTextBox, true, TEXT_BOX_SIZE, TEXT_BOX_SIZE);
    mix_slider_.addListener(this);
    addAndMakeVisible(mix_slider_);

    mix_slider_attachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>
        (audioProcessor.apvts, "MIX", mix_slider_);

    info_button_.addToEditor(this);
    setSize(400, 300);
}

EasyverbAudioProcessorEditor::~EasyverbAudioProcessorEditor()
{
}

//==============================================================================
void EasyverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colour(0xff123456));
    g.setColour(juce::Colours::azure);
    g.fillRect(top_section_);

    
    g.setColour (juce::Colour(0xff123456));
    g.setFont(40.0f);
    g.drawFittedText("EASYVERB", getLocalBounds(), juce::Justification::centredTop, 1);

    g.setColour(juce::Colours::azure);
    SetupSections();
    g.setFont(18.0f);
    g.drawFittedText("REVERB", reverb_text_section_, juce::Justification::left, 1);
    g.drawFittedText("DRY/WET", mix_text_section_, juce::Justification::left, 1);
    // TODO: instead of drawing dots, use the vector of Points to create a vector of animated component triangles.
    for (juce::Point<float> p : CAVE_FG_POINTS) {
        juce::Rectangle<float> r(p, juce::Point<float>(p.getX() + 1, p.getY() + 1));
        g.fillRect(r);
    }
}

void EasyverbAudioProcessorEditor::resized()
{
    info_button_.button.setBounds(getWidth() - 35, 10, 20, 20);
    info_button_.info_text.setBounds(30, 50, getWidth() - 60, getHeight() - 100);
    SetupSections();
    reverb_slider_.setBounds(reverb_section_);
    mix_slider_.setBounds(mix_section_);

    for (auto component : cave_foreground_) {
        component->setBounds(getLocalBounds().withSizeKeepingCentre(400, 300));  // Should be same as whole screen
    }
}

void EasyverbAudioProcessorEditor::SetupCaveForeground() {
    // TODO:Create a constant public vector instead of this datamember and init function...
    cave_foreground_.push_back(std::make_unique<AnimatedComponent>(
        CAVE_FG_POINTS[0],
        CAVE_FG_POINTS[1],
        CAVE_FG_POINTS[6]));
    cave_foreground_.push_back(std::make_unique<AnimatedComponent>(
        CAVE_FG_POINTS[1],
        CAVE_FG_POINTS[2],
        CAVE_FG_POINTS[6]));
}

void EasyverbAudioProcessorEditor::SetupSections()
{
    juce::Rectangle<int> r = getLocalBounds();
    top_section_ = r.removeFromTop(50);
    r.removeFromTop(50);
    r.removeFromBottom(60);
    r.removeFromLeft(100);
    r.removeFromRight(100);

    constexpr int text_section_width = 70;

    juce::Rectangle<int> interface_section = r;
    int section_height = interface_section.getHeight() / 2;

    reverb_section_ = interface_section.removeFromTop(section_height);
    reverb_text_section_ = reverb_section_.removeFromLeft(text_section_width);
    
    mix_section_ = interface_section;
    mix_text_section_ = mix_section_.removeFromLeft(text_section_width);
}

void EasyverbAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    repaint();
}