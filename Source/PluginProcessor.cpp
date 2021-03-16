/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Pfmcpp_project10AudioProcessor::Pfmcpp_project10AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), valueTree( juce::Identifier("Parameters") )
#endif
{
}

Pfmcpp_project10AudioProcessor::~Pfmcpp_project10AudioProcessor()
{
    
}

//==============================================================================
const String Pfmcpp_project10AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Pfmcpp_project10AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Pfmcpp_project10AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Pfmcpp_project10AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Pfmcpp_project10AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Pfmcpp_project10AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Pfmcpp_project10AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Pfmcpp_project10AudioProcessor::setCurrentProgram (int index)
{
}

const String Pfmcpp_project10AudioProcessor::getProgramName (int index)
{
    return {};
}

void Pfmcpp_project10AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Pfmcpp_project10AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    fifo.prepare( this->getTotalNumOutputChannels(), samplesPerBlock );
    
#if SINE_OSC_TEST
    
    juce::dsp::ProcessSpec testOscSpec;
    testOscSpec.sampleRate       = sampleRate;
    testOscSpec.numChannels      = getTotalNumOutputChannels();
    testOscSpec.maximumBlockSize = samplesPerBlock;
    
    sinOsc.prepare( testOscSpec );
    sinOsc.setFrequency( 120.f );
    testOscGain.prepare( testOscSpec );
    testOscGain.setGainDecibels( -6.f );
    
#endif
}

void Pfmcpp_project10AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Pfmcpp_project10AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void Pfmcpp_project10AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();


    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

#if SINE_OSC_TEST
    juce::dsp::AudioBlock<float> testAudioBlock{ buffer };
    juce::dsp::ProcessContextReplacing<float> process{ testAudioBlock };
    sinOsc.process( process );
    testOscGain.setGainDecibels( -6.f );
    testOscGain.process( process );
#endif

    fifo.push( buffer );
    
#if SINE_OSC_TEST
    buffer.clear();
#endif
}

//==============================================================================
bool Pfmcpp_project10AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Pfmcpp_project10AudioProcessor::createEditor()
{
    return new Pfmcpp_project10AudioProcessorEditor (*this);
}

//==============================================================================
void Pfmcpp_project10AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<juce::XmlElement> xml (valueTree.createXml());
    copyXmlToBinary (*xml, destData);
}

void Pfmcpp_project10AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (valueTree.getType()))
            valueTree = juce::ValueTree::fromXml (*xmlState);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Pfmcpp_project10AudioProcessor();
}
