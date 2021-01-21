/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


class Meter  : public juce::Component
{
public:
    Meter();
    ~Meter() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void update(float);
    
    float negativeInfinityDB{-120.f},
          maxDB{0.f};

private:
    
    float currentLevel{0.f};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Meter)
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessorEditor)
};
