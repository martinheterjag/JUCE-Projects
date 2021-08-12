/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EasyverbAudioProcessor::EasyverbAudioProcessor()
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
    params_.roomSize = 0.8f;
    params_.damping = 0.8f;
    params_.width = 0.5f;
    params_.freezeMode = 0.3f;
    params_.dryLevel = 0.0f;
    params_.wetLevel = 1.0f;
}

EasyverbAudioProcessor::~EasyverbAudioProcessor()
{
}

//==============================================================================
const juce::String EasyverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EasyverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EasyverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EasyverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EasyverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EasyverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EasyverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EasyverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EasyverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void EasyverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EasyverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec = { sampleRate, samplesPerBlock, getMainBusNumOutputChannels() };
    
    reverb_.prepare(spec);

    mix_.prepare(spec);
}

void EasyverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EasyverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void EasyverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    mix_.pushDrySamples(buffer);

    //for (int channel = 0; channel < totalNumInputChannels; ++channel)
    //{
    //    auto* channelData = buffer.getWritePointer (channel);

    //    // ..do something to the data...
    //}
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto contextToUse = juce::dsp::ProcessContextReplacing<float>(block);

    reverb_.setParameters(params_);
    reverb_.process(contextToUse);

    mix_.setWetMixProportion(0.2f);
    mix_.mixWetSamples(block);
}

//==============================================================================
bool EasyverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EasyverbAudioProcessor::createEditor()
{
    return new EasyverbAudioProcessorEditor (*this);
}

//==============================================================================
void EasyverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void EasyverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

juce::AudioProcessorValueTreeState::ParameterLayout EasyverbAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    //parameters.push_back(std::make_unique<juce::AudioParameterFloat>("COMPRESS", "Compress", 0.04f, 0.45f, 0.1f));
    //parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VIBRATO", "Vibrato", 0.0f, 0.15f, 0.01f));
    //parameters.push_back(std::make_unique<juce::AudioParameterFloat>("VIBRATO_RATE", "Rate", 0.5f, 4.0f, 2.0f));
    //parameters.push_back(std::make_unique<juce::AudioParameterFloat>("TONE", "Tone", 320.1f, 4700.0f, 2000.0f));
    //parameters.push_back(std::make_unique<juce::AudioParameterFloat>("MIX", "Mix", 0.0f, 0.5f, 0.0f));
    return { parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EasyverbAudioProcessor();
}
