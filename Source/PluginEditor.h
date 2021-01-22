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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessorEditor)
};
