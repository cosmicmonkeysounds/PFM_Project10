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
        buffer.setSize( 1, numSamples );
        prepared.set( true );
    }
    
    bool push( const T& itemToAdd )
    {
        
    }
    
    bool pull( T& thingToPull )
    {
        
    }
    
    AudioBuffer<float>& getBuffer()
    {
        return buffer;
    }
    
    size_t getNumSamples()
    {
        return numSamples;
    }
    
private:
    
    AudioBuffer<float> buffer;
    size_t numSamples{0};
    Atomic<bool> prepared{false};
    
};

template<int NumChannels>
struct FifoHolder : Thread, Timer, Component
{
    
    FifoHolder()
        : Thread( "FifoHolder Thread" )
    {
        startThread();
        startTimerHz( 20 );
    }
    
    ~FifoHolder()
    {
        notify();
        stopThread( 100 );
    }
    
    void paint( Graphics& g ) override
    {
        
    }
    
    void timerCallback() override
    {
        
    }
    
    void run() override
    {
        while( true )
        {
            wait( -1 );
            DBG( "FifoHolder:run()" );
            
            if( threadShouldExit() )
                return;
        }
    }
    
    void cloneBuffers( AudioBuffer<float>& bufferFromProcessLoop )
    {
        
        dsp::AudioBlock<float> audioBlock( bufferFromProcessLoop );
        
        jassert( audioBlock.getNumChannels() == NumChannels );
        
        for( int i = 0; i < NumChannels; ++i )
        {
            jassert( audioBlock.getSingleChannelBlock(i).getNumSamples() <= fifos.at(i).getNumSamples() );
            
            fifos.at(i).getBuffer().clear();
        }
        
        notify();
    }
    
    std::array<Fifo<AudioBuffer<float>>, NumChannels>& getFifos()
    {
        return fifos;
    }
  
private:
    
    std::array<Fifo<AudioBuffer<float>>, NumChannels> fifos;
    
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
    
    FifoHolder<2> fifoHolder;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessor)
};
