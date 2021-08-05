/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

constexpr float BP_FREQ = 2950.0f;

//==============================================================================
GramophonyAudioProcessor::GramophonyAudioProcessor()
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

GramophonyAudioProcessor::~GramophonyAudioProcessor()
{
}

//==============================================================================
const juce::String GramophonyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GramophonyAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GramophonyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GramophonyAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GramophonyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GramophonyAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GramophonyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GramophonyAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GramophonyAudioProcessor::getProgramName (int index)
{
    return {};
}

void GramophonyAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GramophonyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec = { sampleRate, samplesPerBlock, getMainBusNumOutputChannels() };

    chorus_.prepare(spec);

    mix_.prepare(spec);

    float frequency = apvts.getRawParameterValue("TONE")->load();
    filter_ch1_.prepare(spec);
    filter_ch1_.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, frequency, 6.0f);

    filter_ch2_.prepare(spec);
    filter_ch2_.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, frequency, 6.0f);
}

void GramophonyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GramophonyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void GramophonyAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
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

    mix_.pushDrySamples(buffer);

    for (float sample = 0; sample < buffer.getNumSamples(); ++sample) {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            // TODO: make this value tweakable.
            float treshold = apvts.getRawParameterValue("COMPRESS")->load();
            float frequency = apvts.getRawParameterValue("TONE")->load();

            if (*buffer.getReadPointer(channel, sample) >= treshold) {
                *buffer.getWritePointer(channel, sample) = (*buffer.getReadPointer(channel, sample) / 4) + (3 * treshold / 4);
            } else if (*buffer.getReadPointer(channel, sample) <= -treshold) {
                *buffer.getWritePointer(channel, sample) = (*buffer.getReadPointer(channel, sample) / 4) - (3 * treshold / 4);
            }
            // Partly calculated partly by ear set makeup gain.
            *buffer.getWritePointer(channel, sample) *= 5.0f - (11.0f * treshold * treshold);

            if (channel == 0) {
                filter_ch1_.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(getSampleRate(), frequency + 10.0f, 2.7f);
                *buffer.getWritePointer(channel, sample) = filter_ch1_.processSample(*buffer.getReadPointer(channel, sample));
            }
            else if (channel == 1) {
                filter_ch2_.coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(getSampleRate(), frequency - 10.0f, 2.73f);
                *buffer.getWritePointer(channel, sample) = filter_ch2_.processSample(*buffer.getReadPointer(channel, sample));
            }
        }
    }
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto contextToUse = juce::dsp::ProcessContextReplacing<float>(block);


    chorus_.setRate(apvts.getRawParameterValue("VIBRATO_RATE")->load());
    chorus_.setDepth(apvts.getRawParameterValue("VIBRATO")->load());
    chorus_.setCentreDelay(1.0f);
    chorus_.setFeedback(0.0f);
    chorus_.setMix(1.0f);

    chorus_.process(contextToUse);

    mix_.setWetMixProportion(1.0f - apvts.getRawParameterValue("MIX")->load());
    mix_.mixWetSamples(block);
}


//==============================================================================
bool GramophonyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GramophonyAudioProcessor::createEditor()
{
    return new GramophonyAudioProcessorEditor (*this);
}

//==============================================================================
void GramophonyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GramophonyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout GramophonyAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("COMPRESS", "Compress", 0.04f, 0.45f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VIBRATO", "Vibrato", 0.0f, 0.33f, 0.01f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VIBRATO_RATE", "Rate", 0.5f, 4.0f, 2.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("TONE", "Tone", 320.1f, 4700.0f, 2000.0f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "Mix", 0.0f, 0.5f, 0.0f));
    return { parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GramophonyAudioProcessor();
}
