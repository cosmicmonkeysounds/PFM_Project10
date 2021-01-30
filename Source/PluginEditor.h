/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#define NEGATIVE_INFINITY_DB -66.f
#define MAX_DB                12.f

//==============================================================================


struct ValueHolder : Timer
{
    
    ValueHolder();
    ~ValueHolder() override;
    
    void timerCallback() override;
    
    void setThreshold(float newThreshold);
    void updateHeldValue(float input);
    void setHoldTime(int ms);
    float getCurrentValue() const;
    bool isOverThreshold() const;
    
private:
    
    int holdTime{100};
    
    float currentValue{ NEGATIVE_INFINITY_DB },
          threshold{0.f};
    
    juce::int64 peakTime = juce::Time::currentTimeMillis();
    
    void resetCurrentValue() { currentValue = threshold; }
};


//==============================================================================


struct DecayingValueHolder : Timer
{
    DecayingValueHolder();
    ~DecayingValueHolder() override;
    
    void timerCallback() override;
    
    void updateHeldValue(float);
    void setDecayRate(float);
    void setHoldTime(int);
    
    float getCurrentValue() { return currentValue; }

private:
    
    const int timerHz{20};
    int exponent{1}, holdTime{250};
    float currentValue{NEGATIVE_INFINITY_DB},
          decayRateDB{ 6.f / (float)timerHz },
          dx{0.f};
    
    juce::int64 elapsedTime = juce::Time::currentTimeMillis();
};


//==============================================================================


struct Tick
{
    int y{0};
    float db{0.f};
};


//==============================================================================


class Meter : public juce::Component
{
public:
    Meter() = default;
    ~Meter() override = default;

    void paint( juce::Graphics& ) override;
    void resized() override;
    
    void update(float);
    
    std::vector<Tick> ticks;

private:
    
    float currentLevel{0.f};
    
    const float dbStepSize = 6.f;
    const float numberOfSteps = (MAX_DB - NEGATIVE_INFINITY_DB) / dbStepSize;
    
    DecayingValueHolder decayingValueHolder;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Meter )
};


//==============================================================================


class DB_Scale : public juce::Component
{
public:
    DB_Scale() = default;
    ~DB_Scale() override = default;
    
    void paint( juce::Graphics& ) override;
    void resized() override;
    
    std::vector<Tick> ticks;
    int yOffset{0};
    
private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( DB_Scale )
};


//==============================================================================


class TextMeter : public juce::Component
{
public:
    
    TextMeter() = default;
    ~TextMeter() override = default;
    
    void paint(Graphics&) override;
    void resized() override;
    void update(float);

private:
    
    const int decimalPlaces = 2;
    float inputValue = NEGATIVE_INFINITY_DB;
    ValueHolder valueHolder;
    
};


//==============================================================================


template<typename T>
struct Averager
{
    Averager( size_t numElements, T initialValue )
    {
        resize( numElements, initialValue );
    }

    void clear( T initialValue )
    {
        for( auto& e : elementsToAverage )
            e = 0;
        
        writeIndex.store(0);
        sumOfElements.store( initialValue * getSize() );
        average.store( initialValue );
    }
    
    void resize( size_t newSize, T initialValue )
    {
        elementsToAverage.resize( newSize );
        clear( initialValue );
    }

    void add( T t )
    {
        T currSum = sumOfElements.load();
        int currWriteIndex = writeIndex.load();
        
        currSum -= elementsToAverage[currWriteIndex];
        currSum += t;
        elementsToAverage[currWriteIndex] = t;
        
        if( ++currWriteIndex == getSize() )
            currWriteIndex = 0;
        
        writeIndex.store( currWriteIndex );
        sumOfElements.store( currSum );
        average.store( (float)currSum / (float)getSize() );
    }
        
    float getAverage() const { return average.load(); }
    size_t getSize() const { return elementsToAverage.size(); }
    
private:
    
    std::vector<T> elementsToAverage;
    std::atomic<T> sumOfElements{ static_cast<T>(0) };
    std::atomic<int> writeIndex;
    std::atomic<float> average{ 0.f };
    
};


//==============================================================================


struct MacroMeterWidget : juce::Component
{
    
    MacroMeterWidget();

    void paint(juce::Graphics&) override;
    void resized() override;
    
    void update(float);
    
    std::vector<Tick> getTicks();
    int getMeterY();
    
private:
    
    Meter instantMeter, averageMeter;
    TextMeter textMeter;
    Averager<float> averager;
    
};


//==============================================================================


class Pfmcpp_project10AudioProcessorEditor  : public AudioProcessorEditor, public Timer
{
public:
    Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor&);
    ~Pfmcpp_project10AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void timerCallback() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Pfmcpp_project10AudioProcessor& processor;
    
    AudioBuffer<float> buffer;
    
    MacroMeterWidget testMacroMeter;
    DB_Scale testScale;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessorEditor)
};
