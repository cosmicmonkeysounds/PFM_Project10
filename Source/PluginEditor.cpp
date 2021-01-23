/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

ValueHolder::ValueHolder()
{
    startTimerHz(10);
}

ValueHolder::~ValueHolder()
{
    stopTimer();
}

void ValueHolder::timerCallback()
{
    if( peakTime - now > holdTime )
    {
        //DBG( "timerCallback(): peakvalue over time!" );
        currentValue = threshold;
    }
    
    now = juce::Time::getMillisecondCounterHiRes();
//    DBG( "Current Time: " << now );
//    DBG( "PeakTime:     " << peakTime );
}

void ValueHolder::setThreshold(float newThreshold)
{
    threshold = newThreshold;
}

void ValueHolder::updateHeldValue(float input)
{

    if( input > threshold )
    {
        //DBG( "updateHeldValue(): Input is over threshold!" );
        currentValue = input;
        peakTime = juce::Time::getMillisecondCounterHiRes();
    }
    
    else
    {
        resetCurrentValue();
    }
}

void ValueHolder::setHoldTime(int ms)
{
    holdTime = ms;
}

float ValueHolder::getCurrentValue() const
{
    return currentValue;
}

bool ValueHolder::isOverThreshold() const
{
    if( currentValue > threshold )
    {
        return true;
    }
    
    return false;
}

//==============================================================================

void Meter::paint( juce::Graphics& g )
{
    
    g.fillAll( juce::Colours::black );
    
    auto bounds = this->getLocalBounds();
    float heightOfWindow = bounds.getHeight();
    
    //DBG( "Painting meter with level: " << currentLevel );
    
    auto levelMappedHeight = jmap( currentLevel,
                                   NEGATIVE_INFINITY_DB, MAX_DB,
                                   heightOfWindow, 0.f );
    
    g.setColour( Colours::pink );
    g.fillRect( bounds.withHeight(heightOfWindow).withY(levelMappedHeight) );
}

void Meter::resized()
{
    
    ticks.clear();
    
    int meterBoxHeight = getHeight();
    float y = NEGATIVE_INFINITY_DB;
    
    for( int i = 0; i <= (int)numberOfSteps; ++i )
    {
        Tick tick;
        tick.db = y;
        tick.y = juce::jmap( (int)y,
                             (int)NEGATIVE_INFINITY_DB, (int)MAX_DB,
                             meterBoxHeight, 0 );
        
        ticks.push_back( tick );
        y += dbStepSize;
    }
    
}

void Meter::update( float newLevel )
{
    currentLevel = newLevel;
    testValueHolder.updateHeldValue( currentLevel );
    repaint();
}


//==============================================================================


void DB_Scale::paint( juce::Graphics& g )
{
    g.setColour( juce::Colours::white );
    
    for( auto tick : ticks )
        g.drawSingleLineText( juce::String(tick.db), 5, tick.y + yOffset );

}

void DB_Scale::resized()
{

}


//==============================================================================


Pfmcpp_project10AudioProcessorEditor::Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    
    startTimerHz( 20 );
    
    addAndMakeVisible( testMeter );
    addAndMakeVisible( testScale );
    
    setSize (800, 640);
    
}

Pfmcpp_project10AudioProcessorEditor::~Pfmcpp_project10AudioProcessorEditor()
{
    stopTimer();
}

//==============================================================================
void Pfmcpp_project10AudioProcessorEditor::paint (Graphics& g)
{

}

void Pfmcpp_project10AudioProcessorEditor::resized()
{
    
    auto bounds = getBounds();
    
    testMeter.setBounds( bounds.getWidth()/2,
                         JUCE_LIVE_CONSTANT(75),
                         30,
                         JUCE_LIVE_CONSTANT(bounds.getHeight()-150) );
    
    testScale.ticks   = testMeter.ticks;
    testScale.yOffset = testMeter.getY();
    
    testScale.setBounds( testMeter.getRight(), 0, 30, getHeight() );
 
}

void Pfmcpp_project10AudioProcessorEditor::timerCallback()
{
    if( processor.fifo.pull(buffer) )
    {

        auto leftRMSLevel = buffer.getRMSLevel( 0, 0, buffer.getNumSamples() );
        
#if SINE_OSC_TEST
        leftRMSLevel = buffer.getMagnitude( 0, 0, buffer.getNumSamples() );
#endif
        
        auto leftRMSdB    = juce::Decibels::gainToDecibels( leftRMSLevel );
        testMeter.update( leftRMSdB );
    }
}
