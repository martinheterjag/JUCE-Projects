/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <math.h>

//==============================================================================
ReelsAudioProcessor::ReelsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

ReelsAudioProcessor::~ReelsAudioProcessor()
{
}

//==============================================================================
const juce::String ReelsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ReelsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ReelsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ReelsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ReelsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ReelsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ReelsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ReelsAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ReelsAudioProcessor::getProgramName (int index)
{
    return {};
}

void ReelsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ReelsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    grain_buffer_.clear();
}

void ReelsAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ReelsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ReelsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    int grain_size = 20000;
    int offset = 10000;
    if (grain_buffer_.getNumSamples() != grain_size)
        grain_buffer_.setSize(totalNumInputChannels, grain_size, true);

    float record_treshold = 0.075f;
    juce::AudioBuffer<float> dry = buffer;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            // Fill grain buffer if magnitude above threshold
            if (dry.getMagnitude(channel, 0, dry.getNumSamples()) > record_treshold)
            {
                if (grain_rec_position_[channel] >= grain_buffer_.getNumSamples())
                {
                    // Reset to start of grain if we itterated through every sample in grain.
                    grain_rec_position_[channel] = 0;
                }
                float sample_to_record = *buffer.getReadPointer(channel, sample);
                float sample_to_overwrite = *grain_buffer_.getReadPointer(channel, grain_rec_position_[channel]);
                // TODO: set these magic numbers by sliders.
                // TODO: figure out how to filter out the articfacts when recording.
                *grain_buffer_.getWritePointer(channel, grain_rec_position_[channel]) = (sample_to_record * 0.25 + sample_to_overwrite * 0.60);
                grain_rec_position_[channel]++;
            }
        }

        for (int sample = 0; sample < buffer.getNumSamples(); sample++)
        {
            // Play from buffer
            int sample_index_to_play = grain_play_position_[channel] - offset;
            float next_sample_to_play = *grain_buffer_.getReadPointer(channel, abs(sample_index_to_play) % grain_buffer_.getNumSamples());
            *buffer.getWritePointer(channel, sample) = *buffer.getReadPointer(channel, sample) + next_sample_to_play;

            grain_play_position_[channel]++;
        }
    }
    // TODO: Try to fade out the grain buffer slowly.
}

//==============================================================================
bool ReelsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ReelsAudioProcessor::createEditor()
{
    return new ReelsAudioProcessorEditor (*this);
}

//==============================================================================
void ReelsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ReelsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout ReelsAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GRAIN_SIZE", "Grain Size", 0, 30000, 2000));
    return { parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ReelsAudioProcessor();
}
