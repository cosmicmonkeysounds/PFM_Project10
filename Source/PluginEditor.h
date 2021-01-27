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
    
    std::function<void(float)> onUpdate = nullptr;
    
    std::vector<Tick> ticks;

private:
    
    float currentLevel{0.f};
    
    const float dbStepSize = 6.f;
    const float numberOfSteps = (MAX_DB - NEGATIVE_INFINITY_DB) / dbStepSize;
    
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
    
    Meter testMeter;
    DB_Scale testScale;
    TextMeter testTextMeter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessorEditor)
};
