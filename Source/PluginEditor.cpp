/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

void Meter::paint( juce::Graphics& g )
{
    
    g.fillAll( juce::Colours::black );
    
    auto bounds = this->getLocalBounds();
    float heightOfWindow = bounds.getHeight();
    
    //DBG( "Painting meter with level: " << currentLevel );
    
    auto levelMappedHeight = jmap( currentLevel,
                                   NEGATIVE_INFINITY_DB, MAX_DB,
                                   heightOfWindow, 0.f
                            );
    
    g.setColour( Colours::pink );
    g.fillRect( bounds.withHeight(heightOfWindow).withY(levelMappedHeight) );
}

void Meter::resized()
{
    DBG("Meter::resized()");
    
    ticks.clear();
    
    int meterBoxHeight = getHeight();
    float y = NEGATIVE_INFINITY_DB;
    
    for( int i = 0; i <= (int)numberOfSteps; ++i )
    {
        Tick tick;
        tick.db = y;
        tick.y = juce::jmap( (int)y,
                             (int)NEGATIVE_INFINITY_DB, (int)MAX_DB,
                             meterBoxHeight, 0
                            );
        
        DBG( "Tick " << i << " db: " << tick.db << " y: " << tick.y );
        
        ticks.push_back( tick );
        y += dbStepSize;
    }
    
    
}

void Meter::update( float newLevel )
{
    currentLevel = newLevel;
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
    DBG("DB_Scale::resized()");
}


//==============================================================================


Pfmcpp_project10AudioProcessorEditor::Pfmcpp_project10AudioProcessorEditor (Pfmcpp_project10AudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
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
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    DBG("Editor::resized()");
    
    auto bounds = getBounds();
    
    testMeter.setBounds( bounds.getWidth()/2,
                         JUCE_LIVE_CONSTANT(75),
                         30,
                         JUCE_LIVE_CONSTANT(bounds.getHeight()-150)
                        );
    
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
