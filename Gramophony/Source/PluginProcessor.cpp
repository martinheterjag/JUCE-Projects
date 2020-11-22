/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    juce::dsp::ProcessSpec chorus_spec = { sampleRate, samplesPerBlock, getMainBusNumOutputChannels() };
    chorus_.prepare(chorus_spec);

    juce::dsp::ProcessSpec wetdry_spec = { sampleRate, samplesPerBlock, getMainBusNumOutputChannels() };
    mix_.prepare(chorus_spec);

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
            if (*buffer.getWritePointer(channel, sample) >= treshold) {
                *buffer.getWritePointer(channel, sample) = (*buffer.getWritePointer(channel, sample) / 4) + (3 * treshold / 4);
            } else if (*buffer.getWritePointer(channel, sample) <= -treshold) {
                *buffer.getWritePointer(channel, sample) = (*buffer.getWritePointer(channel, sample) / 4) - (3 * treshold / 4);
            }
            // unity gain when threshold ~ 0.3 which is the max value.
            *buffer.getWritePointer(channel, sample) *= 2.0f - (11.0f * treshold * treshold);

        }
    }
    
    chorus_.setRate(2.0f);
    chorus_.setDepth(apvts.getRawParameterValue("VIBRATO")->load());
    chorus_.setCentreDelay(10.0f);
    chorus_.setFeedback(0.0f);
    chorus_.setMix(1.0f);
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto contextToUse = juce::dsp::ProcessContextReplacing<float>(block);
    chorus_.process(contextToUse);

    mix_.setWetMixProportion(apvts.getRawParameterValue("MIX")->load());
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

    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("COMPRESS", "Compress", 0.04f, 0.3f, 0.1f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VIBRATO", "Vibrato", 0.0f, 0.1f, 0.01f));
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "Mix", 0.0f, 1.0f, 1.0f));
    return { parameters.begin(), parameters.end() };
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GramophonyAudioProcessor();
}
