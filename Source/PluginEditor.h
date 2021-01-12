/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================


struct GUIBuffer : Thread, Timer, Component
{
    
    
private:
    
};


//==============================================================================


class Pfmcpp_project10AudioProcessorEditor  : public AudioProcessorEditor
{
public:
    Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor&);
    ~Pfmcpp_project10AudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Pfmcpp_project10AudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pfmcpp_project10AudioProcessorEditor)
};
