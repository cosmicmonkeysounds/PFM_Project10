#include <JuceHeader.h>
#include "Meter.h"

//==============================================================================
Meter::Meter()
{


}

Meter::~Meter()
{

}

void Meter::paint( juce::Graphics& g )
{
    auto bounds = this->getLocalBounds();
    auto h = bounds.getHeight();
    
    g.fillRect( bounds.withHeight(h*currentLevel).withY(h * (1.f-currentLevel)) );
}

void Meter::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void Meter::update( float newLevel )
{
    currentLevel = newLevel;
    repaint();
}
