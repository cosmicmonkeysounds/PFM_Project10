/*
  ==============================================================================

    Meter.h
    Created: 15 Jan 2021 10:45:41pm
    Author:  l33t h4x0r

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class Meter  : public juce::Component
{
public:
    Meter();
    ~Meter() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void update(float);

private:
    
    float currentLevel{0.f};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Meter)
};
