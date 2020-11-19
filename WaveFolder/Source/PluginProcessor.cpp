/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveFolderAudioProcessor::WaveFolderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
        apvts (*this, nullptr, "Parameters", createParameters())
#endif
{
}

WaveFolderAudioProcessor::~WaveFolderAudioProcessor()
{
}

//==============================================================================
const juce::String WaveFolderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WaveFolderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool WaveFolderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool WaveFolderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double WaveFolderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WaveFolderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int WaveFolderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WaveFolderAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String WaveFolderAudioProcessor::getProgramName (int index)
{
    return {};
}

void WaveFolderAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void WaveFolderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //prepare lfo
    juce::dsp::ProcessSpec lfoSpec = { sampleRate,samplesPerBlock,getMainBusNumOutputChannels() };
    thr_lfo_.prepare(lfoSpec);
    //type of oscillator
    thr_lfo_.initialise([](float x) {return std::sin(x); }, 128);
}

void WaveFolderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaveFolderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void WaveFolderAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    thr_lfo_.setFrequency(apvts.getRawParameterValue("THR_LFO_RATE")->load());
    thr_lfo_volume_.setTargetValue(apvts.getRawParameterValue("THR_LFO_DEPTH")->load());

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (auto sample = 0; sample < buffer.getNumSamples(); ++sample) {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            // Do not add any effects if volume is to low to prevent lfos to leak.
            const float gate_threshold = 0.005;
            if (*buffer.getWritePointer(channel, sample) < gate_threshold &&
                *buffer.getWritePointer(channel, sample) > -gate_threshold) {
                continue;
            }
            
            float lfo_offset = thr_lfo_.processSample(0.0f) * thr_lfo_volume_.getNextValue();
            float threshold = apvts.getRawParameterValue("THRESHOLD")->load() + lfo_offset;
            float bias = apvts.getRawParameterValue("BIAS")->load();
            float input_gain = apvts.getRawParameterValue("GAIN")->load();
            float output_gain = apvts.getRawParameterValue("VOLUME")->load();
            *buffer.getWritePointer(channel, sample) *= input_gain;
            *buffer.getWritePointer(channel, sample) += bias;
            // positive side
            if (*buffer.getWritePointer(channel, sample) > threshold) {
                *buffer.getWritePointer(channel, sample) = 
                    2 * threshold - *buffer.getWritePointer(channel, sample);
            }
            // negative side
            if (*buffer.getWritePointer(channel, sample) < 0 - threshold) {
                *buffer.getWritePointer(channel, sample) =
                    2 * (0 - threshold) - *buffer.getWritePointer(channel, sample);
            }
            *buffer.getWritePointer(channel, sample) *= output_gain;
        }
    }
}

//==============================================================================
bool WaveFolderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WaveFolderAudioProcessor::createEditor()
{
    return new WaveFolderAudioProcessorEditor (*this);
}

//==============================================================================
void WaveFolderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void WaveFolderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout WaveFolderAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 2.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("BIAS", "Bias", -0.5f, 0.5f, 0.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("THRESHOLD", "Threshold", 0.0f, 0.5f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VOLUME", "Volume", 0.0f, 2.0f, 0.5f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("THR_LFO_RATE", "Rate", 0.01f, 20.0f, 1.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("THR_LFO_DEPTH", "Depth", 0.0f, 0.2f, 0.01f));
    return { parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveFolderAudioProcessor();
}
