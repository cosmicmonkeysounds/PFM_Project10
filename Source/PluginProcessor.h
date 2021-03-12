/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#define SINE_OSC_TEST false

template<typename T>
struct Fifo
{
    
    void prepare( int numberOfChannels, int numberOfSamples )
    {
    
        for( auto& buffer : buffers )
        {
            buffer.setSize( numberOfChannels, numberOfSamples );
        }
        
    }
    
    bool push( const T& itemToAdd )
    {
        auto scopedWriter = fifo.write(1);
        
        if( scopedWriter.blockSize1 >= 1 )
        {
            auto& buffer = buffers[scopedWriter.startIndex1];
            buffer = itemToAdd;
            return true;
        }
        
        return false;
    }
    
    bool pull( T& itemToPull )
    {
        auto scopedReader = fifo.read(1);
        
        if( scopedReader.blockSize1 >= 1 )
        {
            auto& buffer = buffers[scopedReader.startIndex1];
            itemToPull = buffer;
            return true;
        }
        
        return false;
    }
    
    int numberAvailable() { return fifo.getNumReady(); }
    
private:
    
    static constexpr int Size = 5;
    std::array<T, Size> buffers;
    AbstractFifo fifo{Size};
    
};


//==============================================================================


class Pfmcpp_project10AudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    Pfmcpp_project10AudioProcessor();
    ~Pfmcpp_project10AudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    Fifo<AudioBuffer<float>> fifo;
    Fifo<AudioBuffer<float>> gonioFifo;

private:
    //==============================================================================
    
#if SINE_OSC_TEST
    juce::dsp::Oscillator<float> testOsc{ [](float x) { return std::sin(x); }};
    juce::dsp::Gain<float> testOscGain;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessor)
};
