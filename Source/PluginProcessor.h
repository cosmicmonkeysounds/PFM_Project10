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
        
        prepared.set( true );
    }
    
    bool push( const T& thingToPush )
    {
        
    }
    
    bool pull( T& thingToPull )
    {
        
    }
    
private:
    
    Atomic<bool> prepared{false};
    
};

template<typename T>
struct FifoBuffer
{
    
    std::array<Fifo<T>> getFifos
    {
        return fifos;
    }
  
private:
    std::array<Fifo<T>, 2> fifos;
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
    
    FifoBuffer< AudioBuffer<float> > fifoBuffers;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessor)
};
