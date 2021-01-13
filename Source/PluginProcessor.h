/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

template<typename T>
struct Fifo
{
    
    void prepare( int numberOfSamples )
    {
        numSamples = static_cast<size_t>(numberOfSamples);
        
        for( auto& buffer : buffers )
        {
            buffer.setSize( 1, numSamples );
        }
        
    }
    
    bool push( const T& itemToAdd )
    {
        auto scopedWriter = fifo.write(1);
        
        if( scopedWriter.blockSize1 >= 1 )
        {
            return true;
        }
        
        return false;
    }
    
    bool pull( T& itemToPull )
    {
        auto scopedReader = fifo.read(1);
        
        if( scopedReader.blockSize1 >= 1 )
        {
            return true;
        }
        
        return false;
    }
    
private:
    
    static constexpr int Size = 5;
    size_t numSamples{0};
    
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

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessor)
};
